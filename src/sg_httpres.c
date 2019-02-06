/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 *   –– cross-platform library which helps to develop web servers or frameworks.
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

static ssize_t sg__httpres_zread_cb(void *handle, __SG_UNUSED uint64_t offset, char *dest, size_t dest_size) {
    struct sg__httpres_zholder *holder = handle;
    void *buf;
    int errnum;
    buf = sg_malloc(dest_size);
    if (!buf)
        return MHD_CONTENT_READER_END_WITH_ERROR;
    dest_size = (size_t) holder->read_cb(holder->handle, holder->offset, buf, dest_size);
    if ((ssize_t) dest_size < 0) {
        errnum = Z_STREAM_ERROR;
        dest_size = MHD_CONTENT_READER_END_WITH_ERROR;
        goto done;
    }
    if (dest_size == 0) {
        errnum = Z_STREAM_END;
        dest_size = MHD_CONTENT_READER_END_OF_STREAM;
        goto done;
    }
    sg__zdeflate(&holder->stream, buf, dest_size, dest, &dest_size, &errnum);
    holder->offset += dest_size;
    dest_size = holder->stream.total_out;
    if (dest_size == 0)
        dest_size = MHD_CONTENT_READER_END_OF_STREAM;
done:
    sg_free(buf);
    return errnum == Z_STREAM_END ? dest_size : MHD_CONTENT_READER_END_WITH_ERROR;
}

static void sg__httpres_zfree_cb(void *handle) {
    struct sg__httpres_zholder *holder = handle;
    int errnum;
    if (!holder)
        return;
    sg__zend(&holder->stream, &errnum);
    if (holder->free_cb)
        holder->free_cb(holder->handle);
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
    if (!res || !buf || ((ssize_t) size < 0) || !content_type || (status < 100) || (status > 599))
        return EINVAL;
    if (res->handle)
        return EALREADY;
    res->handle = MHD_create_response_from_buffer(size, buf, MHD_RESPMEM_MUST_COPY);
    if (!res->handle)
        return ENOMEM;
    if (strlen(content_type) > 0)
        sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);
    res->status = status;
    return 0;
}

int sg_httpres_sendfile(struct sg_httpres *res, uint64_t size, uint64_t max_size, uint64_t offset,
                        const char *filename, bool rendered, unsigned int status) {
    struct stat sbuf;
    char *cd_header;
    const char *cd_type, *cd_basename;
    size_t fn_size;
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
#define SG__FNFMT "%s; filename=\"%s\""
    cd_type = rendered ? "inline" : "attachment";
    cd_basename = basename(filename);
    fn_size = (size_t) snprintf(NULL, 0, SG__FNFMT, cd_type, cd_basename) + 1;
    if (!(cd_header = sg_malloc(fn_size))) {
        errnum = ENOMEM;
        goto error;
    }
    snprintf(cd_header, fn_size, SG__FNFMT, cd_type, cd_basename);
#undef SG__FNFMT
    sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_DISPOSITION, cd_header);
    sg_free(cd_header);
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

int sg_httpres_sendstream(struct sg_httpres *res, uint64_t size, size_t block_size, sg_read_cb read_cb, void *handle,
                          sg_free_cb free_cb, unsigned int status) {
    int errnum;
    if (!res || (block_size < 1) || !read_cb || (status < 100) || (status > 599)) {
        errnum = EINVAL;
        goto error;
    }
    if (res->handle) {
        errnum = EALREADY;
        goto error;
    }
    res->handle = MHD_create_response_from_callback((size > 0 ? size : MHD_SIZE_UNKNOWN), block_size,
                                                    read_cb, handle, free_cb);
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
    Bytef *dest;
    uLongf dest_size;
    int errnum;
    if (!res || !buf || ((ssize_t) size < 0) || !content_type || (status < 100) || (status > 599))
        return EINVAL;
    if (res->handle)
        return EALREADY;
    if (size > 0) {
        dest_size = compressBound(size);
        dest = sg_malloc(dest_size);
        if (!dest)
            return ENOMEM;
        errnum = sg__compress(buf, size, dest, (size_t *) &dest_size);
        if ((errnum != Z_OK) || (dest_size >= size)) {
            dest_size = size;
            memcpy(dest, buf, dest_size);
        } else
            sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_ENCODING, "deflate");
    } else {
        dest_size = 0;
        dest = (Bytef *) strdup("");
    }
    res->handle = MHD_create_response_from_buffer(dest_size, dest, MHD_RESPMEM_MUST_FREE);
    if (!res->handle) {
        sg_free(dest);
        return ENOMEM;
    }
    if (strlen(content_type) > 0)
        sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);
    res->status = status;
    return 0;
}

/* TODO: WARNING: this function is experimental. */
int sg_httpres_zsendstream(struct sg_httpres *res, sg_read_cb read_cb, void *handle, sg_free_cb free_cb,
                           unsigned int status) {
    struct sg__httpres_zholder *holder = NULL;
    int errnum;
    if (!res || !read_cb || (status < 100) || (status > 599)) {
        errnum = EINVAL;
        goto error;
    }
    if (res->handle) {
        errnum = EALREADY;
        goto error;
    }
    holder = sg_malloc(sizeof(struct sg__httpres_zholder));
    if (!holder) {
        errnum = ENOMEM;
        goto error;
    }
    sg__zinit(&holder->stream, &errnum);
    if (errnum != Z_OK)
        goto error;
    holder->read_cb = read_cb;
    holder->free_cb = free_cb;
    holder->handle = handle;
    res->handle = MHD_create_response_from_callback(MHD_SIZE_UNKNOWN, SG__BLOCK_SIZE, sg__httpres_zread_cb, holder,
                                                    sg__httpres_zfree_cb);
    if (!res->handle) {
        sg__zend(&holder->stream, &errnum);
        return ENOMEM;
    }
    sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_ENCODING, "deflate");
    res->status = status;
    return 0;
error:
    if (free_cb)
        free_cb(handle);
    return errnum;
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
