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

#ifndef SG_UTILS_H
#define SG_UTILS_H

#ifndef _WIN32
#include <stdlib.h>
#endif /* _WIN32 */
#include <stddef.h>
#include <stdbool.h>
#include "sg_macros.h"
#include "sagui.h"

struct sg__memory_manager {
  sg_malloc_func malloc;
  sg_realloc_func realloc;
  sg_free_func free;
};

struct sg__math_manager {
  sg_pow_func pow;
  sg_fmod_func fmod;
};

#ifdef _WIN32
#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif /* PATH_MAX */

#define realpath(n, r) _fullpath((r), (n), PATH_MAX)

SG__EXTERN char *strndup(const char *s, size_t n);

SG__EXTERN int sg__rename(const char *old, const char *new);

#else /* _WIN32 */
#define sg__rename rename
#endif /* _WIN32 */

#if defined(_WIN32) || defined(__ANDROID__) ||                                 \
  (defined(__linux__) && !defined(__gnu_linux__))

SG__EXTERN char *sg__basename(const char *path);

#else /* _WIN32 || __ANDROID__ || (__linux__ && !__gnu_linux__) */

#define sg__basename basename

#endif /* _WIN32 || __ANDROID__ || (__linux__ && !__gnu_linux__) */

SG__EXTERN char *sg__strdup(const char *str);

SG__EXTERN double sg__pow(double x, double y);

SG__EXTERN double sg__fmod(double x, double y);

/* Converts US-ASCII string to lower case. */
SG__EXTERN void sg__toasciilower(char *str);

SG__EXTERN char *sg__strjoin(char sep, const char *a, const char *b);

SG__EXTERN bool sg__is_cookie_name(const char *name);

SG__EXTERN bool sg__is_cookie_val(const char *val);

SG__EXTERN void sg__err_cb(__SG_UNUSED void *cls, const char *err);

#endif /* SG_UTILS_H */
