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
#include <stdio.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted to make it clear. */

static int get_segments_cb(__SG_UNUSED void *cls, const char *segment) {
    fprintf(stdout, " %s\n", segment);
    return 0;
}

static void route_cb(void *cls, struct sg_route *route) {
    fprintf(stdout, "%s: %s\n", sg_route_path(route), (const char *) cls);
    sg_route_get_segments(route, get_segments_cb, NULL);
}

int main(void) {
    struct sg_router *router;
    struct sg_route *routes = NULL;
    sg_routes_add(&routes, "/foo/[0-9]+", route_cb, "foo-data");
    sg_routes_add(&routes, "/bar/([a-zA-Z]+)", route_cb, "bar-data");
    router = sg_router_new(routes);
    sg_router_dispatch(router, "/foo/123", NULL);
    fprintf(stdout, "---\n");
    sg_router_dispatch(router, "/bar/abc", NULL);
    sg_routes_cleanup(&routes);
    sg_router_free(router);
    fflush(stdout);
    return EXIT_SUCCESS;
}
