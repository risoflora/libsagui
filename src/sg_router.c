/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2019 Silvio Clecio <silvioprog@gmail.com>
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
#include "sg_macros.h"
#include "utlist.h"
#include "sg_routes.h"
#include "sg_router.h"
#include "sagui.h"

struct sg_router *sg_router_new(struct sg_route *routes) {
    struct sg_router *router;
    if (!routes) {
        errno = EINVAL;
        return NULL;
    }
    router = sg_alloc(sizeof(struct sg_router));
    if (!router)
        return NULL;
    router->routes = routes;
    return router;
}

void sg_router_free(struct sg_router *router) {
    sg_free(router);
}

int sg_router_dispatch2(struct sg_router *router, const char *path, void *user_data,
                        sg_router_dispatch_cb dispatch_cb, void *cls, sg_router_match_cb match_cb) {
    struct sg_route *route;
    int ret;
    if (!router || !path || !router->routes)
        return EINVAL;
    LL_FOREACH(router->routes, route) {
        if (dispatch_cb) {
            ret = dispatch_cb(cls, path, route);
            if (ret != 0)
                return ret;
        }
#ifdef PCRE2_JIT_SUPPORT
#define SG__PCRE2_MATCH pcre2_jit_match
#else
#define SG__PCRE2_MATCH pcre2_match
#endif
        route->rc = SG__PCRE2_MATCH(route->re, (PCRE2_SPTR) path, strlen(path), 0, 0, route->match, NULL);
#undef SG__PCRE2_MATCH
        if (route->rc >= 0) {
            route->path = path;
            route->user_data = user_data;
            if (match_cb) {
                ret = match_cb(cls, route);
                if (ret != 0)
                    return ret;
            }
            route->cb(route->cls, route);
            return 0;
        }
    }
    return ENOENT;
}

int sg_router_dispatch(struct sg_router *router, const char *path, void *user_data) {
    return sg_router_dispatch2(router, path, user_data, NULL, NULL, NULL);
}
