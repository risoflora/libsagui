/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 *   –– an ideal C library to develop cross-platform HTTP servers.
 *
 * Copyright (c) 2016-2018 Silvio Clecio <silvioprog@gmail.com>
 *
 * This file is part of Sagui library.
 *
 * Sagui library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sagui library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Sagui library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "sg_macros.h"
#include "microhttpd.h"
#include "sagui.h"
#include "sg_httputils.h"
#include "sg_httpres.h"
#include "sg_httpreq.h"
#include "sg_httpauth.h"

struct sg_httpreq *sg__httpreq_new(struct MHD_Connection *con, const char *version, const char *method,
                                   const char *path) {
    struct sg_httpreq *req;
    sg__new(req);
    req->res = sg__httpres_new(con);
    req->auth = sg__httpauth_new(req->res);
    req->payload = sg_str_new();
    req->con = con;
    req->version = version;
    req->method = method;
    req->path = path;
    return req;
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
    sg__free(req);
}

struct sg_strmap **sg_httpreq_headers(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    if (!req->headers)
        MHD_get_connection_values(req->con, MHD_HEADER_KIND, sg__httpcon_iter, &req->headers);
    return &req->headers;
}

struct sg_strmap **sg_httpreq_cookies(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    if (!req->cookies)
        MHD_get_connection_values(req->con, MHD_COOKIE_KIND, sg__httpcon_iter, &req->cookies);
    return &req->cookies;
}

struct sg_strmap **sg_httpreq_params(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    if (!req->params)
        MHD_get_connection_values(req->con, MHD_GET_ARGUMENT_KIND, sg__httpcon_iter, &req->params);
    return &req->params;
}

struct sg_strmap **sg_httpreq_fields(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    return &req->fields;
}

const char *sg_httpreq_version(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    return req->version;
}

const char *sg_httpreq_method(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    return req->method;
}

const char *sg_httpreq_path(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    return req->path;
}

struct sg_str *sg_httpreq_payload(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    return req->payload;
}

bool sg_httpreq_is_uploading(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return false;
    }
    return req->is_uploading;
}

struct sg_httpupld *sg_httpreq_uploads(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    return req->uplds;
}

#ifdef SG_HTTPS_SUPPORT

void *sg_httpreq_tls_session(struct sg_httpreq *req) {
    const union MHD_ConnectionInfo *info;
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    return (info = MHD_get_connection_info(req->con,
                                           MHD_CONNECTION_INFO_GNUTLS_SESSION, NULL)) ? info->tls_session : NULL;
}

#endif

int sg_httpreq_set_user_data(struct sg_httpreq *req, void *data) {
    if (!req)
        return EINVAL;
    req->user_data = data;
    return 0;
}

void *sg_httpreq_user_data(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    return req->user_data;
}
