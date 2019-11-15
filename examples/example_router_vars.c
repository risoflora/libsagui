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

#include <stdlib.h>
#include <stdio.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted to make it clear. */

static int vars_iter_cb(__SG_UNUSED void *cls, const char *name, const char *val) {
    fprintf(stdout, " %s: %s\n", name, val);
    return 0;
}

static void route_cb(void *cls, struct sg_route *route) {
    fprintf(stdout, "%s: %s\n", sg_route_path(route), (const char *) cls);
    sg_route_vars_iter(route, vars_iter_cb, NULL);
}

int main(void) {
    struct sg_router *router;
    struct sg_route *routes = NULL;
    sg_routes_add(&routes, "/foo/bar", route_cb, "foo-bar-data");
    sg_routes_add(&routes, "/bar", route_cb, "bar-data");
    sg_routes_add(&routes, "/customer/(?P<name>[a-zA-Z]+)", route_cb, "customer-data");
    sg_routes_add(&routes, "/product/(?P<id>[0-9]+)", route_cb, "product-data");
    sg_routes_add(&routes, "/employee/(?P<id>[0-9]+)/[a|i]", route_cb, "employee-data");
    router = sg_router_new(routes);
    sg_router_dispatch(router, "/foo/bar", NULL);
    fprintf(stdout, "---\n");
    sg_router_dispatch(router, "/customer/Torvalds", NULL);
    fprintf(stdout, "---\n");
    sg_router_dispatch(router, "/product/123", NULL);
    fprintf(stdout, "---\n");
    sg_router_dispatch(router, "/employee/123/i", NULL);
    sg_routes_cleanup(&routes);
    sg_router_free(router);
    fflush(stdout);
    return EXIT_SUCCESS;
}
