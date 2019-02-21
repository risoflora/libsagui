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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "sg_macros.h"
#ifdef SG_HTTP_COMPRESSION
#include "zlib.h"
#endif
#include "microhttpd.h"
#include "sagui.h"
#include "sg_utils.h"
#include "sg_strmap.h"
#include "sg_extra.h"
#include "sg_httpres.h"

#ifdef SG_HTTP_COMPRESSION

static ssize_t sg__httpres_zread_cb(void *handle, __SG_UNUSED uint64_t offset, char *buf, size_t size) {
    struct sg__httpres_zholder *holder = handle;
    size_t have;
    void *dest;
    have = (size_t) holder->read_cb(holder->handle, holder->offset, buf, size);
    if ((have == MHD_CONTENT_READER_END_WITH_ERROR) || (have == MHD_CONTENT_READER_END_OF_STREAM))
        return have;
    holder->offset += have;
    if (sg__deflate(&holder->stream, buf, have, &dest, &have, holder->buf) != 0)
        have = MHD_CONTENT_READER_END_WITH_ERROR;
    else {
        memcpy(buf, dest, have);
        sg_free(dest);
    }
    return have;
}

static void sg__httpres_zfree_cb(void *handle) {
    struct sg__httpres_zholder *holder = handle;
    if (!holder)
        return;
    deflateEnd(&holder->stream);
    sg_free(holder->buf);
    if (holder->free_cb)
        holder->free_cb(holder->handle);
    sg_free(holder);
}

static ssize_t sg__httpres_zfread_cb(void *handle, __SG_UNUSED uint64_t offset, char *mem, size_t size) {
    struct sg__httpres_gzholder *holder = handle;
    size_t have;
    void *buf;
    if (holder->status == SG__HTTPRES_GZ_NONE) {
        holder->status = SG__HTTPRES_GZ_STARDED;
        memset(mem, 0, 10);
        mem[0] = (unsigned char) 0x1f;
        mem[1] = (unsigned char) 0x8b;
        mem[2] = (unsigned char) 0x08;
#ifdef _WIN32
        mem[9] = (unsigned char) 0x0b;
#else
        mem[9] = (unsigned char) 0x03;
#endif
        holder->crc = crc32(0L, Z_NULL, 0);
        return 10;
    }
    if (holder->status == SG__HTTPRES_GZ_FINISHING)
        return MHD_CONTENT_READER_END_OF_STREAM;
    have = (size_t) read(*((int *) holder->handle), mem, size); /* TODO: 64-bit? */
    if ((ssize_t) have < 0)
        return MHD_CONTENT_READER_END_WITH_ERROR;
    if (have == 0) {
        holder->status = SG__HTTPRES_GZ_FINISHING;
        mem[0] = (unsigned char) holder->crc;
        mem[1] = (unsigned char) holder->crc >> 8;
        mem[2] = (unsigned char) holder->crc >> 16;
        mem[3] = (unsigned char) holder->crc >> 24;
        mem[4] = (unsigned char) holder->offset;
        mem[5] = (unsigned char) holder->offset >> 8;
        mem[6] = (unsigned char) holder->offset >> 16;
        mem[7] = (unsigned char) holder->offset >> 24;
        return 8;
    }
    holder->offset += have;
    holder->crc = crc32(holder->crc, (const Bytef *) mem, (uInt) have);
    if (sg__deflate(&holder->stream, mem, have, &buf, &have, holder->buf) != 0)
        have = MHD_CONTENT_READER_END_WITH_ERROR;
    else {
        memcpy(mem, buf, have);
        sg_free(buf);
    }
    return have;
}

static void sg__httpres_zffree_cb(void *handle) {
    struct sg__httpres_gzholder *holder = handle;
    if (!holder)
        return;
    deflateEnd(&holder->stream);
    sg_free(holder->buf);
    close(*((int *) holder->handle));
    sg_free(holder->handle);
    sg_free(holder);
}

#endif

struct sg_httpres *sg__httpres_new(struct MHD_Connection *con) {
    struct sg_httpres *res = sg_alloc(sizeof(struct sg_httpres));
    if (!res)
        return NULL;
    res->con = con;
    res->status = 500;
    return res;
}

void sg__httpres_free(struct sg_httpres *res) {
    if (!res)
        return;
    sg_strmap_cleanup(&res->headers);
    MHD_destroy_response(res->handle);
    sg_free(res);
}

int sg__httpres_dispatch(struct sg_httpres *res) {
    sg_strmap_iter(res->headers, sg__strmap_iter, res->handle);
    res->ret = MHD_queue_response(res->con, res->status, res->handle);
    return res->ret;
}

struct sg_strmap **sg_httpres_headers(struct sg_httpres *res) {
    if (res)
        return &res->headers;
    errno = EINVAL;
    return NULL;
}

int sg_httpres_set_cookie(struct sg_httpres *res, const char *name, const char *val) {
    char *str;
    size_t len;
    int ret;
    if (!res || !name || !val || !sg__is_cookie_name(name) || !sg__is_cookie_val(val))
        return EINVAL;
    len = strlen(name) + strlen("=") + strlen(val) + 1;
    str = sg_malloc(len);
    if (!str)
        return ENOMEM;
    snprintf(str, len, "%s=%s", name, val);
    ret = sg_strmap_add(&res->headers, MHD_HTTP_HEADER_SET_COOKIE, str);
    sg_free(str);
    return ret;
}

int sg_httpres_sendbinary(struct sg_httpres *res, void *buf, size_t size, const char *content_type,
                          unsigned int status) {
    int ret;
    if (!res || !buf || ((ssize_t) size < 0) || (status < 100) || (status > 599))
        return EINVAL;
    if (res->handle)
        return EALREADY;
    if (content_type) {
        ret = sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);
        if (ret != 0)
            return ret;
    }
    res->handle = MHD_create_response_from_buffer(size, buf, MHD_RESPMEM_MUST_COPY);
    if (!res->handle)
        return ENOMEM;
    res->status = status;
    return 0;
}

int sg_httpres_sendfile2(struct sg_httpres *res, uint64_t size, uint64_t max_size, uint64_t offset,
                         const char *filename, const char *disposition, unsigned int status) {
    struct stat sbuf;
    size_t fn_size;
    const char *fn;
    char *disp;
    int fd, errnum = 0;
    if (!res || ((int64_t) size < 0) || ((int64_t) max_size < 0) || ((int64_t) offset < 0) || !filename ||
        (status < 100) || (status > 599))
        return EINVAL;
    if (res->handle)
        return EALREADY;
    fd = open(filename, O_RDONLY);
    if ((fd == -1) || fstat(fd, &sbuf)) {
        errnum = errno;
        goto error;
    }
    if (S_ISDIR(sbuf.st_mode)) {
        errnum = EISDIR;
        goto error;
    }
    if (!S_ISREG(sbuf.st_mode)) {
        errnum = EBADF;
        goto error;
    }
    if ((max_size > 0) && ((uint64_t) sbuf.st_size > max_size)) {
        errnum = EFBIG;
        goto error;
    }
    if (disposition) {
#define SG__FNFMT "%s; filename=\"%s\""
        fn = basename(filename);
        fn_size = (size_t) snprintf(NULL, 0, SG__FNFMT, disposition, fn) + 1;
        disp = sg_malloc(fn_size);
        if (!disp) {
            errnum = ENOMEM;
            goto error;
        }
        snprintf(disp, fn_size, SG__FNFMT, disposition, fn);
#undef SG__FNFMT
        errnum = sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_DISPOSITION, disp);
        sg_free(disp);
        if (errnum != 0)
            goto error;
    }
    if (size == 0)
        size = ((uint64_t) sbuf.st_size) - offset;
    res->handle = MHD_create_response_from_fd_at_offset64(size, fd, offset);
    if (!res->handle) {
        errnum = ENOMEM;
        goto error;
    }
    res->status = status;
    return 0;
error:
    if ((fd != -1) && close(fd) && (errnum == 0))
        errnum = errno;
    return errnum;
}

int sg_httpres_sendfile(struct sg_httpres *res, uint64_t size, uint64_t max_size, uint64_t offset,
                        const char *filename, bool downloaded, unsigned int status) {
    return sg_httpres_sendfile2(res, size, max_size, offset, filename, (downloaded ? "attachment" : NULL), status);
}

int sg_httpres_sendstream(struct sg_httpres *res, uint64_t size, sg_read_cb read_cb, void *handle, sg_free_cb free_cb,
                          unsigned int status) {
    int errnum;
    if (!res || !read_cb || (status < 100) || (status > 599)) {
        errnum = EINVAL;
        goto error;
    }
    if (res->handle) {
        errnum = EALREADY;
        goto error;
    }
    res->handle = MHD_create_response_from_callback((size > 0 ? size : MHD_SIZE_UNKNOWN), SG__BLOCK_SIZE, read_cb,
                                                    handle, free_cb);
    if (!res->handle)
        return ENOMEM;
    res->status = status;
    return 0;
error:
    if (free_cb)
        free_cb(handle);
    return errnum;
}

#ifdef SG_HTTP_COMPRESSION

int sg_httpres_zsendbinary(struct sg_httpres *res, void *buf, size_t size, const char *content_type,
                           unsigned int status) {
    size_t zsize;
    void *zbuf;
    int ret;
    if (!res || !buf || ((ssize_t) size < 0) || (status < 100) || (status > 599))
        return EINVAL;
    if (res->handle)
        return EALREADY;
    if (size > 0) {
        zsize = compressBound(size);
        zbuf = sg_malloc(zsize);
        if (!zbuf)
            return ENOMEM;
        if ((sg__compress(zbuf, (uLongf *) &zsize, buf, size, Z_BEST_COMPRESSION) != Z_OK) || (zsize >= size)) {
            zsize = size;
            memcpy(zbuf, buf, zsize);
        } else {
            ret = sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_ENCODING, "deflate");
            if (ret != 0)
                goto error;
        }
    } else {
        zbuf = strdup("");
        zsize = 0;
    }
    if (content_type) {
        ret = sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);
        if (ret != 0)
            goto error;
    }
    res->handle = MHD_create_response_from_buffer(zsize, zbuf, MHD_RESPMEM_MUST_FREE);
    if (!res->handle)
        return ENOMEM;
    res->status = status;
    return 0;
error:
    sg_free(zbuf);
    return ret;
}

int sg_httpres_zsendstream(struct sg_httpres *res, sg_read_cb read_cb, void *handle, sg_free_cb free_cb,
                           unsigned int status) {
    struct sg__httpres_zholder *holder;
    int errnum;
    if (!res || !read_cb || (status < 100) || (status > 599)) {
        errnum = EINVAL;
        goto error;
    }
    if (res->handle) {
        errnum = EALREADY;
        goto error;
    }
    holder = sg_alloc(sizeof(struct sg__httpres_zholder));
    if (!holder) {
        errnum = ENOMEM;
        goto error;
    }
    errnum = deflateInit2(&holder->stream, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL,
                          Z_DEFAULT_STRATEGY);
    if (errnum != Z_OK)
        goto error_stream;
    holder->buf = sg_malloc(SG__ZLIB_CHUNK);
    if (!holder->buf) {
        errnum = ENOMEM;
        goto error_buf;
    }
    errnum = sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_ENCODING, "deflate");
    if (errnum != 0)
        goto error_res;
    holder->read_cb = read_cb;
    holder->free_cb = free_cb;
    holder->handle = handle;
    res->handle = MHD_create_response_from_callback(MHD_SIZE_UNKNOWN, SG__ZLIB_CHUNK + 128,
                                                    sg__httpres_zread_cb, holder, sg__httpres_zfree_cb);
    if (!res->handle) {
        errnum = ENOMEM;
        goto error_res;
    }
    res->status = status;
    return 0;
error_res:
    sg_free(holder->buf);
error_buf:
    deflateEnd(&holder->stream);
error_stream:
    sg_free(holder);
error:
    if (free_cb)
        free_cb(handle);
    return errnum;
}

/* TODO: WARNING: this function is experimental! */
static int sg__httpres_zsendfile2(struct sg_httpres *res, uint64_t max_size, uint64_t offset, const char *filename,
                                  const char *disposition, unsigned int status) {
    struct sg__httpres_gzholder *holder;
    struct stat sbuf;
    size_t fn_size;
    const char *fn;
    char *disp;
    int fd, errnum = 0;
    if (!res || ((int64_t) max_size < 0) || ((int64_t) offset < 0) || !filename || (status < 100) || (status > 599))
        return EINVAL;
    if (res->handle)
        return EALREADY;
    fd = open(filename, O_RDONLY);
    if ((fd == -1) || fstat(fd, &sbuf)) {
        errnum = errno;
        goto error;
    }
    if (S_ISDIR(sbuf.st_mode)) {
        errnum = EISDIR;
        goto error;
    }
    if (!S_ISREG(sbuf.st_mode)) {
        errnum = EBADF;
        goto error;
    }
    if ((max_size > 0) && ((uint64_t) sbuf.st_size > max_size)) {
        errnum = EFBIG;
        goto error;
    }
    if (lseek(fd, offset, SEEK_SET) != (__off_t) offset) { /* TODO: 64-bit? */
        errnum = errno;
        goto error;
    }
    if (disposition) {
#define SG__FNFMT "%s; filename=\"%s\""
        fn = basename(filename);
        fn_size = (size_t) snprintf(NULL, 0, SG__FNFMT, disposition, fn) + 1;
        disp = sg_malloc(fn_size);
        if (!disp) {
            errnum = ENOMEM;
            goto error;
        }
        snprintf(disp, fn_size, SG__FNFMT, disposition, fn);
#undef SG__FNFMT
        errnum = sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_DISPOSITION, disp);
        sg_free(disp);
        if (errnum != 0)
            goto error;
    }
    holder = sg_alloc(sizeof(struct sg__httpres_gzholder));
    if (!holder) {
        errnum = ENOMEM;
        goto error;
    }
    errnum = deflateInit2(&holder->stream, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL,
                          Z_DEFAULT_STRATEGY);
    if (errnum != Z_OK)
        goto error_stream;
    holder->buf = sg_malloc(SG__ZLIB_CHUNK);
    if (!holder->buf) {
        errnum = ENOMEM;
        goto error_buf;
    }
    holder->handle = sg_malloc(sizeof(int));
    if (!holder->handle) {
        errnum = ENOMEM;
        goto error_pfd;
    }
    errnum = sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_ENCODING, "gzip");
    if (errnum != 0)
        goto error_res;
    *((int *) holder->handle) = fd;
    res->handle = MHD_create_response_from_callback(MHD_SIZE_UNKNOWN, SG__ZLIB_CHUNK + 128,
                                                    sg__httpres_zfread_cb, holder, sg__httpres_zffree_cb);
    if (!res->handle) {
        errnum = ENOMEM;
        goto error_res;
    }
    res->status = status;
    return 0;
error_res:
    sg_free(holder->handle);
error_pfd:
    sg_free(holder->buf);
error_buf:
    deflateEnd(&holder->stream);
error_stream:
    sg_free(holder);
error:
    if ((fd != -1) && close(fd) && (errnum == 0))
        errnum = errno;
    return errnum;
}

/* TODO: WARNING: this function is experimental! */
int sg_httpres_zsendfile(struct sg_httpres *res, uint64_t max_size, uint64_t offset, const char *filename,
                         bool downloaded, unsigned int status) {
    return sg__httpres_zsendfile2(res, max_size, offset, filename, (downloaded ? "attachment" : NULL), status);
}

#endif

int sg_httpres_clear(struct sg_httpres *res) {
    if (!res)
        return EINVAL;
    sg_strmap_cleanup(&res->headers);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 500;
    return 0;
}
