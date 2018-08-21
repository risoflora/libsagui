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

#include <errno.h>
#include <sys/stat.h>
#include "sg_macros.h"
#include "uthash.h"
#include "sagui.h"
#include "sg_utils.h"
#include "sg_str.h"
#include "sg_strmap.h"
#include "sg_httpreq.h"

static void sg__httpuplds_add(struct sg_httpsrv *srv, struct sg_httpreq *req, const char *fieldname,
                              const char *filename, const char *content_type, const char *transfer_encoding) {
    sg__new(req->curr_upld);
    LL_APPEND(req->uplds, req->curr_upld);
    req->curr_upld->dir = sg__strdup(srv->uplds_dir);
    req->curr_upld->field = sg__strdup(fieldname);
    req->curr_upld->name = sg__strdup(filename);
    req->curr_upld->mime = sg__strdup(content_type);
    req->curr_upld->encoding = sg__strdup(transfer_encoding);
    req->curr_upld->save_cb = srv->upld_save_cb;
    req->curr_upld->save_as_cb = srv->upld_save_as_cb;
}

static void sg__httpuplds_free(struct sg_httpsrv *srv, struct sg_httpreq *req) {
    if (!req)
        return;
    if (srv && srv->upld_free_cb)
        srv->upld_free_cb(req->curr_upld->handle);
    sg__free(req->curr_upld->dir);
    sg__free(req->curr_upld->field);
    sg__free(req->curr_upld->name);
    sg__free(req->curr_upld->mime);
    sg__free(req->curr_upld->encoding);
    sg__free(req->curr_upld);
}

static void sg__httpuplds_err(struct sg_httpsrv *srv, const char *fmt, ...) {
    va_list ap;
    size_t size;
    char *err;
    va_start(ap, fmt);
    size = (size_t) vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);
    sg__alloc(err, size);
    va_start(ap, fmt);
    vsnprintf(err, size, fmt, ap);
    va_end(ap);
    srv->err_cb(srv->err_cls, err);
    sg__free(err);
}

static int sg__httpuplds_iter(void *cls, __SG_UNUSED enum MHD_ValueKind kind, const char *key, const char *filename,
                              const char *content_type, const char *transfer_encoding, const char *data,
                              uint64_t off, size_t size) {
    struct sg__httpupld_holder *holder;
    char *val;
    if (/*kind == MHD_POSTDATA_KIND && */ size > 0) {
        holder = cls;
        if (filename) {
            if (off == 0) {
                sg__httpuplds_add(holder->srv, holder->req, key, filename, content_type, transfer_encoding);
                if (holder->srv->upld_cb(holder->srv->upld_cls, &holder->req->curr_upld->handle, holder->srv->uplds_dir,
                                         key, filename, content_type, transfer_encoding) != 0)
                    return MHD_NO;
            }
            if (holder->srv->upld_write_cb(holder->req->curr_upld->handle, off, data, size) == (size_t) -1)
                return MHD_NO;
            holder->req->curr_upld->size += size;
            if (holder->srv->uplds_limit > 0) {
                holder->req->total_uplds_size += size;
                if (holder->req->total_uplds_size > holder->srv->uplds_limit) {
                    sg__httpuplds_err(holder->srv, _("Upload too large.\n"));
                    return MHD_NO;
                }
            }
        } else {
            if (off == 0) {
                sg__strmap_new(&holder->req->curr_field, key, data);
                HASH_ADD_STR(holder->req->fields, key, holder->req->curr_field);
            } else {
                if (!(val = sg__realloc(holder->req->curr_field->val, off + size)))
                    oom();
                holder->req->curr_field->val = val;
                memcpy(holder->req->curr_field->val + off, data, size);
            }
            if (holder->srv->payld_limit > 0) {
                holder->req->total_fields_size += size;
                if (holder->req->total_fields_size > holder->srv->payld_limit) {
                    holder->srv->err_cb(holder->srv->err_cls, _("Payload too large.\n"));
                    return MHD_NO;
                }
            }
        }
    }
    return MHD_YES;
}

bool sg__httpuplds_process(struct sg_httpsrv *srv, struct sg_httpreq *req, struct MHD_Connection *con,
                           const char *upld_data, size_t *upld_data_size, int *ret) {
    struct sg__httpupld_holder holder = {srv, req};
    if (*upld_data_size > 0) {
        req->is_uploading = true;
        if (!req->pp)
            req->pp = MHD_create_post_processor(con, srv->post_buf_size, sg__httpuplds_iter, &holder);
        if (req->pp) {
            if (MHD_post_process(req->pp, upld_data, *upld_data_size) != MHD_YES) {
                *ret = MHD_NO;
                return true;
            }
        } else {
            utstring_bincpy(req->payload->buf, upld_data, *upld_data_size);
            if ((srv->payld_limit > 0) && (utstring_len(req->payload->buf) > srv->payld_limit)) {
                *ret = MHD_NO;
                utstring_clear(req->payload->buf);
                srv->err_cb(srv->err_cls, _("Payload too large.\n"));
                return true;
            }
        }
        *upld_data_size = 0;
        *ret = MHD_YES;
        return true;
    }
    return false;
}

void sg__httpuplds_cleanup(struct sg_httpsrv *srv, struct sg_httpreq *req) {
    struct sg_httpupld *tmp;
    LL_FOREACH_SAFE(req->uplds, req->curr_upld, tmp) {
        LL_DELETE(req->uplds, req->curr_upld);
        sg__httpuplds_free(srv, req);
    }
}

int sg__httpupld_cb(void *cls, void **handle, const char *dir, __SG_UNUSED const char *field, const char *name,
                    __SG_UNUSED const char *mime, __SG_UNUSED const char *encoding) {
    struct sg__httpupld *h;
    struct stat sbuf;
    char err[ERR_BUF_SIZE];
    int fd, errnum;
    sg__new(h);
    *handle = h;
    h->srv = cls;
    if (stat(dir, &sbuf) != 0) {
        errnum = errno;
        sg__httpuplds_err(cls, _("Cannot find directory \"%s\": %s.\n"), dir, sg_strerror(errnum, err, sizeof(err)));
        goto fail;
    }
    if (!S_ISDIR(sbuf.st_mode)) {
        errnum = ENOTDIR;
        sg__httpuplds_err(cls, _("Cannot access directory \"%s\": %s.\n"), dir,
                          sg_strerror(errnum, err, sizeof(err)));
        goto fail;
    }
    if (!(h->path = sg__strjoin(PATH_SEP, dir, "sg_upld_tmp_XXXXXX"))) {
        errnum = ENOMEM;
        goto fail;
    }
    if (!(h->dest_path = sg__strjoin(PATH_SEP, dir, name))) {
        errnum = ENOMEM;
        goto fail;
    }
    fd = mkstemp(h->path);
    if (fd == -1) {
        errnum = errno;
        sg__httpuplds_err(cls, _("Cannot create temporary file in \"%s\": %s.\n"), dir,
                          sg_strerror(errnum, err, sizeof(err)));
        goto fail;
    }
    h->file = fdopen(fd, "wb");
    if (!h->file) {
        errnum = errno;
        close(fd);
        unlink(h->path);
        sg__httpuplds_err(cls, _("Cannot open temporary file \"%s\": %s.\n"), h->path,
                          sg_strerror(errnum, err, sizeof(err)));
        goto fail;
    }
    return 0;
fail:
    sg__free(h->path);
    sg__free(h->dest_path);
    sg__free(h);
    *handle = NULL;
    if (errnum == ENOMEM)
        oom();
    return errnum;
}

size_t sg__httpupld_write_cb(void *handle, __SG_UNUSED uint64_t offset, const char *buf, size_t size) {
    struct sg__httpupld *h = handle;
    size_t written = fwrite(buf, 1, size, h->file);
    if (written != size) {
        fclose(h->file);
        h->file = NULL;
        unlink(h->path);
        sg__httpuplds_err(h->srv, _("Cannot write temporary file \"%s\".\n"), h->path);
        return (size_t) -1;
    }
    return written;
}

void sg__httpupld_free_cb(void *handle) {
    struct sg__httpupld *h;
    char err[ERR_BUF_SIZE];
    if (!(h = handle))
        return;
    if (!h->file)
        goto done;
    if (fclose(h->file) == 0) {
        if (unlink(h->path) != 0) {
            sg__httpuplds_err(h->srv, _("Cannot remove temporary file \"%s\": %s.\n"), h->path,
                              sg_strerror(errno, err, sizeof(err)));
        }
        goto done;
    } else
        sg__httpuplds_err(h->srv, _("Cannot close temporary file \"%s\": %s.\n"), h->path,
                          sg_strerror(errno, err, sizeof(err)));
done:
    sg__free(h->path);
    sg__free(h->dest_path);
    sg__free(h);
}

int sg__httpupld_save_cb(void *handle, bool overwritten) {
    struct sg__httpupld *h = handle;
    return h ? sg__httpupld_save_as_cb(h, h->dest_path, overwritten) : EINVAL;
}

int sg__httpupld_save_as_cb(void *handle, const char *path, bool overwritten) {
    struct sg__httpupld *h;
    struct stat sbuf;
    int errnum;
    if (!handle)
        return EINVAL;
    h = handle;
    if (!h->file)
        return EINVAL;
    if ((errnum = fclose(h->file)) != 0)
        return -errnum;
    h->file = NULL;
    if (!path) {
        errnum = EINVAL;
        goto fail;
    }
    if ((stat(path, &sbuf) == 0) && S_ISDIR(sbuf.st_mode)) {
        errnum = EISDIR;
        goto fail;
    }
    if (access(path, F_OK) == 0) {
        if (overwritten)
            unlink(path);
        else {
            errnum = EEXIST;
            goto fail;
        }
    }
    if (sg__rename(h->path, path) != 0) {
        errnum = errno;
        goto fail;
    }
    return 0;
fail:
    unlink(h->path);
    return errnum;
}

int sg_httpuplds_iter(struct sg_httpupld *uplds, sg_httpuplds_iter_cb cb, void *cls) {
    struct sg_httpupld *tmp;
    int ret;
    if (!cb)
        return EINVAL;
    if (uplds)
        LL_FOREACH(uplds, tmp) {
            if ((ret = cb(cls, tmp)) != 0)
                return ret;
        }
    return 0;
}

int sg_httpuplds_next(struct sg_httpupld **upld) {
    if (!upld)
        return EINVAL;
    *upld = (*upld) ? (*upld)->next : NULL;
    return 0;
}

unsigned int sg_httpuplds_count(struct sg_httpupld *uplds) {
    struct sg_httpupld *tmp;
    unsigned int count = 0;
    if (uplds)
        LL_COUNT(uplds, tmp, count);
    return count;
}

void *sg_httpupld_handle(struct sg_httpupld *upld) {
    if (!upld) {
        errno = EINVAL;
        return NULL;
    }
    return upld->handle;
}

const char *sg_httpupld_dir(struct sg_httpupld *upld) {
    if (!upld) {
        errno = EINVAL;
        return NULL;
    }
    return upld->dir;
}

const char *sg_httpupld_field(struct sg_httpupld *upld) {
    if (!upld) {
        errno = EINVAL;
        return NULL;
    }
    return upld->field;
}

const char *sg_httpupld_name(struct sg_httpupld *upld) {
    if (!upld) {
        errno = EINVAL;
        return NULL;
    }
    return upld->name;
}

const char *sg_httpupld_mime(struct sg_httpupld *upld) {
    if (!upld) {
        errno = EINVAL;
        return NULL;
    }
    return upld->mime;
}

const char *sg_httpupld_encoding(struct sg_httpupld *upld) {
    if (!upld) {
        errno = EINVAL;
        return NULL;
    }
    return upld->encoding;
}

uint64_t sg_httpupld_size(struct sg_httpupld *upld) {
    if (!upld) {
        errno = EINVAL;
        return 0;
    }
    return upld->size;
}

int sg_httpupld_save(struct sg_httpupld *upld, bool overwritten) {
    if (!upld)
        return EINVAL;
    return upld->save_cb(upld->handle, overwritten);
}

int sg_httpupld_save_as(struct sg_httpupld *upld, const char *path, bool overwritten) {
    if (!upld || !path)
        return EINVAL;
    return upld->save_as_cb(upld->handle, path, overwritten);
}
