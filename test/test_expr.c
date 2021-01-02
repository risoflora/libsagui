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

#include <string.h>

#include "sg_macros.h"
#include "sg_assert.h"
#include "sg_utils.h"
#include "sg_expr.h"
#include "sg_expr.c"
#include <sagui.h>

#define EXPR_1 "test_mul(2, 3)"
#define EXPR_2 "2 + 3"
#define EXPR_3 "test_missing(2, 3)"
#define EXPR_4 "foo=2, bar=3"
#define EXPR_5 "abc)def"

static double test__mul(void *cls, struct sg_expr_argument *args,
                        const char *identifier) {
  if (!args)
    return NAN;
  ASSERT(strcmp(identifier, "test_mul") == 0);
  ASSERT(strcmp(cls, EXPR_1) == 0);
  return sg_expr_arg(args, 0) * sg_expr_arg(args, 1);
}

struct sg_expr_extension extensions[] = {
  {.func = test__mul, .identifier = "test_mul", .cls = EXPR_1},
  {.func = NULL, .identifier = NULL, .cls = NULL},
};

static void test__expr_clear(struct sg_expr *expr) {
  sg_expr_compile(expr, EXPR_1, strlen(EXPR_1), extensions);
  ASSERT(expr->handle);
  ASSERT(expr->funcs);
  ASSERT(expr->handle);
  sg__expr_clear(expr);
  ASSERT(!expr->handle);
  ASSERT(!expr->funcs);
  ASSERT(!expr->handle);
}

static void test__expr_func(void) {
  vec_expr_t args = vec_init();
  ASSERT(isnan(sg__expr_func(NULL, &args, extensions)));
  vec_push(&args, expr_const(2));
  vec_push(&args, expr_const(3));
  ASSERT(sg__expr_func(NULL, &args, extensions) == 6);
  vec_free(&args);
}

static void test_expr_new(void) {
  struct sg_expr *expr = sg_expr_new();
  ASSERT(expr);
  ASSERT(expr->vars);
  sg_expr_free(expr);
}

static void test_expr_free(void) {
  sg_expr_free(NULL);
}

static void test_expr_compile(struct sg_expr *expr) {
  ASSERT(sg_expr_compile(NULL, EXPR_1, strlen(EXPR_1), extensions) == EINVAL);
  ASSERT(sg_expr_compile(expr, NULL, strlen(EXPR_1), extensions) == EINVAL);
  ASSERT(sg_expr_compile(expr, EXPR_1, strlen(EXPR_1), NULL) == EINVAL);
  ASSERT(sg_expr_compile(expr, EXPR_3, strlen(EXPR_3), extensions) == EINVAL);

  ASSERT(sg_expr_compile(expr, EXPR_1, 0, extensions) == 0);
  sg_expr_clear(expr);
  ASSERT(sg_expr_compile(expr, EXPR_2, strlen(EXPR_2), NULL) == 0);
  ASSERT(sg_expr_compile(expr, EXPR_1, strlen(EXPR_1), extensions) == EALREADY);
  sg_expr_clear(expr);
  ASSERT(sg_expr_compile(expr, EXPR_1, strlen(EXPR_1), extensions) == 0);
  sg_expr_clear(expr);
  ASSERT(sg_expr_compile(expr, "", 0, extensions) == 0);
}

static void test_expr_clear(struct sg_expr *expr) {
  errno = 0;
  sg_expr_clear(NULL);
  ASSERT(errno == EINVAL);
  sg_expr_clear(expr);
  ASSERT(sg_expr_compile(expr, EXPR_1, strlen(EXPR_1), extensions) == 0);
  ASSERT(sg_expr_compile(expr, EXPR_1, strlen(EXPR_1), extensions) == EALREADY);
  sg_expr_clear(expr);
  ASSERT(sg_expr_compile(expr, EXPR_1, strlen(EXPR_1), extensions) == 0);

  sg_expr_clear(expr);
  sg_expr_clear(expr);
}

static void test_expr_eval(struct sg_expr *expr) {
  errno = 0;
  ASSERT(isnan(sg_expr_eval(NULL)));
  ASSERT(errno == EINVAL);
  sg_expr_clear(expr);
  errno = 0;
  ASSERT(isnan(sg_expr_eval(expr)));
  ASSERT(errno == EINVAL);
  ASSERT(sg_expr_compile(expr, EXPR_1, strlen(EXPR_1), extensions) == 0);
  errno = 0;
  ASSERT(!isnan(sg_expr_eval(expr)));
  ASSERT(errno == 0);
}

static void test_expr_var(struct sg_expr *expr) {
  const char *foo = "foo";
  const char *bar = "bar";
  size_t foo_len = strlen(foo);
  size_t bar_len = strlen(bar);
  errno = 0;
  ASSERT(isnan(sg_expr_var(NULL, foo, foo_len)));
  ASSERT(errno == EINVAL);
  errno = 0;
  ASSERT(isnan(sg_expr_var(expr, NULL, foo_len)));
  ASSERT(errno == EINVAL);
  errno = 0;
  ASSERT(isnan(sg_expr_var(expr, foo, 0)));
  ASSERT(errno == EINVAL);

  sg_expr_clear(expr);
  errno = 0;
  ASSERT(sg_expr_var(expr, foo, foo_len) == 0);
  ASSERT(errno == 0);
  ASSERT(sg_expr_compile(expr, EXPR_4, strlen(EXPR_4), extensions) == 0);
  sg_expr_eval(expr);
  errno = 0;
  ASSERT(sg_expr_var(expr, foo, foo_len) == 2);
  ASSERT(errno == 0);
  errno = 0;
  ASSERT(sg_expr_var(expr, bar, bar_len) == 3);
  ASSERT(errno == 0);
  sg_expr_clear(expr);
  ASSERT(sg_expr_var(expr, foo, foo_len) == 0);
  ASSERT(sg_expr_var(expr, bar, bar_len) == 0);
}

static void test_expr_set_var(struct sg_expr *expr) {
  const char *foo = "foo";
  const char *bar = "bar";
  size_t foo_len = strlen(foo);
  size_t bar_len = strlen(bar);
  ASSERT(sg_expr_set_var(NULL, foo, foo_len, 0) == EINVAL);
  ASSERT(sg_expr_set_var(expr, NULL, foo_len, 0) == EINVAL);
  ASSERT(sg_expr_set_var(expr, foo, 0, 0) == EINVAL);

  sg_expr_clear(expr);
  ASSERT(sg_expr_var(expr, foo, foo_len) == 0);
  ASSERT(sg_expr_set_var(expr, foo, foo_len, 2) == 0);
  ASSERT(sg_expr_var(expr, foo, foo_len) == 2);
  ASSERT(sg_expr_var(expr, bar, bar_len) == 0);
  ASSERT(sg_expr_set_var(expr, bar, bar_len, 3) == 0);
  ASSERT(sg_expr_var(expr, bar, bar_len) == 3);
  ASSERT(sg_expr_set_var(expr, foo, foo_len, 0) == 0);
  ASSERT(sg_expr_set_var(expr, bar, bar_len, 0) == 0);
  ASSERT(sg_expr_var(expr, foo, foo_len) == 0);
  ASSERT(sg_expr_var(expr, bar, bar_len) == 0);
}

static void test_expr_arg(void) {
  vec_expr_t args = vec_init();
  struct sg_expr_argument func_args = {.handle = &args};

  errno = 0;
  ASSERT(isnan(sg_expr_arg(NULL, 0)));
  ASSERT(errno == EINVAL);
  errno = 0;
  ASSERT(isnan(sg_expr_arg(&func_args, -1)));
  ASSERT(errno == EINVAL);

  vec_push(&args, expr_const(2));
  vec_push(&args, expr_const(3));
  errno = 0;
  ASSERT(sg_expr_arg(&func_args, 0) == 2);
  ASSERT(sg_expr_arg(&func_args, 1) == 3);
  ASSERT(isnan(sg_expr_arg(&func_args, 3)));
  ASSERT(errno == 0);

  vec_free(&args);
}

static void test_expr_near(struct sg_expr *expr) {
  errno = 0;
  ASSERT(sg_expr_near(NULL) == 0);
  ASSERT(errno == EINVAL);

  sg_expr_clear(expr);
  errno = 0;
  ASSERT(sg_expr_near(expr) == 0);
  ASSERT(errno == 0);
  ASSERT(sg_expr_compile(expr, EXPR_5, strlen(EXPR_5), extensions) == EINVAL);
  errno = 0;
  ASSERT(sg_expr_near(expr) == 4);
  ASSERT(errno == 0);
}

static void test_expr_err_type(struct sg_expr *expr) {
  errno = 0;
  ASSERT(sg_expr_err(NULL) == SG_EXPR_ERR_UNKNOWN);
  ASSERT(errno == EINVAL);
  expr->err = EXPR_ERR_UNKNOWN;

  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_UNKNOWN);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_UNEXPECTED_NUMBER;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_UNEXPECTED_NUMBER);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_UNEXPECTED_WORD;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_UNEXPECTED_WORD);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_UNEXPECTED_PARENS;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_UNEXPECTED_PARENS);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_MISS_EXPECTED_OPERAND;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_MISSING_OPERAND);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_UNKNOWN_OPERATOR;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_UNKNOWN_OPERATOR);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_INVALID_FUNC_NAME;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_INVALID_FUNC_NAME);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_BAD_PARENS;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_BAD_PARENS);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_TOO_FEW_FUNC_ARGS;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_TOO_FEW_FUNC_ARGS);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_FIRST_ARG_IS_NOT_VAR;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_FIRST_ARG_IS_NOT_VAR);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_BAD_VARIABLE_NAME;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_BAD_VARIABLE_NAME);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_BAD_ASSIGNMENT;
  errno = 0;
  ASSERT(sg_expr_err(expr) == SG_EXPR_ERR_BAD_ASSIGNMENT);
  ASSERT(errno == 0);
}

static void test_expr_strerror(struct sg_expr *expr) {
  errno = 0;
  ASSERT(!sg_expr_strerror(NULL));
  ASSERT(errno == EINVAL);
  expr->err = EXPR_ERR_UNKNOWN;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Unknown.\n")) == 0);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_UNEXPECTED_NUMBER;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Unexpected number.\n")) == 0);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_UNEXPECTED_WORD;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Unexpected word.\n")) == 0);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_UNEXPECTED_PARENS;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Unexpected parenthesis.\n")) == 0);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_MISS_EXPECTED_OPERAND;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Missing expected operand.\n")) == 0);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_UNKNOWN_OPERATOR;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Unknown operator.\n")) == 0);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_INVALID_FUNC_NAME;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Invalid function name.\n")) == 0);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_BAD_PARENS;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Bad parenthesis.\n")) == 0);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_TOO_FEW_FUNC_ARGS;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Too few function arguments.\n")) ==
         0);
  expr->err = EXPR_ERR_FIRST_ARG_IS_NOT_VAR;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr),
                _("First argument is not a variable.\n")) == 0);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_BAD_VARIABLE_NAME;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Bad variable name.\n")) == 0);
  ASSERT(errno == 0);
  expr->err = EXPR_ERR_BAD_ASSIGNMENT;
  errno = 0;
  ASSERT(strcmp(sg_expr_strerror(expr), _("Bad assignment.\n")) == 0);
  ASSERT(errno == 0);
}

static void test_expr_calc(void) {
  errno = 0;
  ASSERT(isnan(sg_expr_calc(NULL, strlen(EXPR_1))));
  ASSERT(errno == EINVAL);
  errno = 0;
  ASSERT(isnan(sg_expr_calc(EXPR_1, 0)));
  ASSERT(errno == EINVAL);
  errno = 0;
  ASSERT(isnan(sg_expr_calc(EXPR_3, 0)));
  ASSERT(errno == EINVAL);
  errno = 0;
  ASSERT(sg_expr_calc(EXPR_2, strlen(EXPR_2)) == 5);
  ASSERT(errno == 0);
}

int main(void) {
  struct sg_expr *expr = sg_expr_new();
  test__expr_clear(expr);
  test__expr_func();
  test_expr_new();
  test_expr_free();
  test_expr_compile(expr);
  test_expr_clear(expr);
  test_expr_eval(expr);
  test_expr_var(expr);
  test_expr_set_var(expr);
  test_expr_arg();
  test_expr_near(expr);
  test_expr_err_type(expr);
  test_expr_strerror(expr);
  test_expr_calc();
  sg_expr_free(expr);
  return EXIT_SUCCESS;
}
