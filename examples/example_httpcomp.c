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
#include <stdint.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted to make it clear. */

#define PAGE "<html><head><title>Hello world</title></head><body>Hello world</body></html>"
#define CONTENT_TYPE "text/html; charset=utf-8"

static void req_cb(__SG_UNUSED void *cls, struct sg_httpreq *req, struct sg_httpres *res) {
    struct sg_strmap **headers;
    const char *header;
    headers = sg_httpreq_headers(req);
    if (headers) {
        header = sg_strmap_get(*headers, "Accept-Encoding");
        if (header && strstr(header, "deflate")) {
            sg_httpres_zsendbinary(res, PAGE, strlen(PAGE), CONTENT_TYPE, 200);
            return;
        }
    }
    sg_httpres_sendbinary(res, PAGE, strlen(PAGE), CONTENT_TYPE, 200);
}

int main(int argc, const char *argv[]) {
    struct sg_httpsrv *srv;
    uint16_t port;
    if (argc != 2) {
        printf("%s <PORT>\n", argv[0]);
        return EXIT_FAILURE;
    }
    port = strtol(argv[1], NULL, 10);
    srv = sg_httpsrv_new(req_cb, NULL);
    if (!sg_httpsrv_listen(srv, port, false)) {
        sg_httpsrv_free(srv);
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Server running at http://localhost:%d\n", sg_httpsrv_port(srv));
    fflush(stdout);
    getchar();
    sg_httpsrv_free(srv);
    return EXIT_SUCCESS;
}
