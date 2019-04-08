/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (c) 2016-2019 Silvio Clecio <silvioprog@gmail.com>
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
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif
#include "sg_macros.h"
#include "microhttpd.h"
#include "sagui.h"
#include "sg_extra.h"
#include "sg_httpres.h"
#include "sg_httpreq.h"
#include "sg_httpauth.h"

struct sg_httpreq *sg__httpreq_new(struct MHD_Connection *con, const char *version, const char *method,
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

struct sg_strmap **sg_httpreq_headers(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    if (!req->headers)
        MHD_get_connection_values(req->con, MHD_HEADER_KIND, sg__convals_iter, &req->headers);
    return &req->headers;
}

struct sg_strmap **sg_httpreq_cookies(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    if (!req->cookies)
        MHD_get_connection_values(req->con, MHD_COOKIE_KIND, sg__convals_iter, &req->cookies);
    return &req->cookies;
}

struct sg_strmap **sg_httpreq_params(struct sg_httpreq *req) {
    if (!req) {
        errno = EINVAL;
        return NULL;
    }
    if (!req->params)
        MHD_get_connection_values(req->con, MHD_GET_ARGUMENT_KIND, sg__convals_iter, &req->params);
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

int sg_httpreq_ip(struct sg_httpreq *req, char *ip, size_t *len) {
    const union MHD_ConnectionInfo *info;
    if (!req || !ip || !len || (ssize_t) len < 0)
        return EINVAL;
    info = MHD_get_connection_info(req->con, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
    if (info)
        switch (info->client_addr->sa_family) {
            case AF_INET:
                if (*len > INET_ADDRSTRLEN)
                    *len = INET_ADDRSTRLEN;
                if (!inet_ntop(AF_INET, &(((struct sockaddr_in *) info->client_addr)->sin_addr), ip, *len))
                    return errno;
                break;
            case AF_INET6:
                if (*len > INET6_ADDRSTRLEN)
                    *len = INET6_ADDRSTRLEN;
                if (!inet_ntop(AF_INET6, &(((struct sockaddr_in6 *) info->client_addr)->sin6_addr), ip, *len))
                    return errno;
                break;
        }
    return 0;
}

#ifdef SG_HTTPS_SUPPORT

void *sg_httpreq_tls_session(struct sg_httpreq *req) {
    const union MHD_ConnectionInfo *info;
    if (req) {
        info = MHD_get_connection_info(req->con, MHD_CONNECTION_INFO_GNUTLS_SESSION, NULL);
        return info ? info->tls_session : NULL;
    }
    errno = EINVAL;
    return NULL;
}

#endif

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
