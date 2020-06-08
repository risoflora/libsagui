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

#include "sg_assert.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#ifdef _WIN32
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#else /* _WIN32 */
#include <sys/socket.h>
#endif /* _WIN32 */
#include "sg_macros.h"
#include "sg_utils.h"
#include <sagui.h>

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

static void test__pow(void) {
  ASSERT(sg__pow(1, 2) == 0);
}

static void test__fmod(void) {
  ASSERT(sg__fmod(1, 2) == 0);
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

  ASSERT(sg_version() == ((SG_VERSION_MAJOR << 16) | (SG_VERSION_MINOR << 8) |
                          (SG_VERSION_PATCH)));

  ver_original = sg_version_str();
  ASSERT(ver_original);
  ver_len = strlen(ver_original);
  ASSERT(ver_len > 0);

  sprintf(ver_local, "%d.%d.%d", SG_VERSION_MAJOR, SG_VERSION_MINOR,
          SG_VERSION_PATCH);
  ASSERT(strcmp(ver_original, ver_local) == 0);

  ASSERT(ver_original[ver_len] == '\0');
}

static int mm_tester = 0;

static void *mm_malloc(size_t size) {
  mm_tester = size;
  return &mm_tester;
}

static void *mm_realloc(void *ptr, size_t size) {
  *((int *) ptr) = size;
  return ptr;
}

static void mm_free(void *ptr) {
  *((int *) ptr) = 0;
}

static void test_mm_set(void) {
  ASSERT(sg_mm_set(NULL, mm_realloc, mm_free) == EINVAL);
  ASSERT(sg_mm_set(mm_malloc, NULL, mm_free) == EINVAL);
  ASSERT(sg_mm_set(mm_malloc, mm_realloc, NULL) == EINVAL);

  ASSERT(sg_mm_set(mm_malloc, mm_realloc, mm_free) == 0);

  mm_tester = 0;
  ASSERT(sg_malloc(123) == &mm_tester);
  ASSERT(mm_tester == 123);
  ASSERT(sg_realloc(&mm_tester, 456) == &mm_tester);
  ASSERT(mm_tester == 456);
  sg_free(&mm_tester);
  ASSERT(mm_tester == 0);

  sg_mm_set(malloc, realloc, free);
}

static void test_malloc(void) {
  char *buf;
#define TEST_MEM_BUF_LEN 10
  buf = sg_malloc(TEST_MEM_BUF_LEN);
  ASSERT(buf);
  memset(buf, 'a', TEST_MEM_BUF_LEN - 1);
  buf[TEST_MEM_BUF_LEN - 1] = '\0';
  ASSERT(strlen(buf) == TEST_MEM_BUF_LEN - 1);
#undef TEST_MEM_BUF_LEN
  sg_free(buf);
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

static double math_tester_x = 0;
static double math_tester_y = 0;

static double math_pow(double x, double y) {
  math_tester_x = x;
  math_tester_y = y;
  return 789;
}

static double math_fmod(double x, double y) {
  math_tester_x = x;
  math_tester_y = y;
  return 789;
}

static void test_math_set(void) {
  ASSERT(sg_math_set(NULL, math_fmod) == EINVAL);
  ASSERT(sg_math_set(math_pow, NULL) == EINVAL);

  ASSERT(sg_math_set(math_pow, math_fmod) == 0);

  math_tester_x = 0;
  math_tester_y = 0;
  ASSERT(sg__pow(123, 456) == 789);
  ASSERT(math_tester_x == 123);
  ASSERT(math_tester_y == 456);
  math_tester_x = 0;
  math_tester_y = 0;
  ASSERT(sg__fmod(123, 456) == 789);
  ASSERT(math_tester_x == 123);
  ASSERT(math_tester_y == 456);

  sg_math_set(sg__pow, sg__fmod);
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

static void test_extract_entrypoint(void) {
  char *str;
  errno = 0;
  ASSERT(!sg_extract_entrypoint(NULL));
  ASSERT(errno == EINVAL);

  ASSERT((str = sg_extract_entrypoint("")));
  ASSERT(strcmp(str, "/") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("/")));
  ASSERT(strcmp(str, "/") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("//")));
  ASSERT(strcmp(str, "/") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("///////")));
  ASSERT(strcmp(str, "/") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("foo")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("/foo")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("//foo")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("///////foo")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("foo/")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("foo//")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("/foo/")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("///foo///")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("/foo/bar")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("///foo/bar")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("/foo///bar")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("///foo///bar")));
  ASSERT(strcmp(str, "/foo") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("/a")));
  ASSERT(strcmp(str, "/a") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("/a/b")));
  ASSERT(strcmp(str, "/a") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("//a/b")));
  ASSERT(strcmp(str, "/a") == 0);
  sg_free(str);
  ASSERT((str = sg_extract_entrypoint("//a//b")));
  ASSERT(strcmp(str, "/a") == 0);
  sg_free(str);
}

static void test_tmpdir(void) {
#ifdef _WIN32
  char path[MAX_PATH + 1];
  size_t len;
#endif /* _WIN32 */
  char *tmp = sg_tmpdir();
  ASSERT(tmp != NULL);
#if defined(_WIN32)
  len = GetTempPath(MAX_PATH + 1, path);
  if (path[len - 1] == '\\') {
    len--;
    path[len] = '\0';
  }
  ASSERT(strcmp(tmp, path) == 0);
#elif defined(__ANDROID__) /* _WIN32 */
  ASSERT(strcmp(tmp, "/data/local/tmp") == 0);
#else /* __ANDROID__ */
  ASSERT(strcmp(tmp, "/tmp") == 0);
#endif /* _WIN32 */
  sg_free(tmp);
}

static void test_ip(void) {
  struct sockaddr sa;
  char buf[46];
  memset(&sa, 0, sizeof(struct sockaddr));
  ASSERT(sg_ip(NULL, buf, sizeof(buf)) == EINVAL);
  ASSERT(sg_ip(&sa, NULL, sizeof(buf)) == EINVAL);
  ASSERT(sg_ip(&sa, buf, -1) == EINVAL);

  sa.sa_family = AF_INET;
  ASSERT(sg_ip(&sa, buf, sizeof(buf)) == 0);
  ASSERT(strcmp(buf, "0.0.0.0") == 0);

  /* we do not need massive testing for inet_ntop() since it is already tested by the glibc team. */
}

int main(void) {
  test__strdup();
  test__pow();
  test__fmod();
  test__toasciilower();
  test__strjoin();
  test__is_cookie_name();
  test__is_cookie_val();
  test_version();
  test_mm_set();
  test_malloc();
  test_alloc();
  test_realloc();
  test_free();
  test_math_set();
  test_strerror();
  test_is_post();
  test_extract_entrypoint();
  test_tmpdir();
  test_ip();
  return EXIT_SUCCESS;
}
