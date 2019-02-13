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

/*
 * Echoing payload using cURL:
 *
 * curl --header "Content-Type: application/json" --request POST --data '{"abc":123}' -w "\n" http://localhost:<PORT>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted to make it clear. */

static void req_cb(__SG_UNUSED void *cls, struct sg_httpreq *req, struct sg_httpres *res) {
    struct sg_str *payload = sg_httpreq_payload(req);
    sg_httpres_send(res, sg_str_content(payload), "text/plain", 200);
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
