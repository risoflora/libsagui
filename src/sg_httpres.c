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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "sg_macros.h"
#include "microhttpd.h"
#include "sagui.h"
#include "sg_utils.h"
#include "sg_strmap.h"
#include "sg_httputils.h"
#include "sg_httpres.h"

static ssize_t sg__httpfileread_cb(void *handle, __SG_UNUSED uint64_t offset, char *buf, size_t size) {
    return
#ifdef __ANDROID__
        (ssize_t)
#endif
            fread(buf, 1, size, handle);
}

static void sg__httpfilefree_cb(void *handle) {
    fclose(handle);
}

struct sg_httpres *sg__httpres_new(struct MHD_Connection *con) {
    struct sg_httpres *res;
    sg__new(res);
    res->con = con;
    res->status = 500;
    return res;
}

void sg__httpres_free(struct sg_httpres *res) {
    if (!res)
        return;
    sg_strmap_cleanup(&res->headers);
    MHD_destroy_response(res->handle);
    sg__free(res);
}

int sg__httpres_dispatch(struct sg_httpres *res) {
    sg_strmap_iter(res->headers, sg__httpheaders_iter, res->handle);
    res->ret = MHD_queue_response(res->con, res->status, res->handle);
    return res->ret;
}

struct sg_strmap **sg_httpres_headers(struct sg_httpres *res) {
    if (!res) {
        errno = EINVAL;
        return NULL;
    }
    return &res->headers;
}

int sg_httpres_set_cookie(struct sg_httpres *res, const char *name, const char *val) {
    char *str;
    size_t len;
    int ret;
    if (!res || !name || !val || !sg__is_cookie_name(name) || !sg__is_cookie_val(val))
        return EINVAL;
    len = strlen(name) + strlen("=") + strlen(val) + 1;
    sg__alloc(str, len);
    snprintf(str, len, "%s=%s", name, val);
    ret = sg_strmap_add(&res->headers, MHD_HTTP_HEADER_SET_COOKIE, str);
    sg__free(str);
    return ret;
}

int sg_httpres_sendbinary(struct sg_httpres *res, void *buf, size_t size, const char *content_type,
                          unsigned int status) {
    if (!res || !buf || ((ssize_t) size < 0) || !content_type || (status < 100) || (status > 599))
        return EINVAL;
    if (res->handle)
        return EALREADY;
    if (!(res->handle = MHD_create_response_from_buffer(size, buf, MHD_RESPMEM_MUST_COPY)))
        oom();
    if (strlen(content_type) > 0)
        sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_TYPE, content_type);
    res->status = status;
    return 0;
}

int sg_httpres_sendfile(struct sg_httpres *res, size_t block_size, uint64_t max_size, const char *filename,
                        bool rendered, unsigned int status) {
    FILE *file;
    struct stat64 sbuf;
    char *absolute_path, *cd_header;
    const char *cd_type, *cd_basename;
    size_t fn_size;
    int fd, errnum = 0;
    if (!res || block_size < 1 || ((int64_t) max_size < 0) || !filename || (status < 100) || (status > 599))
        return EINVAL;
    if (res->handle)
        return EALREADY;
    if (!(absolute_path = realpath(filename, NULL)))
        return errno;
#ifdef _WIN32
    errnum = fopen_s(&file, absolute_path, "rb");
    if (errnum)
        goto fail;
#else
    if (!(file = fopen(absolute_path, "rb"))) {
        errnum = errno;
        goto fail;
    }
#endif
    if ((fd = fileno(file)) == -1) {
        errnum = errno;
        goto fail;
    }
    if (fstat64(fd, &sbuf)) {
        errnum = errno;
        goto fail;
    }
    if (S_ISDIR(sbuf.st_mode)) {
        errnum = EISDIR;
        goto fail;
    }
    if (!S_ISREG(sbuf.st_mode)) {
        errnum = EBADF;
        goto fail;
    }
    if ((max_size > 0) && ((uint64_t) sbuf.st_size > max_size)) {
        errnum = EFBIG;
        goto fail;
    }
#define SG_FNFMT "%s; filename=\"%s\""
    cd_type = rendered ? "inline" : "attachment";
    cd_basename = basename(absolute_path);
    fn_size = (size_t) snprintf(NULL, 0, SG_FNFMT, cd_type, cd_basename) + 1;
    if (!(cd_header = sg__malloc(fn_size))) {
        errnum = ENOMEM;
        goto fail;
    }
    snprintf(cd_header, fn_size, SG_FNFMT, cd_type, cd_basename);
#undef SG_FNFMT
    sg_strmap_set(&res->headers, MHD_HTTP_HEADER_CONTENT_DISPOSITION, cd_header);
    sg__free(cd_header);
    if (!(res->handle = MHD_create_response_from_callback((uint64_t) sbuf.st_size, block_size, sg__httpfileread_cb,
                                                          file, sg__httpfilefree_cb))) {
        errnum = ENOMEM;
        goto fail;
    }
    sg__free(absolute_path);
    res->status = status;
    return 0;
fail:
    sg__free(absolute_path);
    if (file) {
        if (errnum == 0)
            errnum = fclose(file);
        else
            fclose(file);
    }
    if (errnum == ENOMEM)
        oom();
    return errnum;
}

int sg_httpres_sendstream(struct sg_httpres *res, uint64_t size, size_t block_size, sg_read_cb read_cb, void *handle,
                          sg_free_cb free_cb, unsigned int status) {
    int errnum;
    if (!res || (block_size < 1) || !read_cb || (status < 100) || (status > 599)) {
        errnum = EINVAL;
        goto failed;
    }
    if (res->handle) {
        errnum = EALREADY;
        goto failed;
    }
    if (!(res->handle = MHD_create_response_from_callback((size > 0 ? size : MHD_SIZE_UNKNOWN), block_size,
                                                          read_cb, handle, free_cb))) {
        errnum = ENOMEM;
        goto failed;
    }
    res->status = status;
    return 0;
failed:
    if (free_cb)
        free_cb(handle);
    if (errnum == ENOMEM)
        oom();
    return errnum;
}

int sg_httpres_clear(struct sg_httpres *res) {
    if (!res)
        return EINVAL;
    sg_strmap_cleanup(&res->headers);
    MHD_destroy_response(res->handle);
    res->handle = NULL;
    res->status = 500;
    return 0;
}
