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

/*
 * Tests:
 *
 * # return "Home"
 * curl http://localhost:<PORT>/home
 * # return "Download"
 * curl http://localhost:<PORT>/download
 * # return "file: <FILENAME>"
 * curl http://localhost:<PORT>/download/<FILENAME>
 * # return "About"
 * curl http://localhost:<PORT>/about
 * # return "404"
 * curl http://localhost:<PORT>/other
 */

#include <stdio.h>
#include <stdlib.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted to make it clear. */

struct holder {
    struct sg_httpreq *req;
    struct sg_httpres *res;
};

static int route_download_file_cb(void *cls, const char *name, const char *val) {
    sprintf(cls, "%s: %s", name, val);
    return 0;
}

static void route_home_cb(__SG_UNUSED void *cls, struct sg_route *route) {
    struct holder *h = sg_route_user_data(route);
    sg_httpres_send(h->res, "<html><head><title>Home</title></head><body>Home</body></html>", "text/html", 200);
}

static void route_download_cb(__SG_UNUSED void *cls, struct sg_route *route) {
    struct holder *h = sg_route_user_data(route);
    struct sg_str *page = sg_str_new();
    char file[256];
    memset(file, 0, sizeof(file));
    sg_route_get_vars(route, route_download_file_cb, file);
    if (strlen(file) == 0)
        strcpy(file, "Download");
    sg_str_printf(page, "<html><head><title>Download</title></head><body>%s</body></html>", file);
    sg_httpres_send(h->res, sg_str_content(page), "text/html", 200);
    sg_str_free(page);
}

static void route_about_cb(__SG_UNUSED void *cls, struct sg_route *route) {
    struct holder *h = sg_route_user_data(route);
    sg_httpres_send(h->res, "<html><head><title>About</title></head><body>About</body></html>", "text/html", 200);
}

static void req_cb(__SG_UNUSED void *cls, struct sg_httpreq *req, struct sg_httpres *res) {
    struct sg_router *router = cls;
    struct holder h = {req, res};
    if (sg_router_dispatch(router, sg_httpreq_path(req), &h) != 0)
        sg_httpres_send(res, "<html><head><title>Not found</title></head><body>404</body></html>", "text/html", 404);
}

int main(void) {
    struct sg_route *routes = NULL;
    struct sg_router *router;
    struct sg_httpsrv *srv;
    sg_routes_add(&routes, "/home", route_home_cb, NULL);
    sg_routes_add(&routes, "/download", route_download_cb, NULL);
    sg_routes_add(&routes, "/download/(?P<file>[a-z]+)", route_download_cb, NULL);
    sg_routes_add(&routes, "/about", route_about_cb, NULL);
    router = sg_router_new(routes);
    srv = sg_httpsrv_new(req_cb, router);
    if (!sg_httpsrv_listen(srv, 0 /* 0 = port chosen randomly */, false)) {
        sg_httpsrv_free(srv);
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Server running at http://localhost:%d\n", sg_httpsrv_port(srv));
    fflush(stdout);
    getchar();
    sg_httpsrv_free(srv);
    sg_routes_cleanup(&routes);
    sg_router_free(router);
    return EXIT_SUCCESS;
}
