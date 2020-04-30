/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2020 Silvio Clecio <silvioprog@gmail.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else /* _WIN32 */
#include <unistd.h>
#endif /* _WIN32 */
#include <sagui.h>

/* NOTE: Error checking has been omitted to make it clear. */

#define PAGE                                                                   \
  "<html>\n"                                                                   \
  "<head>\n"                                                                   \
  "<title>SSE example</title>\n"                                               \
  "</head><body><h2 id=\"counter\">Please wait ...</h2>\n"                     \
  "<script>\n"                                                                 \
  "const es = new EventSource('/');\n"                                         \
  "es.onmessage = function (ev) {\n"                                           \
  "  document.getElementById('counter').innerText = 'Counting: ' + ev.data;\n" \
  "};\n"                                                                       \
  "</script>\n"                                                                \
  "</body>\n"                                                                  \
  "</html>"

static unsigned int COUNT = 0;

#ifdef _WIN32
#define sleep Sleep
#endif /* _WIN32 */

static ssize_t sse_read_cb(__SG_UNUSED void *handle, uint64_t offset, char *buf,
                           size_t size) {
  char msg[20];
  if (offset == 0) {
    size = sprintf(msg, "retry: 1000\n");
  } else {
    size = sprintf(msg, "data: %d\n\n", ++COUNT);
    sleep(1);
  }
  memcpy(buf, msg, size);
  msg[size] = '\0';
  return size;
}

static void req_cb(__SG_UNUSED void *cls, struct sg_httpreq *req,
                   struct sg_httpres *res) {
  struct sg_strmap *pair;
  struct sg_strmap **req_headers = sg_httpreq_headers(req);
  struct sg_strmap **res_headers = sg_httpres_headers(res);
  if (sg_strmap_find(*req_headers, "Accept", &pair) == 0 &&
      strstr(sg_strmap_val(pair), "text/event-stream")) {
    sg_strmap_set(res_headers, "Access-Control-Allow-Origin", "*");
    sg_strmap_set(res_headers, "Content-Type", "text/event-stream");
    sg_httpres_sendstream(res, 0, sse_read_cb, NULL, NULL, 200);
    return;
  }
  if (strcmp(sg_httpreq_path(req), "/favicon.ico") == 0) {
    sg_httpres_send(res, "", "", 204);
    return;
  }
  sg_httpres_send(res, PAGE, "text/html; charset=utf-8", 200);
}

int main(int argc, const char *argv[]) {
  struct sg_httpsrv *srv;
  if (argc != 2) {
    printf("%s <PORT>\n", argv[0]);
    return EXIT_FAILURE;
  }
  srv = sg_httpsrv_new(req_cb, NULL);
  if (!sg_httpsrv_listen(srv, strtol(argv[1], NULL, 10), true)) {
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
