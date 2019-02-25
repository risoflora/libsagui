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

#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "sg_macros.h"
#ifdef SG_HTTP_COMPRESSION
#include "zlib.h"
#endif
#include "microhttpd.h"
#include "sg_extra.h"
#include "sg_strmap.h"
#include "sagui.h"

int sg__convals_iter(void *cls, __SG_UNUSED enum MHD_ValueKind kind, const char *key, const char *val) {
    sg_strmap_add(cls, key, val);
    return MHD_YES;
}

int sg__strmap_iter(void *cls, struct sg_strmap *header) {
    MHD_add_response_header(cls, header->name, header->val);
    return 0;
}

ssize_t sg_eor(bool err) {
    return
#ifdef __ANDROID__
        (ssize_t)
#endif
            err ? MHD_CONTENT_READER_END_WITH_ERROR : MHD_CONTENT_READER_END_OF_STREAM;
}

#ifdef SG_HTTP_COMPRESSION

int sg__compress(const Bytef *src, uLong src_size, Bytef *dest, uLongf *dest_size, int level) {
    const uInt max = (uInt) -1;
    z_stream stream;
    uLong left;
    int errnum;
    left = *dest_size;
    *dest_size = 0;
    stream.zalloc = NULL;
    stream.zfree = NULL;
    stream.opaque = NULL;
    errnum = deflateInit2(&stream, level, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if (errnum != Z_OK)
        return errnum;
    stream.next_out = dest;
    stream.avail_out = 0;
    stream.next_in = (z_const Bytef *) src;
    stream.avail_in = 0;
    do {
        if (stream.avail_out == 0) {
            stream.avail_out = left > (uLong) max ? max : (uInt) left;
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0) {
            stream.avail_in = src_size > (uLong) max ? max : (uInt) src_size;
            src_size -= stream.avail_in;
        }
        errnum = deflate(&stream, src_size ? Z_NO_FLUSH : Z_FINISH);
    } while (errnum == Z_OK);
    *dest_size = stream.total_out;
    deflateEnd(&stream);
    return errnum == Z_STREAM_END ? Z_OK : errnum;
}

int sg__deflate(z_stream *stream, const void *src, size_t src_size, void **dest, size_t *dest_size, void *tmp) {
    unsigned int have;
    int flush, ret;
    *dest = NULL;
    *dest_size = 0;
    do {
        if (src_size > SG__ZLIB_CHUNK) {
            stream->avail_in = SG__ZLIB_CHUNK;
            src_size -= SG__ZLIB_CHUNK;
            flush = Z_NO_FLUSH;
        } else {
            stream->avail_in = (uInt) src_size;
            flush = Z_SYNC_FLUSH;
        }
        stream->next_in = (Bytef *) src;
        do {
            stream->avail_out = SG__ZLIB_CHUNK;
            stream->next_out = tmp;
            ret = deflate(stream, flush);
            have = SG__ZLIB_CHUNK - stream->avail_out;
            *dest_size += have;
            *dest = sg_realloc(*dest, *dest_size);
            if (!*dest)
                return ENOMEM;
            memcpy((Bytef *) *dest + (*dest_size - have), tmp, have);
        } while (stream->avail_out == 0);
    } while (flush != Z_SYNC_FLUSH);
    return (ret == Z_OK) ? 0 : ret;
}

int sg__gzdeflate(z_stream *stream, Bytef *zbuf, int flush, z_const Bytef *src, uInt src_size,
                  Bytef **dest, z_size_t *dest_size) {
    unsigned int have;
    int errnum;
    stream->avail_in = src_size;
    stream->next_in = src;
    *dest = Z_NULL;
    *dest_size = 0;
    do {
        stream->avail_out = SG__ZLIB_CHUNK;
        stream->next_out = zbuf;
        errnum = deflate(stream, flush);
        if (errnum < Z_OK) {
            sg_free(*dest);
            *dest = Z_NULL;
            return errnum;
        }
        have = SG__ZLIB_CHUNK - stream->avail_out;
        *dest_size += have;
        *dest = sg_realloc(*dest, *dest_size);
        if (!*dest)
            return ENOMEM;
        memcpy(*dest + (*dest_size - have), zbuf, have);
    } while (stream->avail_out == 0);
    return 0;
}

#endif
