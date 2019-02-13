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

#ifndef SG_UTILS_H
#define SG_UTILS_H

#ifndef _WIN32
#include <stdlib.h>
#endif
#include <stddef.h>
#include <stdbool.h>
#include "sg_macros.h"
#include "sagui.h"

#ifdef _WIN32
#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif

#define realpath(n, r) _fullpath((r), (n), PATH_MAX)

SG__EXTERN char *strndup(const char *s, size_t n);

SG__EXTERN int sg__rename(const char *old, const char *new);

#else
#define sg__rename rename
#endif

#if defined(_WIN32) || defined(__ANDROID__)
char *basename(const char *path);
#endif

SG__EXTERN char *sg__strdup(const char *str);

/* Converts US-ASCII string to lower case. */
SG__EXTERN void sg__toasciilower(char *str);

SG__EXTERN char *sg__strjoin(char sep, const char *a, const char *b);

SG__EXTERN bool sg__is_cookie_name(const char *name);

SG__EXTERN bool sg__is_cookie_val(const char *val);

SG__EXTERN void sg__err_cb(__SG_UNUSED void *cls, const char *err);

#endif /* SG_UTILS_H */
