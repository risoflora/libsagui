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

SG__EXTERN voidpf sg__zalloc(__SG_UNUSED voidpf opaque, unsigned items, unsigned size);

SG__EXTERN void sg__zfree(__SG_UNUSED voidpf opaque, voidpf ptr);

SG__EXTERN int sg__zcompress(z_const Bytef *src, uLong src_size, Bytef *dest, uLongf *dest_size, int level);

SG__EXTERN int sg__zdeflate(z_stream *stream, Bytef *zbuf, int flush, z_const Bytef *src, uInt src_size,
                            Bytef **dest, z_size_t *dest_size);

#endif

#endif /* SG_EXTRA_H */
