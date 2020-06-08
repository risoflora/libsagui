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
#include <math.h>
#include <sagui.h>

#define DUMMY_EXPR                                                             \
  "$(sum, $1 + $2), x = 2.3, y = 4.5, mul(6.7, sum(x, y)) + 0.11"

/* NOTE: Error checking has been omitted to make it clear. */

static double my_mul(__SG_UNUSED void *cls, struct sg_expr_argument *args,
                     __SG_UNUSED const char *identifier) {
  if (!args)
    return NAN;
  return sg_expr_arg(args, 0) * sg_expr_arg(args, 1);
}

int main(int argc, const char *argv[]) {
  struct sg_expr_extension extensions[] = {
    {.func = my_mul, .identifier = "mul", .cls = NULL},
    {.func = NULL, .identifier = NULL, .cls = NULL},
  };
  struct sg_expr *expr;
  const char *s = argc == 2 ? argv[1] : DUMMY_EXPR;
  int ret;
  expr = sg_expr_new();
  ret = sg_expr_compile(expr, s, strlen(s), extensions);
  if (ret == 0)
    printf("Result: %f\n", sg_expr_eval(expr));
  else {
    printf("%s: %s", s, sg_expr_strerror(expr));
    printf("%*s^--- Error near here.\n", sg_expr_near(expr), "");
  }
  sg_expr_free(expr);
  return EXIT_SUCCESS;
}
