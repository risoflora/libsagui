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

#ifndef SG_EXTRA_H
#define SG_EXTRA_H

#include "sg_macros.h"
#ifdef SG_HTTP_COMPRESSION
#include "zlib.h"
#endif
#include "microhttpd.h"
#include "sg_strmap.h"
#include "sagui.h"

SG__EXTERN int sg__convals_iter(void *cls, __SG_UNUSED enum MHD_ValueKind kind, const char *key, const char *val);

SG__EXTERN int sg__strmap_iter(void *cls, struct sg_strmap *map);

#ifdef SG_HTTP_COMPRESSION

SG__EXTERN int sg__compress(const Bytef *src, uLong src_size, Bytef *dest, uLongf *dest_size, int level);

SG__EXTERN int sg__deflate(z_stream *stream, const void *src, size_t src_size, void **dest, size_t *dest_size,
                           void *tmp);

SG__EXTERN int sg__gzdeflate(z_stream *stream, const void *src, size_t src_size, void **dest, size_t *dest_size,
                             void *tmp);

#endif

#endif /* SG_EXTRA_H */
