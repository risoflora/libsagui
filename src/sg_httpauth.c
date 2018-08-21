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
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "sg_macros.h"
#include "microhttpd.h"
#include "sagui.h"
#include "sg_httputils.h"
#include "sg_strmap.h"
#include "sg_httpauth.h"

struct sg_httpauth *sg__httpauth_new(struct sg_httpres *res) {
    struct sg_httpauth *auth;
    sg__new(auth);
    auth->usr = MHD_basic_auth_get_username_password(res->con, &auth->pwd);
    auth->res = res;
    return auth;
}

void sg__httpauth_free(struct sg_httpauth *auth) {
    if (!auth)
        return;
    sg__free(auth->usr);
    sg__free(auth->pwd);
    sg__free(auth->realm);
    sg__free(auth);
}

bool sg__httpauth_dispatch(struct sg_httpauth *auth) {
    if (auth->res->ret) {
        auth->res->ret = MHD_YES;
        goto done;
    }
    if (auth->canceled || !auth->res->handle) {
        auth->res->ret = MHD_NO;
        goto done;
    }
    sg_strmap_iter(auth->res->headers, sg__httpheaders_iter, auth->res->handle);
    auth->res->ret = MHD_queue_basic_auth_fail_response(auth->res->con, auth->realm ? auth->realm : _("Sagui realm"),
                                                        auth->res->handle);
    return false;
done:
    return auth->res->ret == MHD_YES;
}

int sg_httpauth_set_realm(struct sg_httpauth *auth, const char *realm) {
    if (!auth || !realm)
        return EINVAL;
    if (auth->realm)
        return EALREADY;
    if (!(auth->realm = strdup(realm)))
        oom();
    return 0;
}

const char *sg_httpauth_realm(struct sg_httpauth *auth) {
    if (!auth) {
        errno = EINVAL;
        return NULL;
    }
    return auth->realm;
}

int sg_httpauth_deny(struct sg_httpauth *auth, const char *justification, const char *content_type) {
    if (!auth || !justification || !content_type)
        return EINVAL;
    if (auth->res->handle)
        return EALREADY;
    auth->res->handle = MHD_create_response_from_buffer(strlen(justification), (void *) justification,
                                                        MHD_RESPMEM_MUST_COPY);
    return sg_strmap_add(&auth->res->headers, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);
}

int sg_httpauth_cancel(struct sg_httpauth *auth) {
    if (!auth)
        return EINVAL;
    auth->canceled = true;
    return 0;
}

const char *sg_httpauth_usr(struct sg_httpauth *auth) {
    if (!auth) {
        errno = EINVAL;
        return NULL;
    }
    return auth->usr;
}

const char *sg_httpauth_pwd(struct sg_httpauth *auth) {
    if (!auth) {
        errno = EINVAL;
        return NULL;
    }
    return auth->pwd;
}
