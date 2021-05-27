/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2021 Silvio Clecio <silvioprog@gmail.com>
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

/*
 * Sending HTML form fields using cURL:
 *
 * curl --request POST --data 'username=silvioprog&password=i-love-sagui' -w "\n" http://localhost:<PORT>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted to make it clear. */

static void req_cb(__SG_UNUSED void *cls, struct sg_httpreq *req,
                   struct sg_httpres *res) {
  struct sg_strmap **fields = sg_httpreq_fields(req);
  struct sg_str *result = sg_str_new();
  const char *username = sg_strmap_get(*fields, "username");
  const char *password = sg_strmap_get(*fields, "password");
  sg_str_printf(result, "Username: %s; Password: %s", username, password);
  sg_httpres_send(res, sg_str_content(result), "text/plain", 200);
  sg_str_free(result);
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
  fprintf(stdout, "Server running at http://localhost:%d\n",
          sg_httpsrv_port(srv));
  fflush(stdout);
  getchar();
  sg_httpsrv_free(srv);
  return EXIT_SUCCESS;
}
