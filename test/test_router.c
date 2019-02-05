/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 *   –– cross-platform library which helps to develop web servers or frameworks.
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

#include "sg_assert.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "sg_router.h"
#include <sagui.h>

static void route_empty_cb(__SG_UNUSED void *cls, __SG_UNUSED struct sg_route *route) {
}

static void route_cb(void *cls, struct sg_route *route) {
    strcat(cls, sg_route_path(route));
    strcat(cls, sg_route_rawpattern(route));
    strcat(cls, sg_route_user_data(route));
}

static int router_dispatch_empty_cb(__SG_UNUSED void *cls, __SG_UNUSED const char *path,
                                    __SG_UNUSED struct sg_route *route) {
    return 0;
}

static int router_dispatch_123_cb(__SG_UNUSED void *cls, __SG_UNUSED const char *path,
                                  __SG_UNUSED struct sg_route *route) {
    return 123;
}

static int router_dispatch_concat_cb(void *cls, const char *path, struct sg_route *route) {
    strcat(cls, path);
    strcat(cls, sg_route_rawpattern(route));
    return 0;
}

static int router_match_empty_cb(__SG_UNUSED void *cls, __SG_UNUSED struct sg_route *route) {
    return 0;
}

static int router_match_123_cb(__SG_UNUSED void *cls, __SG_UNUSED struct sg_route *route) {
    return 123;
}

static void test_router_new(void) {
    struct sg_router *router;
    struct sg_route *routes = NULL;
    errno = 0;
    ASSERT(!sg_router_new(NULL));
    ASSERT(errno == EINVAL);

    ASSERT(sg_routes_add(&routes, "foo", route_empty_cb, "foo"));
    ASSERT(sg_routes_add(&routes, "bar", route_empty_cb, "bar"));
    router = sg_router_new(routes);
    ASSERT(router);
    ASSERT(router->routes == routes);
    sg_routes_cleanup(&routes);
    sg_router_free(router);
}

static void test_router_dispatch2(struct sg_router *router, struct sg_route **routes) {
    struct sg_router dummy_router;
    char str[100];
    ASSERT(sg_router_dispatch2(NULL, "foo", "bar", router_dispatch_empty_cb, "foobar",
                               router_match_empty_cb) == EINVAL);
    ASSERT(sg_router_dispatch2(router, NULL, "bar", router_dispatch_empty_cb, "foobar",
                               router_match_empty_cb) == EINVAL);
    dummy_router.routes = NULL;
    ASSERT(sg_router_dispatch2(&dummy_router, "foo", "bar", router_dispatch_empty_cb, "foobar",
                               router_match_empty_cb) == EINVAL);

    ASSERT(sg_router_dispatch2(router, "foo", "bar", router_dispatch_123_cb, "foobar",
                               router_match_empty_cb) == 123);
    memset(str, 0, sizeof(str));
    ASSERT(sg_router_dispatch2(router, "abc", "bar", router_dispatch_concat_cb, str, router_match_empty_cb) == ENOENT);
    ASSERT(strcmp(str, "abc^foo$abc^bar$") == 0);
    ASSERT(sg_router_dispatch2(router, "foo", "bar", router_dispatch_empty_cb, "foobar", router_match_123_cb) == 123);

    memset(str, 0, sizeof(str));
    ASSERT(sg_routes_add(routes, "/abc", route_cb, str));
    ASSERT(sg_router_dispatch2(router, "/abc", "foo", router_dispatch_empty_cb, "bar", router_match_empty_cb) == 0);
    ASSERT(strcmp(str, "/abc^/abc$foo") == 0);
}

static void test_router_dispatch(struct sg_router *router) {
    struct sg_router dummy_router;
    ASSERT(sg_router_dispatch(NULL, "foo", "bar") == EINVAL);
    ASSERT(sg_router_dispatch(router, NULL, "bar") == EINVAL);
    dummy_router.routes = NULL;
    ASSERT(sg_router_dispatch(&dummy_router, "foo", "bar") == EINVAL);

    ASSERT(sg_router_dispatch(router, "foo", NULL) == 0);
    ASSERT(sg_router_dispatch(router, "bar", NULL) == 0);
}

int main(void) {
    struct sg_router *router;
    struct sg_route *routes = NULL;

    test_router_new();

    sg_routes_add(&routes, "foo", route_empty_cb, "foo");
    sg_routes_add(&routes, "bar", route_empty_cb, "bar");
    router = sg_router_new(routes);

    test_router_dispatch2(router, &routes);
    test_router_dispatch(router);

    sg_routes_cleanup(&routes);
    sg_router_free(router);
    return EXIT_SUCCESS;
}
