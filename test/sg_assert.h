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

#ifndef SG_ASSERT_H
#define SG_ASSERT_H

#ifndef _WIN32
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#ifdef _WIN32
#define __progname __argv[0]
#else
#if defined(__USE_GNU) && !defined(__i386__) && !defined(__ANDROID__)
#define __progname program_invocation_short_name
#else
extern const char *__progname;
#endif
#endif

#ifdef NDEBUG
#define ASSERT(expr) ((void) 0)
#else
#define ASSERT(expr)                                                                        \
do {                                                                                        \
    if (!(expr)) {                                                                          \
        if ((fprintf(stderr, "%s: %s:%d: %s: Assertion `%s' failed.\n",                     \
                __progname, __FILE__, __LINE__, __extension__ __FUNCTION__, #expr) > 0))    \
            fflush(stderr);                                                                 \
        exit(EXIT_FAILURE);                                                                 \
    }                                                                                       \
} while (0)
#endif

#endif /* SG_ASSERT_H */
