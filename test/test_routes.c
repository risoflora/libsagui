/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2019 Silvio Clecio <silvioprog@gmail.com>
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

#define SG_EXTERN

#include "sg_assert.h"

#include <stdlib.h>
#ifdef PCRE2_JIT_SUPPORT
#include <stdint.h>
#endif
#include <errno.h>
#include "sg_macros.h"
#include "pcre2.h"
#include "sg_routes.c"

static void route_cb(__SG_UNUSED void *cls,
                     __SG_UNUSED struct sg_route *route) {
}

static int route_segments_empty_iter_cb(__SG_UNUSED void *cls,
                                        __SG_UNUSED unsigned int index,
                                        __SG_UNUSED const char *segment) {
  return 0;
}

static int route_segments_123_iter_cb(__SG_UNUSED void *cls,
                                      __SG_UNUSED unsigned int index,
                                      __SG_UNUSED const char *segment) {
  return 123;
}

static int route_segments_concat_iter_cb(void *cls,
                                         __SG_UNUSED unsigned int index,
                                         const char *segment) {
  const char *str = cls;
  sprintf(cls, "%s%d%s", str, index, segment);
  return 0;
}

static int route_vars_empty_iter_cb(__SG_UNUSED void *cls,
                                    __SG_UNUSED const char *name,
                                    __SG_UNUSED const char *val) {
  return 0;
}

static int route_vars_123_iter_cb(__SG_UNUSED void *cls,
                                  __SG_UNUSED const char *name,
                                  __SG_UNUSED const char *val) {
  return 123;
}

static int route_vars_concat_iter_cb(void *cls, const char *name,
                                     const char *val) {
  strcat(cls, name);
  strcat(cls, val);
  return 0;
}

static int routes_iter_empty_cb(__SG_UNUSED void *cls,
                                __SG_UNUSED struct sg_route *route) {
  return 0;
}

static int routes_iter_123_cb(__SG_UNUSED void *cls,
                              __SG_UNUSED struct sg_route *route) {
  return 123;
}

static int routes_iter_concat_cb(void *cls, struct sg_route *route) {
  strcat(cls, sg_route_rawpattern(route));
  return 0;
}

static void test__route_new(void) {
  struct sg_route *route;
  char err[SG_ERR_SIZE], errmsg[SG_ERR_SIZE];
#ifdef PCRE2_JIT_SUPPORT
  uint32_t opt;
#endif
  int errnum;
  ASSERT(!sg__route_new("some\\Kpattern", err, sizeof(err), &errnum, route_cb,
                        "foo"));
  ASSERT(errnum == EINVAL);
  ASSERT(strcmp(err, _("\\K is not not allowed.\n")) == 0);

  ASSERT(!sg__route_new("foo[bar", err, sizeof(err), &errnum, route_cb, "foo"));
  ASSERT(errnum == EINVAL);
  snprintf(errmsg, sizeof(errmsg),
           _("Pattern compilation failed at offset %d: %s.\n"), 9,
           "missing terminating ] for character class");
  ASSERT(strcmp(err, errmsg) == 0);

  ASSERT((route = sg__route_new("(*NOTEMPTY)pattern", err, sizeof(err), &errnum,
                                route_cb, "foo")));
  ASSERT(errnum == 0);
  ASSERT(strcmp("(*NOTEMPTY)pattern", route->pattern) == 0);
  ASSERT(route->cb == route_cb);
  ASSERT(strcmp("foo", route->cls) == 0);
  sg__route_free(route);

  /* TODO: test internal pcre2_jit_compile() and pcre2_match_data_create_from_pattern() errors. */

#ifdef PCRE2_JIT_SUPPORT
  ASSERT(pcre2_config(PCRE2_CONFIG_JIT, &opt) == 0);
  ASSERT(opt == 1);
#endif

  ASSERT((route = sg__route_new("pattern", err, sizeof(err), &errnum, route_cb,
                                "foo")));
  ASSERT(errnum == 0);
  ASSERT(strcmp("^pattern$", route->pattern) == 0);
  sg__route_free(route);
}

static void test_route_handle(void) {
  struct sg_route route;
  int dummy = 123;
  errno = 0;
  ASSERT(!sg_route_handle(NULL));
  ASSERT(errno == EINVAL);

  route.re = (pcre2_code *) &dummy;
  errno = 0;
  ASSERT(sg_route_handle(&route) == (pcre2_code *) &dummy);
  ASSERT(errno == 0);
}

static void test_route_match(void) {
  struct sg_route route;
  int dummy = 123;
  errno = 0;
  ASSERT(!sg_route_match(NULL));
  ASSERT(errno == EINVAL);

  route.match = (pcre2_match_data *) &dummy;
  errno = 0;
  ASSERT(sg_route_match(&route) == (pcre2_match_data *) &dummy);
  ASSERT(errno == 0);
}

static void test_route_rawpattern(void) {
  struct sg_route route;
  errno = 0;
  ASSERT(!sg_route_rawpattern(NULL));
  ASSERT(errno == EINVAL);

  route.pattern = NULL;
  errno = 0;
  ASSERT(!sg_route_rawpattern(&route));
  ASSERT(errno == 0);

  route.pattern = "^foo$";
  errno = 0;
  ASSERT(strcmp(sg_route_rawpattern(&route), "^foo$") == 0);
  ASSERT(errno == 0);
}

static void test_route_pattern(void) {
  struct sg_route route;
  char *pattern;
  errno = 0;
  ASSERT(!sg_route_pattern(NULL));
  ASSERT(errno == EINVAL);

  route.pattern = NULL;
  errno = 0;
  ASSERT(!sg_route_pattern(&route));
  ASSERT(errno == 0);

  route.pattern = "^foo$";
  errno = 0;
  ASSERT((pattern = sg_route_pattern(&route)));
  ASSERT(errno == 0);
  ASSERT(strcmp(pattern, "foo") == 0);
  sg_free(pattern);
}

static void test_route_path(void) {
  struct sg_route route;
  errno = 0;
  ASSERT(!sg_route_path(NULL));
  ASSERT(errno == EINVAL);

  route.path = NULL;
  errno = 0;
  ASSERT(!sg_route_path(&route));
  ASSERT(errno == 0);

  route.path = "/foo";
  errno = 0;
  ASSERT(strcmp(sg_route_path(&route), "/foo") == 0);
  ASSERT(errno == 0);
}

static void test_route_segments_iter(void) {
  struct sg_route *route;
  char err[SG_ERR_SIZE];
  char str[100];
  int errnum;
  ASSERT(sg_route_segments_iter(NULL, route_segments_empty_iter_cb, "foo") ==
         EINVAL);
  route =
    sg__route_new("/(foo)/(bar)", err, sizeof(err), &errnum, route_cb, "foo");
  ASSERT(sg_route_segments_iter(route, NULL, "foo") == EINVAL);

  route->path = "/foo/bar";
  route->rc = pcre2_match(route->re, (PCRE2_SPTR) route->path,
                          strlen(route->path), 0, 0, route->match, NULL);
  ASSERT(sg_route_segments_iter(route, route_segments_123_iter_cb, NULL) ==
         123);

  memset(str, 0, sizeof(str));
  ASSERT(strcmp(str, "") == 0);
  ASSERT(sg_route_segments_iter(route, route_segments_concat_iter_cb, str) ==
         0);
  ASSERT(strcmp(str, "0foo1bar") == 0);

  sg__route_free(route);
}

static void test_route_vars_iter(void) {
  struct sg_route *route;
  char err[SG_ERR_SIZE];
  char str[100];
  int errnum;
  ASSERT(sg_route_vars_iter(NULL, route_vars_empty_iter_cb, "foo") == EINVAL);
  route = sg__route_new("/(?<var1>[a-z]+)/(?<var2>[0-9]+)", err, sizeof(err),
                        &errnum, route_cb, "foo");
  ASSERT(sg_route_vars_iter(route, NULL, "foo") == EINVAL);

  route->path = "/abc/123";
  route->rc = pcre2_match(route->re, (PCRE2_SPTR) route->path,
                          strlen(route->path), 0, 0, route->match, NULL);
  ASSERT(sg_route_vars_iter(route, route_vars_123_iter_cb, NULL) == 123);

  memset(str, 0, sizeof(str));
  ASSERT(strcmp(str, "") == 0);
  ASSERT(sg_route_vars_iter(route, route_vars_concat_iter_cb, str) == 0);
  ASSERT(strcmp(str, "var1abcvar2123") == 0);

  sg__route_free(route);
}

static void test_route_user_data(void) {
  struct sg_route route;
  errno = 0;
  ASSERT(!sg_route_user_data(NULL));
  ASSERT(errno == EINVAL);

  route.user_data = NULL;
  errno = 0;
  ASSERT(!sg_route_user_data(&route));
  ASSERT(errno == 0);

  route.user_data = "foo";
  errno = 0;
  ASSERT(strcmp(sg_route_user_data(&route), "foo") == 0);
  ASSERT(errno == 0);
}

static void test_routes_add2(void) {
  struct sg_route *routes = NULL;
  struct sg_route *route;
  char err[SG_ERR_SIZE];
  ASSERT(sg_routes_add2(NULL, &route, "/foo", err, sizeof(err), route_cb,
                        "foo") == EINVAL);
  ASSERT(sg_routes_add2(&routes, NULL, "/foo", err, sizeof(err), route_cb,
                        "foo") == EINVAL);
  ASSERT(sg_routes_add2(&routes, &route, NULL, err, sizeof(err), route_cb,
                        "foo") == EINVAL);
  ASSERT(sg_routes_add2(&routes, &route, "/foo", NULL, sizeof(err), route_cb,
                        "foo") == EINVAL);
  ASSERT(sg_routes_add2(&routes, &route, "/foo", err, 0, route_cb, "foo") ==
         EINVAL);
  ASSERT(sg_routes_add2(&routes, &route, "/foo", err, sizeof(err), NULL,
                        "foo") == EINVAL);
  ASSERT(sg_routes_add2(&routes, &route, "/foo\\K/bar", err, sizeof(err),
                        route_cb, "foo") == EINVAL);

  ASSERT(sg_routes_add2(&routes, &route, "foo", err, sizeof(err), route_cb,
                        "foo") == 0);
  ASSERT(sg_routes_add2(&routes, &route, "foo", err, sizeof(err), route_cb,
                        "foo") == EALREADY);
  sg_routes_cleanup(&routes);

  ASSERT(sg_routes_add2(&routes, &route, "foo", err, sizeof(err), route_cb,
                        "foo") == 0);
  ASSERT(route->cb == route_cb);
  ASSERT(strcmp(route->cls, "foo") == 0);
  ASSERT(sg_routes_add2(&routes, &route, "bar", err, sizeof(err), route_cb,
                        "foo") == 0);
  ASSERT(sg_routes_add2(&routes, &route, "foobar", err, sizeof(err), route_cb,
                        "foobar") == 0);
  route = routes;
  ASSERT(strcmp(sg_route_rawpattern(route), "^foo$") == 0);
  sg_routes_next(&route);
  ASSERT(strcmp(sg_route_rawpattern(route), "^bar$") == 0);
  sg_routes_next(&route);
  ASSERT(strcmp(sg_route_rawpattern(route), "^foobar$") == 0);
  sg_routes_cleanup(&routes);
}

static void test_routes_add(void) {
  struct sg_route *routes = NULL;
  struct sg_route *route;
  errno = 0;
  ASSERT(!sg_routes_add(NULL, "/foo", route_cb, "foo"));
  ASSERT(errno == EINVAL);
  errno = 0;
  ASSERT(!sg_routes_add(&routes, NULL, route_cb, "foo"));
  ASSERT(errno == EINVAL);
  errno = 0;
  ASSERT(!sg_routes_add(&routes, "/foo", NULL, "foo"));
  ASSERT(errno == EINVAL);
  errno = 0;
  ASSERT(!sg_routes_add(&routes, "/foo\\K/bar", route_cb, "foo"));
  ASSERT(errno == EINVAL);

  ASSERT(sg_routes_add(&routes, "foo", route_cb, "foo"));
  errno = 0;
  ASSERT(!sg_routes_add(&routes, "foo", route_cb, "foo"));
  ASSERT(errno == EALREADY);
  sg_routes_cleanup(&routes);

  ASSERT(sg_routes_add(&routes, "foo", route_cb, "foo"));
  ASSERT(routes->cb == route_cb);
  ASSERT(strcmp(routes->cls, "foo") == 0);

  ASSERT(sg_routes_add(&routes, "bar", route_cb, "foo"));
  ASSERT(sg_routes_add(&routes, "foobar", route_cb, "foobar"));
  route = routes;
  ASSERT(strcmp(sg_route_rawpattern(route), "^foo$") == 0);
  sg_routes_next(&route);
  ASSERT(strcmp(sg_route_rawpattern(route), "^bar$") == 0);
  sg_routes_next(&route);
  ASSERT(strcmp(sg_route_rawpattern(route), "^foobar$") == 0);
  sg_routes_cleanup(&routes);
}

static void test_routes_rm(void) {
  struct sg_route *routes = NULL;
  struct sg_route *route;
  ASSERT(sg_routes_rm(NULL, "foo") == EINVAL);
  ASSERT(sg_routes_rm(&routes, NULL) == EINVAL);

  ASSERT(sg_routes_rm(&routes, "foo") == ENOENT);

  ASSERT(sg_routes_add(&routes, "foo", route_cb, "foo"));
  ASSERT(sg_routes_add(&routes, "bar", route_cb, "bar"));
  ASSERT(sg_routes_count(routes) == 2);
  route = routes;
  ASSERT(strcmp(sg_route_rawpattern(route), "^foo$") == 0);
  sg_routes_next(&route);
  ASSERT(strcmp(sg_route_rawpattern(route), "^bar$") == 0);
  ASSERT(sg_routes_rm(&routes, "foo") == 0);
  ASSERT(sg_routes_count(routes) == 1);
  ASSERT(strcmp(sg_route_rawpattern(route), "^bar$") == 0);
  ASSERT(sg_routes_rm(&routes, "bar") == 0);
  ASSERT(sg_routes_count(routes) == 0);
  ASSERT(sg_routes_rm(&routes, "bar") == ENOENT);
  sg_routes_cleanup(&routes);
}

static void test_routes_iter(void) {
  struct sg_route *routes = NULL;
  char str[100];
  ASSERT(sg_routes_iter(NULL, routes_iter_empty_cb, "foo") == 0);
  ASSERT(sg_routes_iter(routes, NULL, "foo") == EINVAL);

  ASSERT(sg_routes_add(&routes, "foo", route_cb, "foo"));
  ASSERT(sg_routes_add(&routes, "bar", route_cb, "bar"));
  ASSERT(sg_routes_count(routes) == 2);

  ASSERT(sg_routes_iter(routes, routes_iter_123_cb, "foo") == 123);
  memset(str, 0, sizeof(str));
  ASSERT(sg_routes_iter(routes, routes_iter_concat_cb, str) == 0);
  ASSERT(strcmp(str, "^foo$^bar$") == 0);
  sg_routes_cleanup(&routes);
}

static void test_routes_next(void) {
  struct sg_route *routes = NULL;
  struct sg_route *route;
  ASSERT(sg_routes_next(NULL) == EINVAL);
  ASSERT(sg_routes_add(&routes, "foo", route_cb, "foo"));
  ASSERT(sg_routes_add(&routes, "bar", route_cb, "bar"));
  ASSERT(sg_routes_count(routes) == 2);
  route = routes;
  ASSERT(strcmp(sg_route_rawpattern(route), "^foo$") == 0);
  ASSERT(sg_routes_next(&route) == 0);
  ASSERT(strcmp(sg_route_rawpattern(route), "^bar$") == 0);
  ASSERT(sg_routes_next(&route) == 0);
  ASSERT(!route);
  sg_routes_cleanup(&routes);
}

static void test_routes_count(void) {
  struct sg_route *routes = NULL;
  ASSERT(sg_routes_count(NULL) == 0);
  ASSERT(sg_routes_add(&routes, "foo", route_cb, "foo"));
  ASSERT(sg_routes_add(&routes, "bar", route_cb, "bar"));
  ASSERT(sg_routes_count(routes) == 2);
  sg_routes_cleanup(&routes);
}

static void test_routes_cleanup(void) {
  struct sg_route *routes = NULL;
  ASSERT(sg_routes_cleanup(NULL) == EINVAL);
  ASSERT(sg_routes_cleanup(&routes) == 0);
  ASSERT(!routes);
  ASSERT(sg_routes_add(&routes, "foo", route_cb, "foo"));
  ASSERT(sg_routes_add(&routes, "bar", route_cb, "bar"));
  ASSERT(sg_routes_count(routes) == 2);
  ASSERT(sg_routes_cleanup(&routes) == 0);
  ASSERT(!routes);
  ASSERT(sg_routes_cleanup(&routes) == 0);
}

int main(void) {
  test__route_new();
  test_route_handle();
  test_route_match();
  test_route_rawpattern();
  test_route_pattern();
  test_route_path();
  test_route_segments_iter();
  test_route_vars_iter();
  test_route_user_data();
  test_routes_add2();
  test_routes_add();
  test_routes_rm();
  test_routes_iter();
  test_routes_next();
  test_routes_count();
  test_routes_cleanup();
  return EXIT_SUCCESS;
}
