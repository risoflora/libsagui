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

#define SG_EXTERN

#include "sg_assert.h"

#include <string.h>
#include "sg_httpres.c"

#ifndef TEST_HTTPRES_BASE_PATH
#ifdef __ANDROID__
#define TEST_HTTPRES_BASE_PATH SG_ANDROID_TESTS_DEST_DIR "/"
#else
#define TEST_HTTPRES_BASE_PATH ""
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

static void test__httpfileread_cb(void) {
    const char *path = TEST_HTTPRES_BASE_PATH "foo.txt";
    const size_t len = 3;
    char str[4];
    FILE *file;
    unlink(path);
    ASSERT((file = fopen(path, "w")));
    ASSERT(fwrite("foo", 1, len, file) == len);
    sg__httpfilefree_cb(file);
    ASSERT(access(path, F_OK) == 0);
    ASSERT((file = fopen(path, "r")));
    memset(str, 0, sizeof(str));
    ASSERT(strcmp(str, "") == 0);
    ASSERT(sg__httpfileread_cb(file, 0, str, len) == (ssize_t) len);
    ASSERT(strcmp(str, "foo") == 0);
    sg__httpfilefree_cb(file);
}

static void test__httpfilefree_cb(void) {
    const char *path = TEST_HTTPRES_BASE_PATH "foo.txt";
    const size_t len = 3;
    char str[4];
    FILE *file;
    unlink(path);
    ASSERT((file = fopen(path, "w")));
    ASSERT(fwrite("foo", 1, len, file) == len);
    sg__httpfilefree_cb(file);
    ASSERT(access(path, F_OK) == 0);
    ASSERT((file = fopen(path, "r")));
    memset(str, 0, sizeof(str));
    ASSERT(strcmp(str, "") == 0);
    ASSERT(fread(str, 1, len, file) == len);
    ASSERT(strcmp(str, "foo") == 0);
    sg__httpfilefree_cb(file);
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
    ASSERT((headers = sg_httpres_headers(res)));
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

static void test_httpres_sendbinary(struct sg_httpres *res) {
    char *str = "foo";
    const size_t len = strlen(str);

    ASSERT(sg_httpres_sendbinary(NULL, str, len, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_sendbinary(res, NULL, len, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_sendbinary(res, str, (size_t) -1, "text/plain", 200) == EINVAL);
    ASSERT(sg_httpres_sendbinary(res, str, len, NULL, 200) == EINVAL);
    ASSERT(sg_httpres_sendbinary(res, str, len, "text/plain", 99) == EINVAL);
    ASSERT(sg_httpres_sendbinary(res, str, len, "text/plain", 600) == EINVAL);

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

static void test_httpres_sendfile(struct sg_httpres *res) {
#define FILENAME "foo.txt"
#define PATH TEST_HTTPRES_BASE_PATH FILENAME
    const size_t len = 3;
    char str[4];
    FILE *file;
    char *dir;
    size_t block_size = sizeof(int);
    uint64_t max_size = 10;

    ASSERT(sg_httpres_sendfile(NULL, block_size, max_size, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, 0, max_size, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, block_size, (uint64_t) -1, PATH, false, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, block_size, max_size, NULL, false, 200) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, block_size, max_size, PATH, false, 99) == EINVAL);
    ASSERT(sg_httpres_sendfile(res, block_size, max_size, PATH, false, 600) == EINVAL);

#ifdef _WIN32
    ASSERT(sg_httpres_sendfile(res, block_size, max_size, "", false, 200) == EACCES);
#else
    ASSERT(sg_httpres_sendfile(res, block_size, max_size, "", false, 200) == ENOENT);
#endif
    dir = sg_tmpdir();
#ifdef _WIN32
    ASSERT(sg_httpres_sendfile(res, block_size, max_size, dir, false, 200) == EACCES);
#else
    ASSERT(sg_httpres_sendfile(res, block_size, max_size, dir, false, 200) == EISDIR);
#endif
    sg_free(dir);

    strcpy(str, "foo");
    unlink(PATH);
    ASSERT((file = fopen(PATH, "w")));
    ASSERT(fwrite(str, 1, len, file) == len);
    ASSERT(fclose(file) == 0);
    ASSERT(sg_httpres_sendfile(res, block_size, 1, PATH, false, 200) == EFBIG);

    ASSERT(sg_httpres_sendfile(res, block_size, len, PATH, true, 200) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "inline; filename=\"" FILENAME "\"") == 0);

    ASSERT(sg_httpres_sendfile(res, block_size, len, PATH, true, 200) == EALREADY);

    sg_free(res->handle);
    res->handle = NULL;
    ASSERT(sg_httpres_sendfile(res, block_size, len, PATH, false, 201) == 0);
    ASSERT(strcmp(sg_strmap_get(*sg_httpres_headers(res), MHD_HTTP_HEADER_CONTENT_DISPOSITION),
                  "attachment; filename=\"" FILENAME "\"") == 0);
    ASSERT(res->status == 201);
#undef PATH
#undef FILENAME
    sg_free(res->handle);
    res->handle = NULL;
}

static void test_httpres_sendstream(struct sg_httpres *res) {
    char *str;
    size_t size = sizeof(int), block_size = sizeof(int);
    int buf = 1;
    ASSERT(sg_httpres_sendstream(NULL, size, block_size, dummy_read_cb, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_sendstream(res, size, 0, dummy_read_cb, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_sendstream(res, size, block_size, NULL, &buf, dummy_free_cb, 200) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_sendstream(res, size, block_size, dummy_read_cb, &buf, dummy_free_cb, 99) == EINVAL);
    ASSERT(buf == 0);
    buf = 1;
    ASSERT(sg_httpres_sendstream(res, size, block_size, dummy_read_cb, &buf, dummy_free_cb, 600) == EINVAL);
    ASSERT(buf == 0);

    size = sizeof(str);
    str = sg_alloc(size);
    ASSERT(sg_httpres_sendstream(res, 0, block_size, dummy_read_cb, str, dummy_free_cb, 200) == 0);
    sg_free(res->handle);
    res->handle = NULL;
    ASSERT(sg_httpres_sendstream(res, size, block_size, dummy_read_cb, str, dummy_free_cb, 201) == 0);
    ASSERT((res->status = 201));
    ASSERT(sg_httpres_sendstream(res, size, block_size, dummy_read_cb, str, dummy_free_cb, 200) == EALREADY);
    sg_free(res->handle);
    res->handle = NULL;
    sg_free(str);
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
    test__httpfileread_cb();
    test__httpfilefree_cb();
    test__httpres_new();
    test__httpres_free();
    test__httpres_dispatch(res);
    test_httpres_headers(res);
    test_httpres_set_cookie(res);
    test_httpres_sendbinary(res);
    test_httpres_sendfile(res);
    test_httpres_sendstream(res);
    test_httpres_clear(res);
    sg__httpres_free(res);
    return EXIT_SUCCESS;
}
