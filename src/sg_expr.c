/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2021 Silvio Clecio <silvioprog@gmail.com>
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

#include <errno.h>
#include "sg_macros.h"
#include "sg_utils.h"
#include "sg_expr.h"
#include "sagui.h"

void sg__expr_clear(struct sg_expr *expr) {
  expr_destroy(expr->handle, expr->vars);
  expr->vars->head = NULL;
  sg_free(expr->funcs);
  expr->funcs = NULL;
  expr->handle = NULL;
  expr->near = 0;
}

expr_num_t sg__expr_func(__SG_UNUSED struct expr_func *func, vec_expr_t *args,
                         void *context) {
  struct sg_expr_argument func_args;
  struct sg_expr_extension *extension = context;
  func_args.handle = args;
  return extension->func(extension->cls, (args->len > 0) ? &func_args : NULL,
                         extension->identifier);
}

struct sg_expr *sg_expr_new(void) {
  struct sg_expr *expr = sg_alloc(sizeof(struct sg_expr));
  if (expr) {
    expr->vars = sg_alloc(sizeof(struct expr_var_list));
    if (expr->vars)
      return expr;
  }
  sg_expr_free(expr);
  errno = ENOMEM;
  return NULL;
}

void sg_expr_free(struct sg_expr *expr) {
  if (!expr)
    return;
  sg__expr_clear(expr);
  sg_free(expr->vars);
  sg_free(expr);
}

int sg_expr_compile(struct sg_expr *expr, const char *str, size_t len,
                    struct sg_expr_extension *extensions) {
  struct expr_func *funcs, *func;
  struct sg_expr_extension *extension;
  int count;
  if (!expr || !str)
    return EINVAL;
  if (expr->handle)
    return EALREADY;
  count = 0;
  while (extensions) {
    funcs = sg_realloc(expr->funcs, (count + 1) * sizeof(struct expr_func));
    if (!funcs) {
      sg_free(expr->funcs);
      expr->funcs = NULL;
      return ENOMEM;
    }
    expr->funcs = funcs;
    func = *&expr->funcs + count;
    memset(func, 0, sizeof(struct expr_func));
    func->f = sg__expr_func;
    extension = *&extensions + count;
    if (!extension->func || !extension->identifier)
      break;
    func->name = extension->identifier;
    func->context = extension;
    count++;
  }
  expr->handle =
    expr_create2(str, len, expr->vars, expr->funcs, &expr->near, &expr->err);
  if (!expr->handle) {
    sg_free(expr->funcs);
    expr->funcs = NULL;
    return EINVAL;
  }
  return 0;
}

int sg_expr_clear(struct sg_expr *expr) {
  if (!expr)
    return EINVAL;
  sg__expr_clear(expr);
  return 0;
}

double sg_expr_eval(struct sg_expr *expr) {
  if (!expr || !expr->handle) {
    errno = EINVAL;
    return NAN;
  }
  return expr_eval(expr->handle);
}

double sg_expr_var(struct sg_expr *expr, const char *name, size_t len) {
  struct expr_var *var;
  if (expr && name && len > 0) {
    var = expr_var(expr->vars, name, len);
    if (var)
      return var->value;
    errno = ENOMEM;
  } else
    errno = EINVAL;
  return NAN;
}

int sg_expr_set_var(struct sg_expr *expr, const char *name, size_t len,
                    double val) {
  struct expr_var *var;
  if (!expr || !name || len <= 0)
    return EINVAL;
  var = expr_var(expr->vars, name, len);
  if (!var)
    return ENOMEM;
  var->value = val;
  return 0;
}

double sg_expr_arg(struct sg_expr_argument *args, int index) {
  if (args && index > -1)
    return expr_eval(&vec_nth(args->handle, index));
  errno = EINVAL;
  return NAN;
}

int sg_expr_near(struct sg_expr *expr) {
  if (expr)
    return expr->near;
  errno = EINVAL;
  return 0;
}

enum sg_expr_err_type sg_expr_err(struct sg_expr *expr) {
  if (expr)
    switch (expr->err) {
      case EXPR_ERR_UNKNOWN:
        return SG_EXPR_ERR_UNKNOWN;
      case EXPR_ERR_UNEXPECTED_NUMBER:
        return SG_EXPR_ERR_UNEXPECTED_NUMBER;
      case EXPR_ERR_UNEXPECTED_WORD:
        return SG_EXPR_ERR_UNEXPECTED_WORD;
      case EXPR_ERR_UNEXPECTED_PARENS:
        return SG_EXPR_ERR_UNEXPECTED_PARENS;
      case EXPR_ERR_MISS_EXPECTED_OPERAND:
        return SG_EXPR_ERR_MISSING_OPERAND;
      case EXPR_ERR_UNKNOWN_OPERATOR:
        return SG_EXPR_ERR_UNKNOWN_OPERATOR;
      case EXPR_ERR_INVALID_FUNC_NAME:
        return SG_EXPR_ERR_INVALID_FUNC_NAME;
      case EXPR_ERR_BAD_PARENS:
        return SG_EXPR_ERR_BAD_PARENS;
      case EXPR_ERR_TOO_FEW_FUNC_ARGS:
        return SG_EXPR_ERR_TOO_FEW_FUNC_ARGS;
      case EXPR_ERR_FIRST_ARG_IS_NOT_VAR:
        return SG_EXPR_ERR_FIRST_ARG_IS_NOT_VAR;
      case EXPR_ERR_BAD_VARIABLE_NAME:
        return SG_EXPR_ERR_BAD_VARIABLE_NAME;
      case EXPR_ERR_BAD_ASSIGNMENT:
        return SG_EXPR_ERR_BAD_ASSIGNMENT;
      default:
        break;
    }
  errno = EINVAL;
  return SG_EXPR_ERR_UNKNOWN;
}

const char *sg_expr_strerror(struct sg_expr *expr) {
  if (expr)
    switch (expr->err) {
      case EXPR_ERR_UNKNOWN:
        return _("Unknown.\n");
      case EXPR_ERR_UNEXPECTED_NUMBER:
        return _("Unexpected number.\n");
      case EXPR_ERR_UNEXPECTED_WORD:
        return _("Unexpected word.\n");
      case EXPR_ERR_UNEXPECTED_PARENS:
        return _("Unexpected parenthesis.\n");
      case EXPR_ERR_MISS_EXPECTED_OPERAND:
        return _("Missing expected operand.\n");
      case EXPR_ERR_UNKNOWN_OPERATOR:
        return _("Unknown operator.\n");
      case EXPR_ERR_INVALID_FUNC_NAME:
        return _("Invalid function name.\n");
      case EXPR_ERR_BAD_CALL:
        return _("Bad call.\n");
      case EXPR_ERR_BAD_PARENS:
        return _("Bad parenthesis.\n");
      case EXPR_ERR_TOO_FEW_FUNC_ARGS:
        return _("Too few function arguments.\n");
      case EXPR_ERR_FIRST_ARG_IS_NOT_VAR:
        return _("First argument is not a variable.\n");
      case EXPR_ERR_BAD_VARIABLE_NAME:
        return _("Bad variable name.\n");
      case EXPR_ERR_BAD_ASSIGNMENT:
        return _("Bad assignment.\n");
      default:
        break;
    }
  errno = EINVAL;
  return NULL;
}

double sg_expr_calc(const char *str, size_t len) {
  double ret = NAN;
  int near, error = -1;
  if (str && len > 0)
    ret = expr_calc2(str, len, &near, &error);
  if (error != 0)
    errno = EINVAL;
  return ret;
}
