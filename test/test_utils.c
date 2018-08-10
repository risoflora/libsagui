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

#ifdef _WIN32
#include <windows.h>
#endif
#include <string.h>
#include <sagui.h>
#include "sg_utils.h"

static void test__strdup(void) {
    char *str1, *str2;
    ASSERT(!sg__strdup(NULL));
    str1 = sg__strdup("");
    ASSERT(strcmp(str1, "") == 0);
    sg_free(str1);
    str1 = sg__strdup("abc123");
    ASSERT(strcmp(str1, "abc123") == 0);
    str2 = sg__strdup(str1);
    ASSERT(str1 != str2);
    ASSERT(strcmp(str1, str2) == 0);
    sg_free(str1);
    sg_free(str2);
}

static void test__toasciilower(void) {
    char str[100];
    memset(str, 0, sizeof(str));
    ASSERT(strcmp(str, "") == 0);
    sg__toasciilower(str);
    ASSERT(strcmp(str, "") == 0);
    sprintf(str, "A");
    ASSERT(strcmp(str, "A") == 0);
    sg__toasciilower(str);
    ASSERT(strcmp(str, "a") == 0);
    sprintf(str, "ABC");
    sg__toasciilower(str);
    ASSERT(strcmp(str, "abc") == 0);
    sprintf(str, "ABC123 def456");
    sg__toasciilower(str);
    ASSERT(strcmp(str, "abc123 def456") == 0);
    sprintf(str, "ABC");
    sg__toasciilower(str);
    ASSERT(strcmp(str, "abc") == 0);
    sprintf(str, "ABÇñãÁÊD");
    sg__toasciilower(str);
    ASSERT(strcmp(str, "abÇñãÁÊd") == 0);
}

static void test__strjoin(void) {
    char *str;
    ASSERT(!sg__strjoin(0, "", ""));
    ASSERT(!sg__strjoin('/', NULL, ""));
    ASSERT(!sg__strjoin('/', "", NULL));
    str = sg__strjoin('/', "", "");
    ASSERT(strcmp(str, "") == 0);
    sg_free(str);
    str = sg__strjoin('/', "abc", "");
    ASSERT(strcmp(str, "abc") == 0);
    sg_free(str);
    str = sg__strjoin('/', "", "123");
    ASSERT(strcmp(str, "123") == 0);
    sg_free(str);
    str = sg__strjoin('/', "abc", "123");
    ASSERT(strcmp(str, "abc/123") == 0);
    sg_free(str);
    str = sg__strjoin('/', "a", "b");
    ASSERT(strcmp(str, "a/b") == 0);
    sg_free(str);
}

static void test__is_cookie_name(void) {
    char *str = strdup("");
    ASSERT(sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("a");
    ASSERT(sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc");
    ASSERT(sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc_123");
    ASSERT(sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("123_abc");
    ASSERT(sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc-123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc 123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc|123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abç");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc\a123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc(123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc)123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc<123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc>123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc@123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc,123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc;123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc:123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc\\123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc\"123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc/123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc[123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc]123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc?123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc=123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc{123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
    str = strdup("abc}123");
    ASSERT(!sg__is_cookie_name(str));
    sg_free(str);
}

static void test__is_cookie_val(void) {
    char *str = strdup("");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("a");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("abc");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("abc_123");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("123_abc");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("abc=123");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("abc-123");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("\"abc123\"");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("abc 123");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("abc\"123");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("abc,123");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("abc;123");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("abc\\123");
    ASSERT(sg__is_cookie_val(str));
    sg_free(str);
    str = strdup("abc\a123");
    ASSERT(!sg__is_cookie_val(str));
    sg_free(str);
}

static void test_version(void) {
    const char *ver_original;
    char ver_local[9];
    size_t ver_len;

    ASSERT(sg_version() > 0);

    ver_original = sg_version_str();
    ASSERT(ver_original);
    ver_len = strlen(ver_original);
    ASSERT(ver_len > 0);

    sprintf(ver_local, "%d.%d.%d", SG_VERSION_MAJOR, SG_VERSION_MINOR, SG_VERSION_PATCH);
    ASSERT(strcmp(ver_original, ver_local) == 0);

    ASSERT(ver_original[ver_len] == '\0');
}

static void test_alloc(void) {
    char *buf;
#define TEST_MEM_BUF_LEN 10
    buf = sg_alloc(TEST_MEM_BUF_LEN);
    ASSERT(buf);
    for (unsigned char i = 0; i < TEST_MEM_BUF_LEN; i++)
        ASSERT(buf[i] == 0);
    memset(buf, 'a', TEST_MEM_BUF_LEN - 1);
    buf[TEST_MEM_BUF_LEN - 1] = '\0';
    ASSERT(strlen(buf) == TEST_MEM_BUF_LEN - 1);
#undef TEST_MEM_BUF_LEN
    sg_free(buf);
}

static void test_realloc(void) {
    char *buf;
#define TEST_MEM_BUF_LEN 10
    buf = sg_alloc(TEST_MEM_BUF_LEN / 2);
    ASSERT(buf);
    buf = sg_realloc(buf, TEST_MEM_BUF_LEN);
    ASSERT(buf);
    memset(buf, 'a', TEST_MEM_BUF_LEN - 1);
    buf[TEST_MEM_BUF_LEN - 1] = '\0';
    ASSERT(strlen(buf) == TEST_MEM_BUF_LEN - 1);
#undef TEST_MEM_BUF_LEN
    sg_free(buf);
}

static void test_free(void) {
    sg_free(NULL);
}

static void test_strerror(void) {
    char err[256];
    ASSERT(!sg_strerror(0, NULL, sizeof(err)));
    ASSERT(!sg_strerror(0, err, 0));
    ASSERT(sg_strerror(EINTR, err, sizeof(err)) == err);
    ASSERT(strcmp(err, strerror(EINTR)) == 0);
    ASSERT(sg_strerror(EINVAL, err, sizeof(err)) == err);
    ASSERT(strcmp(err, strerror(EINVAL)) == 0);
}

static void test_is_post(void) {
    ASSERT(!sg_is_post(NULL));
    ASSERT(!sg_is_post(""));
    ASSERT(!sg_is_post("abc"));
    ASSERT(!sg_is_post("GET"));
    ASSERT(!sg_is_post("HEAD"));
    ASSERT(sg_is_post("POST"));
    ASSERT(sg_is_post("PUT"));
    ASSERT(sg_is_post("DELETE"));
    ASSERT(sg_is_post("OPTIONS"));
}

static void test_tmpdir(void) {
#ifdef _WIN32
    char path[MAX_PATH + 1];
    size_t len;
#endif
    char *tmp = sg_tmpdir();
    ASSERT(tmp != NULL);
#if defined(_WIN32)
    len = GetTempPath(MAX_PATH + 1, path);
    if (path[len - 1] == '\\') {
        len--;
        path[len] = '\0';
    }
    ASSERT(strcmp(tmp, path) == 0);
#elif defined(__ANDROID__)
    ASSERT(strcmp(tmp, "/data/local/tmp") == 0);
#else
    ASSERT(strcmp(tmp, "/tmp") == 0);
#endif
    sg_free(tmp);
}

int main(void) {
    test__strdup();
    test__toasciilower();
    test__strjoin();
    test__is_cookie_name();
    test__is_cookie_val();
    test_version();
    test_alloc();
    test_realloc();
    test_free();
    test_strerror();
    test_is_post();
    test_tmpdir();
    return EXIT_SUCCESS;
}
