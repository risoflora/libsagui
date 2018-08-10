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
#include <limits.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted for clarity. */

#ifdef _WIN32
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

#define PAGE_FORM                                                        \
    "<html>"                                                             \
    "<body>"                                                             \
    "<form action=\"\" method=\"post\" enctype=\"multipart/form-data\">" \
    "<fieldset>"                                                         \
    "<legend>Choose the files:</legend>"                                 \
    "File 1: <input type=\"file\" name=\"file1\"/><br>"                  \
    "File 2: <input type=\"file\" name=\"file2\"/><br>"                  \
    "<input type=\"submit\"/>"                                           \
    "</fieldset>"                                                        \
    "</form>"                                                            \
    "</body>"                                                            \
    "</html>"

#define PAGE_DONE                          \
    "<html>"                               \
    "<head>"                               \
    "<title>Uploads</title>"               \
    "</head>"                              \
    "<body>"                               \
    "<strong>Uploaded files:</strong><br>" \
    "%s"                                   \
    "</body>"                              \
    "</html>"

#define CONTENT_TYPE "text/html; charset=utf-8"

static void process_uploads(struct sg_httpreq *req, struct sg_httpres *res) {
    struct sg_httpupld *upld;
    struct sg_str *body;
    const char *name;
    char *str;
    char errmsg[256];
    int errnum;
    body = sg_str_new();
    sg_str_printf(body, "<ol>");
    upld = sg_httpreq_uploads(req);
    while (upld) {
        name = sg_httpupld_name(upld);
        if ((errnum = sg_httpupld_save(upld, true)) == 0)
            sg_str_printf(body, "<li><a href=\"?file=%s\">%s</a></li>", name, name);
        else {
            sg_strerror(-errnum, errmsg, sizeof(errmsg));
            sg_str_printf(body, "<li><font color='red'>%s - failed - %s</font></li>", name, errmsg);
        }
        sg_httpuplds_next(&upld);
    }
    sg_str_printf(body, "</ol>");
    str = strdup(sg_str_content(body));
    sg_str_clear(body);
    sg_str_printf(body, PAGE_DONE, str);
    free(str);
    sg_httpres_send(res, sg_str_content(body), CONTENT_TYPE, 200);
    sg_str_free(body);
}

static void req_cb(__SG_UNUSED void *cls, struct sg_httpreq *req, struct sg_httpres *res) {
    struct sg_strmap **qs;
    const char *file;
    char path[PATH_MAX];
    if (sg_httpreq_is_uploading(req))
        process_uploads(req, res);
    else {
        if ((qs = sg_httpreq_params(req)) && (file = sg_strmap_get(*qs, "file"))) {
            sprintf(path, "%s%c%s", sg_tmpdir(), PATH_SEP, file);
            sg_httpres_sendfile(res, 4096, 0, path, false, 200);
        } else
            sg_httpres_send(res, PAGE_FORM, CONTENT_TYPE, 200);
    }
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
