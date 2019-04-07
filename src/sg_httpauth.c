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
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "sg_macros.h"
#include "microhttpd.h"
#include "sagui.h"
#include "sg_extra.h"
#include "sg_strmap.h"
#include "sg_httpauth.h"

struct sg_httpauth *sg__httpauth_new(struct sg_httpres *res) {
    struct sg_httpauth *auth = sg_alloc(sizeof(struct sg_httpauth));
    if (!auth)
        return NULL;
    auth->usr = MHD_basic_auth_get_username_password(res->con, &auth->pwd);
    auth->res = res;
    return auth;
}

void sg__httpauth_free(struct sg_httpauth *auth) {
    if (!auth)
        return;
    sg_free(auth->usr);
    sg_free(auth->pwd);
    sg_free(auth->realm);
    sg_free(auth);
}

bool sg__httpauth_dispatch(struct sg_httpauth *auth) {
    if (auth->res->ret) {
        auth->res->ret = MHD_YES;
        goto done;
    }
    if (auth->canceled) {
        auth->res->ret = auth->res->handle ? MHD_YES : MHD_NO;
        goto done;
    }
    if (auth->res->handle) {
        sg_strmap_iter(auth->res->headers, sg__strmap_iter, auth->res->handle);
        auth->res->ret = MHD_queue_basic_auth_fail_response(auth->res->con,
                                                            auth->realm ? auth->realm : _("Sagui realm"),
                                                            auth->res->handle);
    }
    return false;
done:
    return auth->res->ret == MHD_YES;
}

int sg_httpauth_set_realm(struct sg_httpauth *auth, const char *realm) {
    if (!auth || !realm)
        return EINVAL;
    if (auth->realm)
        return EALREADY;
    auth->realm = strdup(realm);
    if (!auth->realm)
        return ENOMEM;
    return 0;
}

const char *sg_httpauth_realm(struct sg_httpauth *auth) {
    if (auth)
        return auth->realm;
    errno = EINVAL;
    return NULL;
}

int sg_httpauth_deny(struct sg_httpauth *auth, const char *reason, const char *content_type) {
    if (!auth || !reason || !content_type)
        return EINVAL;
    if (auth->res->handle)
        return EALREADY;
    auth->res->handle = MHD_create_response_from_buffer(strlen(reason), (void *) reason, MHD_RESPMEM_MUST_COPY);
    if (!auth->res->handle)
        return ENOMEM;
    return sg_strmap_add(&auth->res->headers, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);
}

int sg_httpauth_cancel(struct sg_httpauth *auth) {
    if (!auth)
        return EINVAL;
    auth->canceled = true;
    return 0;
}

const char *sg_httpauth_usr(struct sg_httpauth *auth) {
    if (auth)
        return auth->usr;
    errno = EINVAL;
    return NULL;
}

const char *sg_httpauth_pwd(struct sg_httpauth *auth) {
    if (auth)
        return auth->pwd;
    errno = EINVAL;
    return NULL;
}
