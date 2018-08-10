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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted for clarity. */

#define CONTENT_TYPE "text/html; charset=utf-8"
#define BEGIN_PAGE "<html><head><title>Cookies</title></head><body>"
#define END_PAGE "</body></html>"
#define INITIAL_PAGE BEGIN_PAGE "Use F5 to refresh this page ..." END_PAGE
#define COUNT_PAGE BEGIN_PAGE "Refresh number: %d" END_PAGE
#define COOKIE_NAME "refresh_count"

static int strtoint(const char *str) {
    if (!str)
        return 0;
    return (int) strtol(str, NULL, 10);
}

static void req_cb(__SG_UNUSED void *cls, __SG_UNUSED struct sg_httpreq *req, struct sg_httpres *res) {
    struct sg_strmap **cookies = sg_httpreq_cookies(req);
    char str[100];
    int count;
    if (strcmp(sg_httpreq_path(req), "/favicon.ico") == 0) {
        sg_httpres_send(res, "", "", 204);
        return;
    }
    count = cookies ? strtoint(sg_strmap_get(*cookies, COOKIE_NAME)) : 0;
    if (count == 0) {
        snprintf(str, sizeof(str), INITIAL_PAGE);
        count = 1;
    } else {
        snprintf(str, sizeof(str), COUNT_PAGE, count);
        count++;
    }
    sg_httpres_send(res, str, CONTENT_TYPE, 200);
    snprintf(str, sizeof(str), "%d", count);
    sg_httpres_set_cookie(res, COOKIE_NAME, str);
}

int main(void) {
    struct sg_httpsrv *srv = sg_httpsrv_new(req_cb, NULL);
    if (!sg_httpsrv_listen(srv, 0 /* 0 = port chosen randomly */, false)) {
        sg_httpsrv_free(srv);
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Server running at http://localhost:%d\n", sg_httpsrv_port(srv));
    fflush(stdout);
    getchar();
    sg_httpsrv_free(srv);
    return EXIT_SUCCESS;
}
