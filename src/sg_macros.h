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

#ifndef SG_MACROS_H
#define SG_MACROS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef NDEBUG
#include <unistd.h>
#include <errno.h>
#endif

#ifdef SG_PATH_ROUTING
# ifndef PCRE2_CODE_UNIT_WIDTH
#  define PCRE2_CODE_UNIT_WIDTH 8
# endif
#include <pcre2.h>
#endif

#ifndef SG__EXTERN
# if defined(_WIN32) && defined(BUILD_TESTING)
#  define SG__EXTERN __declspec(dllexport) extern
# else
#  define SG__EXTERN
# endif
#endif

#define _(String) (String) /* macro to make it easy to mark text for translation */

#define xstr(a) str(a) /* stringify the result of expansion of a macro argument */
#define str(a) #a

#ifdef _WIN32
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

/* macro used for handling `Out of memory` errors. */
#ifndef oom
#ifdef NDEBUG
#define oom() exit(EXIT_FAILURE)
#else
#define oom()                                                                                 \
do {                                                                                          \
    if (isatty(fileno(stderr)) && (fprintf(stderr, _("%s:%d: %s: Out of memory.\n"),          \
                                        __FILE__, __LINE__, __extension__ __FUNCTION__) > 0)) \
        fflush(stderr);                                                                       \
    exit(EXIT_FAILURE);                                                                       \
} while (0)
#endif
#endif

#ifndef sg__malloc
#define sg__malloc(size) malloc((size))
#endif

#ifndef sg__alloc
#define sg__alloc(ptr, size)           \
do {                                   \
    if (!((ptr) = sg__malloc((size)))) \
        oom();                         \
    memset((ptr), 0, (size));          \
} while (0)
#endif

#ifndef sg__new
#define sg__new(ptr) sg__alloc((ptr), sizeof(*(ptr)))
#endif

#ifndef sg__realloc
#define sg__realloc(ptr, size) realloc((ptr), (size))
#endif

#ifndef sg__free
#define sg__free(ptr) free((ptr))
#endif

#endif /* SG_MACROS_H */
