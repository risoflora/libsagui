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

#include "sg_assert.h"

#include <string.h>
#include <microhttpd.h>
#ifdef SG_HTTP_COMPRESSION
#include "zlib.h"
#endif
#include "sg_macros.h"
#include "sg_strmap.h"
#include "sg_extra.h"
#include <sagui.h>

static void test__convals_iter(void) {
    struct sg_strmap *map = NULL;
    ASSERT(sg__convals_iter(NULL, MHD_HEADER_KIND, "foo", "bar") == MHD_YES);
    ASSERT(!map);
    ASSERT(sg__convals_iter(&map, MHD_HEADER_KIND, "foo", "bar") == MHD_YES);
    ASSERT(map);
    ASSERT(strcmp(sg_strmap_get(map, "foo"), "bar") == 0);
    sg_strmap_cleanup(&map);
}

static void test__strmap_iter(void) {
    struct sg_strmap *header = sg_alloc(sizeof(struct sg_strmap));
    struct MHD_Response *res = sg_alloc(64);
    header->name = "";
    header->val = "";
    ASSERT(sg__strmap_iter(res, header) == 0);
    sg_free(header);
    sg_free(res);
}

static void test_eor(void) {
    ASSERT(sg_eor(false) == (ssize_t) MHD_CONTENT_READER_END_OF_STREAM);
    ASSERT(sg_eor(true) == (ssize_t) MHD_CONTENT_READER_END_WITH_ERROR);
}

#ifdef SG_HTTP_COMPRESSION

static int sg__uncompress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong *sourceLen) {
    z_stream stream;
    const uInt max = (uInt) -1;
    uLong len, left;
    int err;
    Byte buf[1];
    len = *sourceLen;
    if (*destLen) {
        left = *destLen;
        *destLen = 0;
    } else {
        left = 1;
        dest = buf;
    }
    stream.next_in = (z_const Bytef *) source;
    stream.avail_in = 0;
    stream.zalloc = (alloc_func) 0;
    stream.zfree = (free_func) 0;
    stream.opaque = (voidpf) 0;
    err = inflateInit2(&stream, -MAX_WBITS);
    if (err != Z_OK)
        return err;
    stream.next_out = dest;
    stream.avail_out = 0;
    do {
        if (stream.avail_out == 0) {
            stream.avail_out = left > (uLong) max ? max : (uInt) left;
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0) {
            stream.avail_in = len > (uLong) max ? max : (uInt) len;
            len -= stream.avail_in;
        }
        err = inflate(&stream, Z_NO_FLUSH);
    } while (err == Z_OK);
    *sourceLen -= len + stream.avail_in;
    if (dest != buf)
        *destLen = stream.total_out;
    else if (stream.total_out && err == Z_BUF_ERROR)
        left = 1;
    inflateEnd(&stream);
    return err == Z_STREAM_END ? Z_OK :
           err == Z_NEED_DICT ? Z_DATA_ERROR :
           err == Z_BUF_ERROR && (left + stream.avail_out) != 0 ? Z_DATA_ERROR :
           err;
}

static void test__zcompress(void) {
    const char *text = "ffffffffffoooooooooobbbbbbbbbbaaaaaaaaaarrrrrrrrrr";
    char src[100], dest[100];
    size_t src_size, dest_size;
    sprintf(src, "%s", text);
    src_size = strlen(src);
    dest_size = compressBound(src_size);
    ASSERT(sg__zcompress(NULL, src_size, (Bytef *) dest, (uLong *) &dest_size, -10) != Z_OK);
    dest_size = compressBound(src_size);
    ASSERT(sg__zcompress((Bytef *) src, src_size, (Bytef *) dest, (uLong *) &dest_size, 9) == Z_OK);
    ASSERT(dest_size == 15);
    memset(src, 0, sizeof(src));
    memcpy(src, dest, dest_size);
    memset(dest, 0, sizeof(dest));
    src_size = dest_size;
    dest_size = sizeof(dest);
    ASSERT(sg__uncompress2((Bytef *) dest, (uLongf *) &dest_size, (Bytef *) src, (uLong *) &src_size) == Z_OK);
    ASSERT(dest_size == 50);
    dest[dest_size] = '\0';
    ASSERT(strcmp(dest, text) == 0);
}

static void test__zdeflate(void) {
    const char *text = "ffffffffffoooooooooobbbbbbbbbbaaaaaaaaaarrrrrrrrrr";
    z_stream stream;
    char src[100], zbuf[SG__ZLIB_CHUNK];
    char *dest;
    size_t src_size, dest_size;
    memset(&stream, 0, sizeof(z_stream));
    ASSERT(deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL,
                        Z_DEFAULT_STRATEGY) == Z_OK);
    sprintf(src, "%s", text);
    src_size = strlen(src);
    ASSERT(sg__zdeflate(&stream, (Bytef *) zbuf, Z_FINISH, (Bytef *) src, (uInt) src_size,
                        (Bytef **) &dest, &dest_size) == Z_OK);
    ASSERT(deflateEnd(&stream) == Z_OK);
    ASSERT(dest_size == 15);
    memset(src, 0, sizeof(src));
    memcpy(src, dest, dest_size);
    free(dest);
    dest = malloc(100);
    src_size = dest_size;
    dest_size = 100;
    ASSERT(sg__uncompress2((Bytef *) dest, (uLongf *) &dest_size, (Bytef *) src, (uLong *) &src_size) == Z_OK);
    ASSERT(dest_size == 50);
    if (dest) {
        dest[dest_size] = '\0';
        ASSERT(strcmp(dest, text) == 0);
    }
    free(dest);


    memset(&stream, 0, sizeof(z_stream));
    ASSERT(deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) == Z_OK);
    sprintf(src, "%s", text);
    src_size = strlen(src);
    ASSERT(sg__zdeflate(&stream, (Bytef *) zbuf, Z_NO_FLUSH, (Bytef *) src, (uInt) src_size,
                        (Bytef **) &dest, &dest_size) == Z_OK);
    free(dest);
    ASSERT(dest_size == 0);
    ASSERT(sg__zdeflate(&stream, (Bytef *) zbuf, Z_FINISH, (Bytef *) src, 0, (Bytef **) &dest, &dest_size) == Z_OK);
    ASSERT(deflateEnd(&stream) == Z_OK);
    ASSERT(dest_size == 15);
    memset(src, 0, sizeof(src));
    memcpy(src, dest, dest_size);
    free(dest);
    dest = malloc(100);
    src_size = dest_size;
    dest_size = 100;
    ASSERT(sg__uncompress2((Bytef *) dest, (uLongf *) &dest_size, (Bytef *) src, (uLong *) &src_size) == Z_OK);
    ASSERT(dest_size == 50);
    if (dest) {
        dest[dest_size] = '\0';
        ASSERT(strcmp(dest, text) == 0);
    }
    free(dest);
}

#endif

int main(void) {
    test__convals_iter();
    test__strmap_iter();
    test_eor();
#ifdef SG_HTTP_COMPRESSION
    test__zcompress();
    test__zdeflate();
#endif
    return EXIT_SUCCESS;
}
