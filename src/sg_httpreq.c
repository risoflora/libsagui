/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2020 Silvio Clecio <silvioprog@gmail.com>
 *
 * Sagui library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Sagui library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sagui library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <errno.h>
#include <pthread.h>
#include "sg_macros.h"
#include "microhttpd.h"
#include "sagui.h"
#include "sg_extra.h"
#include "sg_httpreq.h"
#include "sg_httpres.h"
#include "sg_httpauth.h"
#include "sg_httpsrv.h"

static void *sg__httpreq_isolate_cb(void *cls) {
  struct sg__httpreq_isolated *isolated = cls;
  isolated->cb(isolated->cls, isolated->handle, isolated->handle->res);
  sg__httpsrv_lock(isolated->handle->srv);
  LL_DELETE(isolated->handle->srv->isolated_list, isolated);
  sg__httpsrv_unlock(isolated->handle->srv);
  MHD_resume_connection(isolated->handle->con);
  sg_free(isolated);
  return NULL;
}

struct sg_httpreq *sg__httpreq_new(struct sg_httpsrv *srv,
                                   struct MHD_Connection *con,
                                   const char *version, const char *method,
                                   const char *path) {
  struct sg_httpreq *req = sg_alloc(sizeof(struct sg_httpreq));
  if (!req)
    return NULL;
  req->res = sg__httpres_new(con);
  if (!req->res)
    goto error;
  req->auth = sg__httpauth_new(req->res);
  if (!req->auth)
    goto error;
  req->payload = sg_str_new();
  if (!req->payload)
    goto error;
  req->srv = srv;
  req->con = con;
  req->version = version;
  req->method = method;
  req->path = path;
  return req;
error:
  sg__httpres_free(req->res);
  sg__httpauth_free(req->auth);
  sg_str_free(req->payload);
  return NULL;
}

void sg__httpreq_free(struct sg_httpreq *req) {
  if (!req)
    return;
  req->user_data = NULL;
  sg_strmap_cleanup(&req->headers);
  sg_strmap_cleanup(&req->cookies);
  sg_strmap_cleanup(&req->params);
  sg_strmap_cleanup(&req->fields);
  sg_str_free(req->payload);
  MHD_destroy_post_processor(req->pp);
  sg__httpres_free(req->res);
  sg__httpauth_free(req->auth);
  sg_free(req);
}

struct sg_httpsrv *sg_httpreq_srv(struct sg_httpreq *req) {
  if (req)
    return req->srv;
  errno = EINVAL;
  return NULL;
}

struct sg_strmap **sg_httpreq_headers(struct sg_httpreq *req) {
  if (!req) {
    errno = EINVAL;
    return NULL;
  }
  if (!req->headers)
    MHD_get_connection_values(req->con, MHD_HEADER_KIND, sg__convals_iter,
                              &req->headers);
  return &req->headers;
}

struct sg_strmap **sg_httpreq_cookies(struct sg_httpreq *req) {
  if (!req) {
    errno = EINVAL;
    return NULL;
  }
  if (!req->cookies)
    MHD_get_connection_values(req->con, MHD_COOKIE_KIND, sg__convals_iter,
                              &req->cookies);
  return &req->cookies;
}

struct sg_strmap **sg_httpreq_params(struct sg_httpreq *req) {
  if (!req) {
    errno = EINVAL;
    return NULL;
  }
  if (!req->params)
    MHD_get_connection_values(req->con, MHD_GET_ARGUMENT_KIND, sg__convals_iter,
                              &req->params);
  return &req->params;
}

struct sg_strmap **sg_httpreq_fields(struct sg_httpreq *req) {
  if (req)
    return &req->fields;
  errno = EINVAL;
  return NULL;
}

const char *sg_httpreq_version(struct sg_httpreq *req) {
  if (req)
    return req->version;
  errno = EINVAL;
  return NULL;
}

const char *sg_httpreq_method(struct sg_httpreq *req) {
  if (req)
    return req->method;
  errno = EINVAL;
  return NULL;
}

const char *sg_httpreq_path(struct sg_httpreq *req) {
  if (req)
    return req->path;
  errno = EINVAL;
  return NULL;
}

struct sg_str *sg_httpreq_payload(struct sg_httpreq *req) {
  if (req)
    return req->payload;
  errno = EINVAL;
  return NULL;
}

bool sg_httpreq_is_uploading(struct sg_httpreq *req) {
  if (req)
    return req->is_uploading;
  errno = EINVAL;
  return false;
}

struct sg_httpupld *sg_httpreq_uploads(struct sg_httpreq *req) {
  if (req)
    return req->uplds;
  errno = EINVAL;
  return NULL;
}

const void *sg_httpreq_client(struct sg_httpreq *req) {
  const union MHD_ConnectionInfo *info;
  if (req) {
    info =
      MHD_get_connection_info(req->con, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
    return info ? info->client_addr : NULL;
  }
  errno = EINVAL;
  return NULL;
}

#ifdef SG_HTTPS_SUPPORT

void *sg_httpreq_tls_session(struct sg_httpreq *req) {
  const union MHD_ConnectionInfo *info;
  if (req) {
    info = MHD_get_connection_info(req->con, MHD_CONNECTION_INFO_GNUTLS_SESSION,
                                   NULL);
    return info ? info->tls_session : NULL;
  }
  errno = EINVAL;
  return NULL;
}

#endif /* SG_HTTPS_SUPPORT */

int sg_httpreq_isolate(struct sg_httpreq *req, sg_httpreq_cb cb, void *cls) {
  struct sg__httpreq_isolated *isolated;
  int errnum = 0;
  if (!req || !cb)
    return EINVAL;
  sg__httpsrv_lock(req->srv);
  if (req->isolated) {
    errnum = EALREADY;
    goto error;
  }
  isolated = sg_malloc(sizeof(struct sg__httpreq_isolated));
  if (!isolated) {
    errnum = ENOMEM;
    goto error;
  }
  isolated->handle = req;
  isolated->cb = cb;
  isolated->cls = cls;
  MHD_suspend_connection(req->con);
  LL_APPEND(req->srv->isolated_list, isolated);
  req->isolated = true;
  errnum =
    pthread_create(&isolated->thread, NULL, sg__httpreq_isolate_cb, isolated);
  if (errnum != 0) {
    LL_DELETE(req->srv->isolated_list, isolated);
    sg_free(isolated);
    MHD_resume_connection(req->con);
  }
error:
  sg__httpsrv_unlock(req->srv);
  return errnum;
}

int sg_httpreq_set_user_data(struct sg_httpreq *req, void *data) {
  if (!req)
    return EINVAL;
  req->user_data = data;
  return 0;
}

void *sg_httpreq_user_data(struct sg_httpreq *req) {
  if (req)
    return req->user_data;
  errno = EINVAL;
  return NULL;
}
