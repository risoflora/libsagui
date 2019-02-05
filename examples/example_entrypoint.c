/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 *   –– cross-platform library which helps to develop web servers or frameworks.
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
#include <string.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted to make it clear. */

static void r1_route_cb(void *cls, struct sg_route *route) {
    fprintf(stdout, "%s: %s\n", sg_route_path(route), (const char *) cls);
    fflush(stdout);
}

static void r2_route_cb(void *cls, struct sg_route *route) {
    fprintf(stdout, "%s: %s\n", sg_route_path(route), (const char *) cls);
    fflush(stdout);
}

int main(void) {
    struct sg_router *r1, *r2;
    struct sg_route *rts1 = NULL, *rts2 = NULL;
    struct sg_entrypoints *entrypoints;
    struct sg_entrypoint *entrypoint;
    sg_routes_add(&rts1, "/foo", r1_route_cb, "r1-foo-data");
    sg_routes_add(&rts1, "/bar", r1_route_cb, "r1-bar-data");
    r1 = sg_router_new(rts1);
    sg_routes_add(&rts2, "/foo", r2_route_cb, "r2-foo-data");
    sg_routes_add(&rts2, "/bar", r2_route_cb, "r2-bar-data");
    r2 = sg_router_new(rts2);

    entrypoints = sg_entrypoints_new();
    sg_entrypoints_add(entrypoints, "/r1", r1);
    sg_entrypoints_add(entrypoints, "/r2", r2);

    sg_entrypoints_find(entrypoints, &entrypoint, "/r1/foo");
    sg_router_dispatch(sg_entrypoint_user_data(entrypoint), "/foo", NULL);
    sg_entrypoints_find(entrypoints, &entrypoint, "/r1/bar");
    sg_router_dispatch(sg_entrypoint_user_data(entrypoint), "/bar", NULL);
    sg_entrypoints_find(entrypoints, &entrypoint, "/r2/foo");
    sg_router_dispatch(sg_entrypoint_user_data(entrypoint), "/foo", NULL);
    sg_entrypoints_find(entrypoints, &entrypoint, "/r2/bar");
    sg_router_dispatch(sg_entrypoint_user_data(entrypoint), "/bar", NULL);

    sg_routes_cleanup(&rts1);
    sg_routes_cleanup(&rts2);
    sg_router_free(r1);
    sg_router_free(r2);
    sg_entrypoints_free(entrypoints);
    return EXIT_SUCCESS;
}
