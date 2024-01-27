/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2024 Silvio Clecio <silvioprog@gmail.com>
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

#include <errno.h>
#include <string.h>
#include <sagui.h>

static void test_str_write(struct sg_str *str, const char *val, size_t len) {
  ASSERT(sg_str_write(NULL, val, len) == EINVAL);
  ASSERT(sg_str_write(str, NULL, len) == EINVAL);
  ASSERT(sg_str_write(str, val, 0) == EINVAL);

  sg_str_clear(str);
  ASSERT(sg_str_write(str, val, len) == 0);
  ASSERT(sg_str_length(str) == len);
}

static void test_str_printf_va(struct sg_str *str, const char *fmt,
                               va_list ap) {
  ASSERT(sg_str_printf_va(NULL, fmt, ap) == EINVAL);
  ASSERT(sg_str_printf_va(str, NULL, ap) == EINVAL);
#if !(defined(__arm__) || defined(__arm64__))
  ASSERT(sg_str_printf_va(str, fmt, NULL) == EINVAL);
#endif /* !(__arm__ || __arm64__) */

  sg_str_clear(str);
  sg_str_printf_va(str, fmt, ap);
  ASSERT(strcmp(sg_str_content(str), "abc123def456") == 0);
}

static void test_str_printf(struct sg_str *str, const char *fmt, ...) {
  va_list ap;
  ASSERT(sg_str_printf(NULL, "%s", "") == EINVAL);
  ASSERT(sg_str_printf(str, NULL) == EINVAL); //-V::575,618

  sg_str_clear(str);
  ASSERT(sg_str_printf(str, "%s", "") == 0);
  ASSERT(strlen(sg_str_content(str)) == 0);
  ASSERT(sg_str_printf(str, "%s%d", "abc", 123) == 0);
  ASSERT(strcmp(sg_str_content(str), "abc123") == 0);

  va_start(ap, fmt);
  test_str_printf_va(str, fmt, ap);
  va_end(ap);
}

static void test_str_content(struct sg_str *str, const char *val, size_t len) {
  errno = 0;
  ASSERT(!sg_str_content(NULL));
  ASSERT(errno == EINVAL);

  sg_str_clear(str);
  ASSERT(strlen(sg_str_content(str)) == 0);
  sg_str_write(str, val, len);
  ASSERT(strcmp(sg_str_content(str), val) == 0);
}

static void test_str_length(struct sg_str *str, const char *val, size_t len) {
  errno = 0;
  ASSERT(sg_str_length(NULL) == 0);
  ASSERT(errno == EINVAL);

  sg_str_clear(str);
  ASSERT(sg_str_length(str) == 0);
  sg_str_write(str, val, len);
  ASSERT(sg_str_length(str) == len);
}

static void test_str_clear(struct sg_str *str, const char *val, size_t len) {
  ASSERT(sg_str_clear(NULL) == EINVAL);

  sg_str_clear(str);
  sg_str_write(str, val, len);
  ASSERT(sg_str_length(str) > 0);
  sg_str_clear(str);
  ASSERT(sg_str_length(str) == 0);
}

int main(void) {
  struct sg_str *str;
  const char *val = "abc123def456";
  const size_t len = strlen(val);

  str = sg_str_new();
  ASSERT(str);

  test_str_write(str, val, len);
  test_str_printf(str, "%s%d%s%d", "abc", 123, "def", 456);
  /* the function `sg_str_printf_va()` is already tested by `test_str_printf()`. */
  test_str_content(str, val, len);
  test_str_length(str, val, len);
  test_str_clear(str, val, len);

  sg_str_free(str);
  return EXIT_SUCCESS;
}
