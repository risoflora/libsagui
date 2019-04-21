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

#define SG_EXTERN

#include "sg_assert.h"

#include <string.h>
#include "sg_httpres.c"

#ifndef TEST_HTTPRES_BASE_PATH
#ifdef __ANDROID__
#define TEST_HTTPRES_BASE_PATH SG_ANDROID_TESTS_DEST_DIR "/"
#else
#ifdef _WIN32
#define TEST_HTTPRES_BASE_PATH ""
#else
#define TEST_HTTPRES_BASE_PATH "/tmp/"
#endif
#endif
#endif

static ssize_t dummy_read_cb(void *handle, uint64_t offset, char *buf, size_t size) {
    (void) handle;
    (void) offset;
    (void) buf;
    (void) size;
    return 0;
}

static void dummy_free_cb(void *handle) {
    *((int *) handle) = 0;
}

static void test__httpres_new(void) {
    struct sg_httpres *res = sg__httpres_new(NULL);
    ASSERT(res);
    ASSERT(res->status == 500);
    sg__httpres_free(res);
}

static void test__httpres_free(void) {
    sg__httpres_free(NULL);
}

static void test__httpres_dispatch(struct sg_httpres *res) {
    ASSERT(sg__httpres_dispatch(res) == 0);
}

static void test_httpres_headers(struct sg_httpres *res) {
    struct sg_strmap **headers;
    errno = 0;
    ASSERT(!sg_httpres_headers(NULL));
    ASSERT(errno == EINVAL);

    errno = 0;
    res->headers = NULL;
    ASSERT(sg_httpres_headers(res));
    ASSERT(errno == 0);
    headers = sg_httpres_headers(res);
    ASSERT(headers);
    ASSERT(sg_strmap_count(*headers) == 0);
    sg_strmap_add(&res->headers, "foo", "bar");
    sg_strmap_add(&res->headers, "abc", "123");
    ASSERT(sg_strmap_count(*headers) == 2);
    ASSERT(strcmp(sg_strmap_get(*headers, "foo"), "bar") == 0);
    ASSERT(strcmp(sg_strmap_get(*headers, "abc"), "123") == 0);
}

static void test_httpres_set_cookie(struct sg_httpres *res) {
    struct sg_strmap **fields;
    ASSERT(sg_httpres_set_cookie(NULL, "foo", "bar") == EINVAL);
    ASSERT(sg_httpres_set_cookie(res, NULL, "bar") == EINVAL);
    ASSERT(sg_httpres_set_cookie(res, "foo", NULL) == EINVAL);

    fields = sg_httpres_headers(res);
    sg_strmap_cleanup(fields);
    ASSERT(sg_httpres_set_cookie(res, "foo", "bar") == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_SET_COOKIE), "foo=bar") == 0);
}

static void test_httpres_send(struct sg_httpres *res) {
    char *str = "foo";

    ASSERT(sg_httpres_send(NULL, str, "text/plain", 200) == EINVAL);
    str = NULL;
    ASSERT(sg_httpres_send(res, str, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_send(res, str, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_send(res, str, "text/plain", 99) == EINVAL);
    ASSERT(sg_httpres_send(res, str, "text/plain", 600) == EINVAL);

    res->status = 0;
    ASSERT(sg_httpres_send(res, "", "text/plain", 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 0;
    ASSERT(sg_httpres_send(res, "", "", 204) == 0); /* No content. */
    ASSERT(res->status == 204);
    MHD_destroy_response(res->handle);
    res->handle = NULL;

    res->status = 0;
    str = "foo";
    ASSERT(sg_httpres_send(res, str, "text/plain", 201) == 0);
    ASSERT(res->status == 201);
    ASSERT(sg_httpres_send(res, str, "text/plain", 200) == EALREADY);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_TYPE), "text/plain") == 0);
    ASSERT(res->status == 201);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
}

static void test_httpres_sendbinary(struct sg_httpres *res) {
    char *str = "foo";
    const size_t len = strlen(str);

    ASSERT(sg_httpres_sendbinary(NULL, str, len, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_sendbinary(res, NULL, len, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_sendbinary(res, str, (size_t) -1, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_sendbinary(res, str, len, "text/plain", 99) == EINVAL);
    ASSERT(sg_httpres_sendbinary(res, str, len, "text/plain", 600) == EINVAL);

    res->status = 0;
    ASSERT(sg_httpres_sendbinary(res, str, len, NULL, 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 0;
    ASSERT(sg_httpres_sendbinary(res, "", len, "text/plain", 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 0;
    ASSERT(sg_httpres_sendbinary(res, "foo", 0, "text/plain", 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 0;
    ASSERT(sg_httpres_sendbinary(res, str, len, "", 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 0;
    ASSERT(sg_httpres_sendbinary(res, "", 0, "", 204) == 0); /* No content. */
    ASSERT(res->status == 204);
    MHD_destroy_response(res->handle);
    res->handle = NULL;

    res->status = 0;
    ASSERT(sg_httpres_sendbinary(res, str, len, "text/plain", 201) == 0);
    ASSERT(res->status == 201);
    ASSERT(sg_httpres_sendbinary(res, str, len, "text/plain", 200) == EALREADY);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_TYPE), "text/plain") == 0);
    ASSERT(res->status == 201);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
}

static void test_httpres_download(struct sg_httpres *res) {
#define FILENAME "foo.txt"
#define PATH TEST_HTTPRES_BASE_PATH FILENAME
    const size_t len = 3;
    char str[4];
    FILE *file;
    char *dir;

    ASSERT(sg_httpres_download(NULL, PATH) == EINVAL);
    ASSERT(sg_httpres_download(res, NULL) == EINVAL);

#ifdef _WIN32
    ASSERT(sg_httpres_download(res, "") == EACCES);
#else
    ASSERT(sg_httpres_download(res, "") == ENOENT);
#endif
    dir = sg_tmpdir();
#ifdef _WIN32
    ASSERT(sg_httpres_download(res, dir) == EACCES);
#else
    ASSERT(sg_httpres_download(res, dir) == EISDIR);
#endif
    sg_free(dir);

    strcpy(str, "foo");
    unlink(PATH);
    file = fopen(PATH, "w");
    ASSERT(file);
    ASSERT(fwrite(str, 1, len, file) == len);
    ASSERT(fclose(file) == 0);
    ASSERT(sg_httpres_download(res, PATH) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "attachment; filename=\""
                   FILENAME
                   "\"") == 0);
    ASSERT(sg_httpres_download(res, PATH) == EALREADY);
    sg_free(res->handle);
    res->handle = NULL;
#undef PATH
#undef FILENAME
    sg_free(res->handle);
    res->handle = NULL;
}

static void test_httpres_render(struct sg_httpres *res) {
#define FILENAME "foo.txt"
#define PATH TEST_HTTPRES_BASE_PATH FILENAME
    const size_t len = 3;
    char str[4];
    FILE *file;
    char *dir;

    ASSERT(sg_httpres_render(NULL, PATH) == EINVAL);
    ASSERT(sg_httpres_render(res, NULL) == EINVAL);

#ifdef _WIN32
    ASSERT(sg_httpres_render(res, "") == EACCES);
#else
    ASSERT(sg_httpres_render(res, "") == ENOENT);
#endif
    dir = sg_tmpdir();
#ifdef _WIN32
    ASSERT(sg_httpres_render(res, dir) == EACCES);
#else
    ASSERT(sg_httpres_render(res, dir) == EISDIR);
#endif
    sg_free(dir);

    strcpy(str, "foo");
    unlink(PATH);
    file = fopen(PATH, "w");
    ASSERT(file);
    ASSERT(fwrite(str, 1, len, file) == len);
    ASSERT(fclose(file) == 0);
    ASSERT(sg_httpres_render(res, PATH) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "inline; filename=\""
                   FILENAME
                   "\"") == 0);
    ASSERT(sg_httpres_render(res, PATH) == EALREADY);
    sg_free(res->handle);
    res->handle = NULL;
#undef PATH
#undef FILENAME
    sg_free(res->handle);
    res->handle = NULL;
}

static void test_httpres_sendfile2(struct sg_httpres *res) {
#define FILENAME "foo.txt"
#define PATH TEST_HTTPRES_BASE_PATH FILENAME
    const size_t len = 3;
    char str[4];
    FILE *file;
    char *dir;
    size_t size = sizeof(int);
    uint64_t max_size = 10, offset = 0;

    ASSERT(sg_httpres_sendfile2(NULL, size, max_size, offset, PATH, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile2(res, (uint64_t) -1, max_size, offset, PATH, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile2(res, size, (uint64_t) -1, offset, PATH, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile2(res, size, max_size, (uint64_t) -1, PATH, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile2(res, size, max_size, offset, NULL, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile2(res, size, max_size, offset, PATH, NULL, 99) == EINVAL);
    ASSERT(sg_httpres_sendfile2(res, size, max_size, offset, PATH, NULL, 600) == EINVAL);

#ifdef _WIN32
    ASSERT(sg_httpres_sendfile2(res, size, max_size, offset, "", NULL, 200) == EACCES);
#else
    ASSERT(sg_httpres_sendfile2(res, size, max_size, offset, "", NULL, 200) == ENOENT);
#endif
    dir = sg_tmpdir();
#ifdef _WIN32
    ASSERT(sg_httpres_sendfile2(res, size, max_size, offset, dir, NULL, 200) == EACCES);
#else
    ASSERT(sg_httpres_sendfile2(res, size, max_size, offset, dir, NULL, 200) == EISDIR);
#endif
    sg_free(dir);

    strcpy(str, "foo");
    unlink(PATH);
    file = fopen(PATH, "w");
    ASSERT(file);
    ASSERT(fwrite(str, 1, len, file) == len);
    ASSERT(fclose(file) == 0);
    ASSERT(sg_httpres_sendfile2(res, size, 1, offset, PATH, NULL, 200) == EFBIG);

    ASSERT(sg_httpres_sendfile2(res, size, len, offset, PATH, "attachment", 200) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "attachment; filename=\""
                   FILENAME
                   "\"") == 0);

    ASSERT(sg_httpres_sendfile2(res, size, len, offset, PATH, "attachment", 200) == EALREADY);

    sg_free(res->handle);
    res->handle = NULL;
    ASSERT(sg_httpres_sendfile2(res, size, len, offset, PATH, "inline", 201) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "inline; filename=\""
                   FILENAME
                   "\"") == 0);
    ASSERT(res->status == 201);

    sg_free(res->handle);
    res->handle = NULL;
    sg_strmap_cleanup(sg_httpres_headers(res));
    ASSERT(sg_httpres_sendfile2(res, size, len, offset, PATH, NULL, 200) == 0);
    ASSERT(!sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION));
    ASSERT(res->status == 200);

    sg_free(res->handle);
    res->handle = NULL;
    ASSERT(sg_httpres_sendfile2(res, size, len, offset, PATH, "abc123", 201) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "abc123; filename=\""
                   FILENAME
                   "\"") == 0);
    ASSERT(res->status == 201);
#undef PATH
#undef FILENAME
    sg_free(res->handle);
    res->handle = NULL;
}

static void test_httpres_sendfile(struct sg_httpres *res) {
#define FILENAME "foo.txt"
#define PATH TEST_HTTPRES_BASE_PATH FILENAME
    const size_t len = 3;
    char str[4];
    FILE *file;
    char *dir;
    size_t size = sizeof(int);
    uint64_t max_size = 10, offset = 0;

    ASSERT(sg_httpres_sendfile(NULL, size, max_size, offset, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, (uint64_t) -1, max_size, offset, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, size, (uint64_t) -1, offset, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, size, max_size, (uint64_t) -1, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, size, max_size, offset, NULL, false, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, size, max_size, offset, PATH, false, 99) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, size, max_size, offset, PATH, false, 600) == EINVAL);

#ifdef _WIN32
    ASSERT(sg_httpres_sendfile(res, size, max_size, offset, "", false, 200) == EACCES);
#else
    ASSERT(sg_httpres_sendfile(res, size, max_size, offset, "", false, 200) == ENOENT);
#endif
    dir = sg_tmpdir();
#ifdef _WIN32
    ASSERT(sg_httpres_sendfile(res, size, max_size, offset, dir, false, 200) == EACCES);
#else
    ASSERT(sg_httpres_sendfile(res, size, max_size, offset, dir, false, 200) == EISDIR);
#endif
    sg_free(dir);

    strcpy(str, "foo");
    unlink(PATH);
    file = fopen(PATH, "w");
    ASSERT(file);
    ASSERT(fwrite(str, 1, len, file) == len);
    ASSERT(fclose(file) == 0);
    ASSERT(sg_httpres_sendfile(res, size, 1, offset, PATH, false, 200) == EFBIG);

    ASSERT(sg_httpres_sendfile(res, size, len, offset, PATH, true, 200) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "attachment; filename=\""
                   FILENAME
                   "\"") == 0);

    ASSERT(sg_httpres_sendfile(res, size, len, offset, PATH, true, 200) == EALREADY);

    sg_free(res->handle);
    res->handle = NULL;
    sg_strmap_cleanup(sg_httpres_headers(res));
    ASSERT(sg_httpres_sendfile(res, size, len, offset, PATH, false, 201) == 0);
    ASSERT(!sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION));
    ASSERT(res->status == 201);
#undef PATH
#undef FILENAME
    sg_free(res->handle);
    res->handle = NULL;
}

static void test_httpres_sendstream(struct sg_httpres *res) {
    char *str;
    size_t size = sizeof(int);
    int buf = 1;
    ASSERT(sg_httpres_sendstream(NULL, size, dummy_read_cb, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_sendstream(res, (uint64_t) -1, NULL, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_sendstream(res, size, NULL, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_sendstream(res, size, dummy_read_cb, &buf, dummy_free_cb, 99) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_sendstream(res, size, dummy_read_cb, &buf, dummy_free_cb, 600) == EINVAL);
    ASSERT(buf == 0);

    size = sizeof(str);
    str = sg_alloc(size);
    ASSERT(sg_httpres_sendstream(res, 0, dummy_read_cb, str, dummy_free_cb, 200) == 0);
    sg_free(res->handle);
    res->handle = NULL;
    ASSERT(sg_httpres_sendstream(res, size, dummy_read_cb, str, dummy_free_cb, 201) == 0);
    ASSERT(res->status == 201);
    ASSERT(sg_httpres_sendstream(res, size, dummy_read_cb, str, dummy_free_cb, 200) == EALREADY);
    sg_free(res->handle);
    res->handle = NULL;
    sg_free(str);
}

static void test_httpres_zsend(struct sg_httpres *res) {
    char *str = "foo";

    ASSERT(sg_httpres_zsend(NULL, str, "text/plain", 200) == EINVAL);
    str = NULL;
    ASSERT(sg_httpres_zsend(res, str, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_zsend(res, str, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_zsend(res, str, "text/plain", 99) == EINVAL);
    ASSERT(sg_httpres_zsend(res, str, "text/plain", 600) == EINVAL);

    str = "foooo";
    res->status = 0;
    sg_strmap_cleanup(&res->headers);
    ASSERT(sg_httpres_zsend(res, str, "text/plain", 200) == 0);
    ASSERT(!sg_strmap_get(res->headers, "Content-Encoding"));
    ASSERT(strcmp(sg_strmap_get(res->headers, "Content-Type"), "text/plain") == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    str = "fooooooooooobaaaaaaaaaarrrrrrrrrrr";
    res->status = 0;
    ASSERT(sg_httpres_zsend(res, str, "text/plain", 200) == 0);
    ASSERT(strcmp(sg_strmap_get(res->headers, "Content-Encoding"), "deflate") == 0);
    ASSERT(strcmp(sg_strmap_get(res->headers, "Content-Type"), "text/plain") == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;

    res->status = 0;
    str = "foo";
    ASSERT(sg_httpres_zsend(res, str, "", 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 0;
    ASSERT(sg_httpres_zsend(res, "", "", 204) == 0); /* No content. */
    ASSERT(res->status == 204);
    MHD_destroy_response(res->handle);
    res->handle = NULL;

    res->status = 0;
    ASSERT(sg_httpres_zsend(res, str, "text/plain", 201) == 0);
    ASSERT(res->status == 201);
    ASSERT(sg_httpres_zsend(res, str, "text/plain", 200) == EALREADY);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_TYPE), "text/plain") == 0);
    ASSERT(res->status == 201);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
}

static void test_httpres_zsendbinary2(struct sg_httpres *res) {
    char *str = "foo";
    size_t len = strlen(str);

    ASSERT(sg_httpres_zsendbinary2(NULL, -1, str, len, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_zsendbinary2(res, -2, str, len, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_zsendbinary2(res, 10, str, len, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_zsendbinary2(res, -1, NULL, len, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_zsendbinary2(res, -1, str, (size_t) -1, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_zsendbinary2(res, -1, str, len, "text/plain", 99) == EINVAL);
    ASSERT(sg_httpres_zsendbinary2(res, -1, str, len, "text/plain", 600) == EINVAL);

    str = "foooo";
    len = strlen(str);
    res->status = 0;
    sg_strmap_cleanup(&res->headers);
    ASSERT(sg_httpres_zsendbinary2(res, -1, str, len, "text/plain", 200) == 0);
    ASSERT(!sg_strmap_get(res->headers, "Content-Encoding"));
    ASSERT(strcmp(sg_strmap_get(res->headers, "Content-Type"), "text/plain") == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    str = "fooooooooooobaaaaaaaaaarrrrrrrrrrr";
    len = strlen(str);
    res->status = 0;
    ASSERT(sg_httpres_zsendbinary2(res, -1, str, len, "text/plain", 200) == 0);
    ASSERT(strcmp(sg_strmap_get(res->headers, "Content-Encoding"), "deflate") == 0);
    ASSERT(strcmp(sg_strmap_get(res->headers, "Content-Type"), "text/plain") == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;

    str = "foo";
    len = strlen(str);
    res->status = 0;
    ASSERT(sg_httpres_zsendbinary2(res, -1, "foo", 0, "text/plain", 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;

    res->status = 0;
    ASSERT(sg_httpres_zsendbinary2(res, -1, str, len, NULL, 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 0;
    ASSERT(sg_httpres_zsendbinary2(res, -1, str, len, "", 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 0;
    ASSERT(sg_httpres_zsendbinary2(res, -1, "", 0, "", 204) == 0); /* No content. */
    ASSERT(res->status == 204);
    MHD_destroy_response(res->handle);
    res->handle = NULL;

    res->status = 0;
    ASSERT(sg_httpres_zsendbinary2(res, -1, str, len, "text/plain", 201) == 0);
    ASSERT(res->status == 201);
    ASSERT(sg_httpres_zsendbinary2(res, -1, str, len, "text/plain", 200) == EALREADY);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_TYPE), "text/plain") == 0);
    ASSERT(res->status == 201);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
}

static void test_httpres_zsendbinary(struct sg_httpres *res) {
    char *str = "foo";
    size_t len = strlen(str);

    ASSERT(sg_httpres_zsendbinary(NULL, str, len, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_zsendbinary(res, NULL, len, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_zsendbinary(res, str, (size_t) -1, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_zsendbinary(res, str, len, "text/plain", 99) == EINVAL);
    ASSERT(sg_httpres_zsendbinary(res, str, len, "text/plain", 600) == EINVAL);

    str = "foooo";
    len = strlen(str);
    res->status = 0;
    sg_strmap_cleanup(&res->headers);
    ASSERT(sg_httpres_zsendbinary(res, str, len, "text/plain", 200) == 0);
    ASSERT(!sg_strmap_get(res->headers, "Content-Encoding"));
    ASSERT(strcmp(sg_strmap_get(res->headers, "Content-Type"), "text/plain") == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    str = "fooooooooooobaaaaaaaaaarrrrrrrrrrr";
    len = strlen(str);
    res->status = 0;
    ASSERT(sg_httpres_zsendbinary(res, str, len, "text/plain", 200) == 0);
    ASSERT(strcmp(sg_strmap_get(res->headers, "Content-Encoding"), "deflate") == 0);
    ASSERT(strcmp(sg_strmap_get(res->headers, "Content-Type"), "text/plain") == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;

    str = "foo";
    len = strlen(str);
    res->status = 0;
    ASSERT(sg_httpres_zsendbinary(res, "foo", 0, "text/plain", 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;

    res->status = 0;
    ASSERT(sg_httpres_zsendbinary(res, str, len, NULL, 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 0;
    ASSERT(sg_httpres_zsendbinary(res, str, len, "", 200) == 0);
    ASSERT(res->status == 200);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 0;
    ASSERT(sg_httpres_zsendbinary(res, "", 0, "", 204) == 0); /* No content. */
    ASSERT(res->status == 204);
    MHD_destroy_response(res->handle);
    res->handle = NULL;

    res->status = 0;
    ASSERT(sg_httpres_zsendbinary(res, str, len, "text/plain", 201) == 0);
    ASSERT(res->status == 201);
    ASSERT(sg_httpres_zsendbinary(res, str, len, "text/plain", 200) == EALREADY);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_TYPE), "text/plain") == 0);
    ASSERT(res->status == 201);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
}

static void test_httpres_zsendstream2(struct sg_httpres *res) {
    char *str;
    size_t size = sizeof(int);
    int buf = 1;
    ASSERT(sg_httpres_zsendstream2(NULL, -1, size, dummy_read_cb, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_zsendstream2(res, -2, size, dummy_read_cb, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_zsendstream2(res, 10, size, dummy_read_cb, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_zsendstream2(res, -1, size, NULL, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_zsendstream2(res, -1, size, dummy_read_cb, &buf, dummy_free_cb, 99) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_zsendstream2(res, -1, size, dummy_read_cb, &buf, dummy_free_cb, 600) == EINVAL);
    ASSERT(buf == 0);

    str = sg_alloc(sizeof(str));
    ASSERT(sg_httpres_zsendstream2(res, -1, size, dummy_read_cb, str, dummy_free_cb, 200) == 0);
    sg_free(res->handle);
    res->handle = NULL;
    ASSERT(sg_httpres_zsendstream2(res, -1, size, dummy_read_cb, str, dummy_free_cb, 201) == 0);
    ASSERT(res->status == 201);
    ASSERT(sg_httpres_zsendstream2(res, -1, size, dummy_read_cb, str, dummy_free_cb, 200) == EALREADY);
    sg_free(res->handle);
    res->handle = NULL;
    sg_free(str);
}

static void test_httpres_zsendstream(struct sg_httpres *res) {
    char *str;
    int buf = 1;
    ASSERT(sg_httpres_zsendstream(NULL, dummy_read_cb, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_zsendstream(res, NULL, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_zsendstream(res, dummy_read_cb, &buf, dummy_free_cb, 99) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_zsendstream(res, dummy_read_cb, &buf, dummy_free_cb, 600) == EINVAL);
    ASSERT(buf == 0);

    str = sg_alloc(sizeof(str));
    ASSERT(sg_httpres_zsendstream(res, dummy_read_cb, str, dummy_free_cb, 200) == 0);
    sg_free(res->handle);
    res->handle = NULL;
    ASSERT(sg_httpres_zsendstream(res, dummy_read_cb, str, dummy_free_cb, 201) == 0);
    ASSERT(res->status == 201);
    ASSERT(sg_httpres_zsendstream(res, dummy_read_cb, str, dummy_free_cb, 200) == EALREADY);
    sg_free(res->handle);
    res->handle = NULL;
    sg_free(str);
}

static void test_httpres_zdownload(struct sg_httpres *res) {
#define FILENAME "foo.txt"
#define PATH TEST_HTTPRES_BASE_PATH FILENAME
    const size_t len = 3;
    char str[4];
    FILE *file;
    char *dir;

    ASSERT(sg_httpres_zdownload(NULL, PATH) == EINVAL);
    ASSERT(sg_httpres_zdownload(res, NULL) == EINVAL);

#ifdef _WIN32
    ASSERT(sg_httpres_zdownload(res, "") == EACCES);
#else
    ASSERT(sg_httpres_zdownload(res, "") == ENOENT);
#endif
    dir = sg_tmpdir();
#ifdef _WIN32
    ASSERT(sg_httpres_zdownload(res, dir) == EACCES);
#else
    ASSERT(sg_httpres_zdownload(res, dir) == EISDIR);
#endif
    sg_free(dir);

    strcpy(str, "foo");
    unlink(PATH);
    file = fopen(PATH, "w");
    ASSERT(file);
    ASSERT(fwrite(str, 1, len, file) == len);
    ASSERT(fclose(file) == 0);
    ASSERT(sg_httpres_zdownload(res, PATH) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "attachment; filename=\""
                   FILENAME
                   "\"") == 0);
    ASSERT(sg_httpres_zdownload(res, PATH) == EALREADY);
    sg_free(res->handle);
    res->handle = NULL;
#undef PATH
#undef FILENAME
    sg_free(res->handle);
    res->handle = NULL;
}

static void test_httpres_zrender(struct sg_httpres *res) {
#define FILENAME "foo.txt"
#define PATH TEST_HTTPRES_BASE_PATH FILENAME
    const size_t len = 3;
    char str[4];
    FILE *file;
    char *dir;

    ASSERT(sg_httpres_zrender(NULL, PATH) == EINVAL);
    ASSERT(sg_httpres_zrender(res, NULL) == EINVAL);

#ifdef _WIN32
    ASSERT(sg_httpres_zrender(res, "") == EACCES);
#else
    ASSERT(sg_httpres_zrender(res, "") == ENOENT);
#endif
    dir = sg_tmpdir();
#ifdef _WIN32
    ASSERT(sg_httpres_zrender(res, dir) == EACCES);
#else
    ASSERT(sg_httpres_zrender(res, dir) == EISDIR);
#endif
    sg_free(dir);

    strcpy(str, "foo");
    unlink(PATH);
    file = fopen(PATH, "w");
    ASSERT(file);
    ASSERT(fwrite(str, 1, len, file) == len);
    ASSERT(fclose(file) == 0);
    ASSERT(sg_httpres_zrender(res, PATH) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "inline; filename=\""
                   FILENAME
                   "\"") == 0);
    ASSERT(sg_httpres_zrender(res, PATH) == EALREADY);
    sg_free(res->handle);
    res->handle = NULL;
#undef PATH
#undef FILENAME
    sg_free(res->handle);
    res->handle = NULL;
}

static void test_httpres_zsendfile2(struct sg_httpres *res) {
#define FILENAME "foo.txt"
#define PATH TEST_HTTPRES_BASE_PATH FILENAME
    const size_t len = 3;
    char str[4];
    FILE *file;
    char *dir;
    size_t size = sizeof(int);
    uint64_t max_size = 10, offset = 0;

    ASSERT(sg_httpres_zsendfile2(NULL, -1, size, max_size, offset, PATH, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile2(res, -2, size, max_size, offset, PATH, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile2(res, 10, size, max_size, offset, PATH, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile2(res, -1, (uint64_t) -1, max_size, offset, PATH, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile2(res, -1, size, (uint64_t) -1, offset, PATH, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile2(res, -1, size, max_size, (uint64_t) -1, PATH, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile2(res, -1, size, max_size, offset, NULL, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile2(res, -1, size, max_size, offset, PATH, NULL, 99) == EINVAL);
    ASSERT(sg_httpres_zsendfile2(res, -1, size, max_size, offset, PATH, NULL, 600) == EINVAL);

#ifdef _WIN32
    ASSERT(sg_httpres_zsendfile2(res, -1, size, max_size, offset, "", NULL, 200) == EACCES);
#else
    ASSERT(sg_httpres_zsendfile2(res, -1, size, max_size, offset, "", NULL, 200) == ENOENT);
#endif
    dir = sg_tmpdir();
#ifdef _WIN32
    ASSERT(sg_httpres_zsendfile2(res, -1, size, max_size, offset, dir, NULL, 200) == EACCES);
#else
    ASSERT(sg_httpres_zsendfile2(res, -1, size, max_size, offset, dir, NULL, 200) == EISDIR);
#endif
    sg_free(dir);

    strcpy(str, "foo");
    unlink(PATH);
    file = fopen(PATH, "w");
    ASSERT(file);
    ASSERT(fwrite(str, 1, len, file) == len);
    ASSERT(fclose(file) == 0);
    ASSERT(sg_httpres_zsendfile2(res, -1, size, 1, offset, PATH, NULL, 200) == EFBIG);

    ASSERT(sg_httpres_zsendfile2(res, -1, size, len, offset, PATH, "attachment", 200) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "attachment; filename=\""
                   FILENAME
                   "\"") == 0);

    ASSERT(sg_httpres_zsendfile2(res, -1, size, len, offset, PATH, "attachment", 200) == EALREADY);

    sg_free(res->handle);
    res->handle = NULL;
    ASSERT(sg_httpres_zsendfile2(res, -1, size, len, offset, PATH, "inline", 201) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "inline; filename=\""
                   FILENAME
                   "\"") == 0);
    ASSERT(res->status == 201);

    sg_free(res->handle);
    res->handle = NULL;
    sg_strmap_cleanup(sg_httpres_headers(res));
    ASSERT(sg_httpres_zsendfile2(res, -1, size, len, offset, PATH, NULL, 200) == 0);
    ASSERT(!sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION));
    ASSERT(res->status == 200);

    sg_free(res->handle);
    res->handle = NULL;
    ASSERT(sg_httpres_zsendfile2(res, -1, size, len, offset, PATH, "abc123", 201) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "abc123; filename=\""
                   FILENAME
                   "\"") == 0);
    ASSERT(res->status == 201);
#undef PATH
#undef FILENAME
    sg_free(res->handle);
    res->handle = NULL;
}

static void test_httpres_zsendfile(struct sg_httpres *res) {
#define FILENAME "foo.txt"
#define PATH TEST_HTTPRES_BASE_PATH FILENAME
    const size_t len = 3;
    char str[4];
    FILE *file;
    char *dir;
    size_t size = sizeof(int);
    uint64_t max_size = 10, offset = 0;

    ASSERT(sg_httpres_zsendfile(NULL, size, max_size, offset, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile(res, (uint64_t) -1, max_size, offset, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile(res, size, (uint64_t) -1, offset, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile(res, size, max_size, (uint64_t) -1, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile(res, size, max_size, offset, NULL, false, 200) == EINVAL);
    ASSERT(sg_httpres_zsendfile(res, size, max_size, offset, PATH, false, 99) == EINVAL);
    ASSERT(sg_httpres_zsendfile(res, size, max_size, offset, PATH, false, 600) == EINVAL);

#ifdef _WIN32
    ASSERT(sg_httpres_zsendfile(res, size, max_size, offset, "", false, 200) == EACCES);
#else
    ASSERT(sg_httpres_zsendfile(res, size, max_size, offset, "", false, 200) == ENOENT);
#endif
    dir = sg_tmpdir();
#ifdef _WIN32
    ASSERT(sg_httpres_zsendfile(res, size, max_size, offset, dir, false, 200) == EACCES);
#else
    ASSERT(sg_httpres_zsendfile(res, size, max_size, offset, dir, false, 200) == EISDIR);
#endif
    sg_free(dir);

    strcpy(str, "foo");
    unlink(PATH);
    file = fopen(PATH, "w");
    ASSERT(file);
    ASSERT(fwrite(str, 1, len, file) == len);
    ASSERT(fclose(file) == 0);
    ASSERT(sg_httpres_zsendfile(res, size, 1, offset, PATH, false, 200) == EFBIG);

    ASSERT(sg_httpres_zsendfile(res, size, len, offset, PATH, true, 200) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "attachment; filename=\""
                   FILENAME
                   "\"") == 0);

    ASSERT(sg_httpres_zsendfile(res, size, len, offset, PATH, true, 200) == EALREADY);

    sg_free(res->handle);
    res->handle = NULL;
    sg_strmap_cleanup(sg_httpres_headers(res));
    ASSERT(sg_httpres_zsendfile(res, size, len, offset, PATH, false, 201) == 0);
    ASSERT(!sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION));
    ASSERT(res->status == 201);
#undef PATH
#undef FILENAME
    sg_free(res->handle);
    res->handle = NULL;
}

static void test_httpres_clear(struct sg_httpres *res) {
    struct sg_strmap **headers;
    ASSERT(sg_httpres_clear(NULL) == EINVAL);

    headers = sg_httpres_headers(res);
    ASSERT(sg_strmap_add(headers, "foo", "bar") == 0);
    ASSERT(sg_strmap_add(headers, "lorem", "ipsum") == 0);
    ASSERT(sg_httpres_set_cookie(res, "my", "cookie") == 0);
    ASSERT(sg_httpres_send(res, "", "", 200) == 0);

    ASSERT(res->handle);
    ASSERT(strcmp(sg_strmap_get(*headers, "foo"), "bar") == 0);
    ASSERT(strcmp(sg_strmap_get(*headers, "lorem"), "ipsum") == 0);
    ASSERT(strcmp(sg_strmap_get(*headers, "Set-Cookie"), "my=cookie") == 0);
    ASSERT(res->status == 200);

    ASSERT(sg_httpres_clear(res) == 0);

    ASSERT(!res->handle);
    ASSERT(!sg_strmap_get(*headers, "foo"));
    ASSERT(!sg_strmap_get(*headers, "lorem"));
    ASSERT(!sg_strmap_get(*headers, "Set-Cookie"));
    ASSERT(res->status == 500);
}

int main(void) {
    struct sg_httpres *res = sg__httpres_new(NULL);
    ASSERT(res);
    test__httpres_new();
    test__httpres_free();
    test__httpres_dispatch(res);
    test_httpres_headers(res);
    test_httpres_set_cookie(res);
    test_httpres_send(res);
    test_httpres_sendbinary(res);
    test_httpres_download(res);
    test_httpres_render(res);
    test_httpres_sendfile2(res);
    test_httpres_sendfile(res);
    test_httpres_sendstream(res);
    test_httpres_zsend(res);
    test_httpres_zsendbinary2(res);
    test_httpres_zsendbinary(res);
    test_httpres_zsendstream2(res);
    test_httpres_zsendstream(res);
    test_httpres_zdownload(res);
    test_httpres_zrender(res);
    test_httpres_zsendfile2(res);
    test_httpres_zsendfile(res);
    test_httpres_clear(res);
    sg__httpres_free(res);
    return EXIT_SUCCESS;
}
