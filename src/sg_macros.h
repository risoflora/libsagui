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

#ifndef SG_MACROS_H
#define SG_MACROS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef NDEBUG
#include <unistd.h>
#include <errno.h>
#endif /* NDEBUG */
#include "sagui.h"

/* used by utstring library */
#ifndef utstring_oom
#define utstring_oom() ((void) 0)
#endif /* utstring_oom */

/* used by uthash library */
#ifndef HASH_NONFATAL_OOM
#define HASH_NONFATAL_OOM 1
#endif /* HASH_NONFATAL_OOM */
#ifndef uthash_malloc
#define uthash_malloc sg_malloc
#endif /* uthash_malloc */
#ifndef uthash_free
#define uthash_free(p, sz) sg_free((p))
#endif /* uthash_free */

/* used bt pcre2 library */
#ifdef SG_PATH_ROUTING
#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif /* PCRE2_CODE_UNIT_WIDTH */
#endif /* SG_PATH_ROUTING */

/* used by expr library */
#ifdef SG_MATH_EXPR_EVAL
#define expr_alloc sg_alloc
#define expr_realloc sg_realloc
#define expr_free sg_free
#define expr_pow sg__pow
#define expr_fmod sg__fmod
#endif /* SG_MATH_EXPR_EVAL */

#ifndef SG__EXTERN
#if defined(_WIN32) && defined(BUILD_TESTING)
#define SG__EXTERN __declspec(dllexport) extern
#else /* SG__EXTERN */
#define SG__EXTERN
#endif /* _WIN32 && BUILD_TESTING */
#endif /* SG__EXTERN */

/* macro to make it easy to mark text for translation */
#define _(String) (String)

/* stringify the result of expansion of a macro argument */
#define xstr(a) str(a)
#define str(a) #a

#ifdef _WIN32
#define PATH_SEP '\\'
#else /* _WIN32 */
#define PATH_SEP '/'
#endif /* _WIN32 */

#ifndef SG__BLOCK_SIZE
#ifdef _WIN32
#define SG__BLOCK_SIZE 16384 /* 16k */
#else /* _WIN32 */
#define SG__BLOCK_SIZE 4096 /* 4k */
#endif /* _WIN32 */
#endif /* SG__BLOCK_SIZE */

#ifndef sg__off_t
#ifdef _WIN64
#define sg__off_t off64_t
#else /* _WIN64 */
#define sg__off_t off_t
#endif /* _WIN64 */
#endif /* sg__off_t */

#ifndef sg__lseek
#ifdef _WIN32
#ifdef _WIN64
#define sg__lseek _lseeki64
#else /* _WIN64 */
#define sg__lseek _lseek
#endif /* _WIN64 */
#else /* _WIN32 */
#define sg__lseek lseek
#endif /* _WIN32 */
#endif /* sg__lseek */

#ifndef SG__ZLIB_CHUNK
#define SG__ZLIB_CHUNK 16384 /* 16k */
#endif /* SG__ZLIB_CHUNK */

#endif /* SG_MACROS_H */
