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

#include "sg_assert.h"

#include <stdlib.h>
#include <string.h>
#include "sg_httpreq.h"

static void test__httpreq_new(void) {
    struct MHD_Connection *con = sg_alloc(64);
    struct sg_httpreq *req = sg__httpreq_new(con, "abc", "def", "ghi");
    ASSERT(req);
    ASSERT(strcmp(req->version, "abc") == 0);
    ASSERT(strcmp(req->method, "def") == 0);
    ASSERT(strcmp(req->path, "ghi") == 0);
    sg_free(con);
    sg__httpreq_free(req);
}

static void test__httpreq_free(void) {
    sg__httpreq_free(NULL);
}

static void test_httpreq_headers(struct sg_httpreq *req) {
    struct sg_strmap **headers;
    errno = 0;
    ASSERT(!sg_httpreq_headers(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    req->headers = NULL;
    ASSERT(sg_httpreq_headers(req));
    ASSERT(errno == 0);
    ASSERT((headers = sg_httpreq_headers(req)));
    ASSERT(sg_strmap_count(*headers) == 0);
    sg_strmap_add(&req->headers, "foo", "bar");
    sg_strmap_add(&req->headers, "abc", "123");
    ASSERT(sg_strmap_count(*headers) == 2);
    ASSERT(strcmp(sg_strmap_get(*headers, "foo"), "bar") == 0);
    ASSERT(strcmp(sg_strmap_get(*headers, "abc"), "123") == 0);
}

static void test_httpreq_cookies(struct sg_httpreq *req) {
    struct sg_strmap **cookies;
    errno = 0;
    ASSERT(!sg_httpreq_cookies(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    req->cookies = NULL;
    ASSERT(sg_httpreq_cookies(req));
    ASSERT(errno == 0);
    ASSERT((cookies = sg_httpreq_cookies(req)));
    ASSERT(sg_strmap_count(*cookies) == 0);
    sg_strmap_add(&req->cookies, "foo", "bar");
    sg_strmap_add(&req->cookies, "abc", "123");
    ASSERT(sg_strmap_count(*cookies) == 2);
    ASSERT(strcmp(sg_strmap_get(*cookies, "foo"), "bar") == 0);
    ASSERT(strcmp(sg_strmap_get(*cookies, "abc"), "123") == 0);
}

static void test_httpreq_params(struct sg_httpreq *req) {
    struct sg_strmap **params;
    errno = 0;
    ASSERT(!sg_httpreq_params(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    req->params = NULL;
    ASSERT(sg_httpreq_params(req));
    ASSERT(errno == 0);
    ASSERT((params = sg_httpreq_params(req)));
    ASSERT(sg_strmap_count(*params) == 0);
    sg_strmap_add(&req->params, "foo", "bar");
    sg_strmap_add(&req->params, "abc", "123");
    ASSERT(sg_strmap_count(*params) == 2);
    ASSERT(strcmp(sg_strmap_get(*params, "foo"), "bar") == 0);
    ASSERT(strcmp(sg_strmap_get(*params, "abc"), "123") == 0);
}

static void test_httpreq_fields(struct sg_httpreq *req) {
    struct sg_strmap **fields;
    errno = 0;
    ASSERT(!sg_httpreq_fields(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    req->fields = NULL;
    ASSERT(sg_httpreq_fields(req));
    ASSERT(errno == 0);
    ASSERT((fields = sg_httpreq_fields(req)));
    ASSERT(sg_strmap_count(*fields) == 0);
    sg_strmap_add(&req->fields, "foo", "bar");
    sg_strmap_add(&req->fields, "abc", "123");
    ASSERT(sg_strmap_count(*fields) == 2);
    ASSERT(strcmp(sg_strmap_get(*fields, "foo"), "bar") == 0);
    ASSERT(strcmp(sg_strmap_get(*fields, "abc"), "123") == 0);
}

static void test_httpreq_version(struct sg_httpreq *req) {
    errno = 0;
    ASSERT(!sg_httpreq_version(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    req->version = NULL;
    ASSERT(!sg_httpreq_version(req));
    ASSERT(errno == 0);
    req->version = "1.0";
    ASSERT(strcmp(sg_httpreq_version(req), "1.0") == 0);
    req->version = "1.1";
    ASSERT(strcmp(sg_httpreq_version(req), "1.1") == 0);
}

static void test_httpreq_method(struct sg_httpreq *req) {
    errno = 0;
    ASSERT(!sg_httpreq_method(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    req->method = NULL;
    ASSERT(!sg_httpreq_method(req));
    ASSERT(errno == 0);
    req->method = "GET";
    ASSERT(strcmp(sg_httpreq_method(req), "GET") == 0);
    req->method = "POST";
    ASSERT(strcmp(sg_httpreq_method(req), "POST") == 0);
}

static void test_httpreq_path(struct sg_httpreq *req) {
    errno = 0;
    ASSERT(!sg_httpreq_path(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    req->path = NULL;
    ASSERT(!sg_httpreq_path(req));
    ASSERT(errno == 0);
    req->path = "/foo";
    ASSERT(strcmp(sg_httpreq_path(req), "/foo") == 0);
    req->path = "/bar";
    ASSERT(strcmp(sg_httpreq_path(req), "/bar") == 0);
}

static void test_httpreq_payload(struct sg_httpreq *req) {
    struct sg_str *old_payload;
    errno = 0;
    ASSERT(!sg_httpreq_payload(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    old_payload = req->payload;
    req->payload = NULL;
    ASSERT(!sg_httpreq_payload(req));
    req->payload = old_payload;
    ASSERT(errno == 0);
    ASSERT(sg_str_length(sg_httpreq_payload(req)) == 0);
    sg_str_printf(sg_httpreq_payload(req), "%s", "abc");
    ASSERT(strcmp(sg_str_content(sg_httpreq_payload(req)), "abc") == 0);
    sg_str_printf(sg_httpreq_payload(req), "%d", 123);
    ASSERT(strcmp(sg_str_content(sg_httpreq_payload(req)), "abc123") == 0);
}

static void test_httpreq_is_uploading(struct sg_httpreq *req) {
    errno = 0;
    ASSERT(!sg_httpreq_is_uploading(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    req->is_uploading = false;
    ASSERT(!sg_httpreq_is_uploading(req));
    ASSERT(errno == 0);
    req->is_uploading = true;
    ASSERT(sg_httpreq_is_uploading(req));
    ASSERT(errno == 0);
}

static void test_httpreq_uploads(struct sg_httpreq *req) {
    struct sg_httpupld *tmp;
    errno = 0;
    ASSERT(!sg_httpreq_uploads(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    req->uplds = NULL;
    ASSERT(!sg_httpreq_uploads(req));
    ASSERT(errno == 0);
    req->curr_upld = sg_alloc(sizeof(struct sg_httpupld));
    LL_APPEND(req->uplds, req->curr_upld);
    req->curr_upld->name = "foo";
    ASSERT(strcmp(sg_httpupld_name(req->curr_upld), "foo") == 0);
    req->curr_upld = sg_alloc(sizeof(struct sg_httpupld));
    LL_APPEND(req->uplds, req->curr_upld);
    req->curr_upld->name = "bar";
    ASSERT(strcmp(sg_httpupld_name(req->curr_upld), "bar") == 0);
    LL_FOREACH_SAFE(req->uplds, req->curr_upld, tmp) {
        LL_DELETE(req->uplds, req->curr_upld);
        sg_free(req->curr_upld);
    }
}

#ifdef SG_HTTPS_SUPPORT

static void test_httpreq_tls_session(void) {
    errno = 0;
    ASSERT(!sg_httpreq_tls_session(NULL));
    ASSERT(errno == EINVAL);
    /* more tests in `test_httpsrv_tls_curl.c`. */
}

#endif

static void test_httpreq_set_user_data(struct sg_httpreq *req) {
    const char *dummy = "foo";
    ASSERT(sg_httpreq_set_user_data(NULL, (void *) dummy) == EINVAL);

    ASSERT(sg_httpreq_set_user_data(req, (void *) dummy) == 0);
    ASSERT(strcmp(sg_httpreq_user_data(req), "foo") == 0);
    dummy = "bar";
    ASSERT(sg_httpreq_set_user_data(req, (void *) dummy) == 0);
    ASSERT(strcmp(sg_httpreq_user_data(req), "bar") == 0);

}

static void test_httpreq_user_data(struct sg_httpreq *req) {
    errno = 0;
    ASSERT(!sg_httpreq_user_data(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    sg_httpreq_set_user_data(req, NULL);
    ASSERT(!sg_httpreq_user_data(req));
    ASSERT(errno == 0);
    sg_httpreq_set_user_data(req, "foo");
    ASSERT(strcmp(sg_httpreq_user_data(req), "foo") == 0);
    sg_httpreq_set_user_data(req, "bar");
    ASSERT(strcmp(sg_httpreq_user_data(req), "bar") == 0);
}

int main(void) {
    struct sg_httpreq *req = sg__httpreq_new(NULL, NULL, NULL, NULL);
    test__httpreq_new();
    test__httpreq_free();
    test_httpreq_headers(req);
    test_httpreq_cookies(req);
    test_httpreq_params(req);
    test_httpreq_fields(req);
    test_httpreq_version(req);
    test_httpreq_method(req);
    test_httpreq_path(req);
    test_httpreq_payload(req);
    test_httpreq_is_uploading(req);
    test_httpreq_uploads(req);
#ifdef SG_HTTPS_SUPPORT
    test_httpreq_tls_session();
#endif
    test_httpreq_set_user_data(req);
    test_httpreq_user_data(req);
    sg__httpreq_free(req);
    return EXIT_SUCCESS;
}
