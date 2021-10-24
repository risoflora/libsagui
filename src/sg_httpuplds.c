/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2021 Silvio Clecio <silvioprog@gmail.com>
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

#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
#include "sg_macros.h"
#include "uthash.h"
#include "sagui.h"
#include "sg_utils.h"
#include "sg_str.h"
#include "sg_strmap.h"
#include "sg_httpreq.h"
#include "sg_httpsrv.h"

static void sg__httpuplds_free(struct sg_httpsrv *srv, struct sg_httpreq *req);

static int sg__httpuplds_add(struct sg_httpsrv *srv, struct sg_httpreq *req,
                             const char *fieldname, const char *filename,
                             const char *content_type,
                             const char *transfer_encoding) {
  req->curr_upld = sg_alloc(sizeof(struct sg_httpupld));
  if (!req->curr_upld)
    return ENOMEM;
  LL_APPEND(req->uplds, req->curr_upld);
  req->curr_upld->dir = sg__strdup(srv->uplds_dir);
  if (!req->curr_upld->dir)
    goto error;
  req->curr_upld->field = sg__strdup(fieldname);
  req->curr_upld->name = sg__strdup(filename);
  if (!req->curr_upld->name)
    goto error;
  req->curr_upld->mime = sg__strdup(content_type);
  req->curr_upld->encoding = sg__strdup(transfer_encoding);
  req->curr_upld->save_cb = srv->upld_save_cb;
  req->curr_upld->save_as_cb = srv->upld_save_as_cb;
  return 0;
error:
  sg__httpuplds_free(NULL, req);
  return ENOMEM;
}

static void sg__httpuplds_free(struct sg_httpsrv *srv, struct sg_httpreq *req) {
  if (!req)
    return;
  if (srv && srv->upld_free_cb)
    srv->upld_free_cb(req->curr_upld->handle);
  sg_free(req->curr_upld->dir);
  sg_free(req->curr_upld->field);
  sg_free(req->curr_upld->name);
  sg_free(req->curr_upld->mime);
  sg_free(req->curr_upld->encoding);
  sg_free(req->curr_upld);
}

static enum MHD_Result
  sg__httpuplds_iter(void *cls, __SG_UNUSED enum MHD_ValueKind kind,
                     const char *key, const char *filename,
                     const char *content_type, const char *transfer_encoding,
                     const char *data, uint64_t off, size_t size) {
  struct sg__httpupld_holder *holder;
  char *val;
  if (/*kind == MHD_POSTDATA_KIND && */ size > 0) {
    holder = cls;
    if (filename) {
      if (off == 0) {
        if ((sg__httpuplds_add(holder->srv, holder->req, key, filename,
                               content_type, transfer_encoding) != 0) ||
            (holder->srv->upld_cb(holder->srv->upld_cls,
                                  &holder->req->curr_upld->handle,
                                  holder->srv->uplds_dir, key, filename,
                                  content_type, transfer_encoding) != 0))
          return MHD_NO;
      }
      if (holder->srv->upld_write_cb(holder->req->curr_upld->handle, off, data,
                                     size) == -1)
        return MHD_NO;
      holder->req->curr_upld->size += size;
      if (holder->srv->uplds_limit > 0) {
        holder->req->total_uplds_size += size;
        if (holder->req->total_uplds_size > holder->srv->uplds_limit) {
          sg__httpsrv_eprintf(holder->srv, _("Upload too large.\n"));
          return MHD_NO;
        }
      }
    } else {
      if (off == 0) {
        holder->req->curr_field = sg__strmap_new(key, data);
        if (!holder->req->curr_field)
          return MHD_NO;
        HASH_ADD_STR(holder->req->fields, key, holder->req->curr_field);
      } else {
        val = sg_realloc(holder->req->curr_field->val, off + size + 1);
        if (!val)
          return MHD_NO;
        holder->req->curr_field->val = val;
        memcpy(holder->req->curr_field->val + off, data, size + 1);
      }
      if (holder->srv->payld_limit > 0) {
        holder->req->total_fields_size += size;
        if (holder->req->total_fields_size > holder->srv->payld_limit) {
          holder->srv->err_cb(holder->srv->cls, _("Payload too large.\n"));
          return MHD_NO;
        }
      }
    }
  }
  return MHD_YES;
}

bool sg__httpuplds_process(struct sg_httpsrv *srv, struct sg_httpreq *req,
                           struct MHD_Connection *con, const char *upld_data,
                           size_t *upld_data_size, int *ret) {
  struct sg__httpupld_holder holder = {srv, req};
  if (*upld_data_size > 0) {
    req->is_uploading = true;
    if (!req->pp)
      req->pp = MHD_create_post_processor(con, srv->post_buf_size,
                                          sg__httpuplds_iter, &holder);
    if (req->pp) {
      if (MHD_post_process(req->pp, upld_data, *upld_data_size) != MHD_YES) {
        *ret = MHD_NO;
        return true;
      }
    } else {
      utstring_bincpy(req->payload->buf, upld_data, *upld_data_size);
      if ((srv->payld_limit > 0) &&
          (utstring_len(req->payload->buf) > srv->payld_limit)) {
        *ret = MHD_NO;
        utstring_clear(req->payload->buf);
        srv->err_cb(srv->cls, _("Payload too large.\n"));
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

int sg__httpupld_cb(void *cls, void **handle, const char *dir,
                    __SG_UNUSED const char *field, const char *name,
                    __SG_UNUSED const char *mime,
                    __SG_UNUSED const char *encoding) {
  char err[SG_ERR_SIZE >> 2];
  struct sg__httpupld *upld;
  struct stat sbuf;
  int errnum;
  upld = sg_alloc(sizeof(struct sg__httpupld));
  if (!upld)
    return ENOMEM;
  upld->fd = -1;
  upld->srv = cls;
  if (stat(dir, &sbuf)) {
    sg__httpsrv_eprintf(cls, _("Cannot find uploads directory \"%s\": %s.\n"),
                        dir, sg_strerror(errno, err, sizeof(err)));
    errnum = ENOENT;
    goto error_dir;
  }
  if (!S_ISDIR(sbuf.st_mode)) {
    sg__httpsrv_eprintf(cls, _("Cannot access uploads directory \"%s\": %s.\n"),
                        dir, sg_strerror(ENOTDIR, err, sizeof(err)));
    errnum = ENOTDIR;
    goto error_dir;
  }
  upld->path = sg__strjoin(PATH_SEP, dir, "sg_upld_tmp_XXXXXX");
  if (!upld->path) {
    errnum = ENOMEM;
    goto error_path;
  }
  upld->dest = sg__strjoin(PATH_SEP, dir, name);
  if (!upld->dest) {
    errnum = ENOMEM;
    goto error_path;
  }
  upld->fd = mkstemp(upld->path);
  if (upld->fd == -1) {
    errnum = errno;
    sg__httpsrv_eprintf(
      cls, _("Cannot create temporary upload file in \"%s\": %s.\n"), dir,
      sg_strerror(errnum, err, sizeof(err)));
    goto error_path;
  }
  *handle = upld;
  return 0;
error_path:
  sg_free(upld->path);
  sg_free(upld->dest);
error_dir:
  sg_free(upld);
  return errnum;
}

ssize_t sg__httpupld_write_cb(void *handle, __SG_UNUSED uint64_t offset,
                              const char *buf, size_t size) {
  return write(((struct sg__httpupld *) handle)->fd, buf, size);
}

void sg__httpupld_free_cb(void *handle) {
  struct sg__httpupld *upld = handle;
  if (!upld)
    return;
  if (upld->fd != -1)
    close(upld->fd);
  upld->fd = -1;
  unlink(upld->path);
  sg_free(upld->path);
  sg_free(upld->dest);
  sg_free(upld);
}

int sg__httpupld_save_cb(void *handle, bool overwritten) {
  struct sg__httpupld *upld = handle;
  return upld ? sg__httpupld_save_as_cb(upld, upld->dest, overwritten) : EINVAL;
}

int sg__httpupld_save_as_cb(void *handle, const char *path, bool overwritten) {
  struct sg__httpupld *upld = handle;
  struct stat sbuf;
  if (!handle || !path || (upld->fd < 0))
    return EINVAL;
  if (close(upld->fd))
    return errno;
  upld->fd = -1;
  if ((stat(path, &sbuf) >= 0) && S_ISDIR(sbuf.st_mode))
    return EISDIR;
  if (!access(path, F_OK)) {
    if (overwritten)
      unlink(path);
    else
      return EEXIST;
  }
  if (sg__rename(upld->path, path))
    return errno;
  return 0;
}

int sg_httpuplds_iter(struct sg_httpupld *uplds, sg_httpuplds_iter_cb cb,
                      void *cls) {
  struct sg_httpupld *tmp;
  int ret;
  if (!cb)
    return EINVAL;
  if (uplds)
    LL_FOREACH(uplds, tmp) {
      ret = cb(cls, tmp);
      if (ret != 0)
        return ret;
    }
  return 0;
}

int sg_httpuplds_next(struct sg_httpupld **upld) {
  if (upld) {
    *upld = (*upld) ? (*upld)->next : NULL;
    return 0;
  }
  return EINVAL;
}

unsigned int sg_httpuplds_count(struct sg_httpupld *uplds) {
  struct sg_httpupld *tmp;
  unsigned int count = 0;
  if (uplds)
    LL_COUNT(uplds, tmp, count);
  return count;
}

void *sg_httpupld_handle(struct sg_httpupld *upld) {
  if (upld)
    return upld->handle;
  errno = EINVAL;
  return NULL;
}

const char *sg_httpupld_dir(struct sg_httpupld *upld) {
  if (upld)
    return upld->dir;
  errno = EINVAL;
  return NULL;
}

const char *sg_httpupld_field(struct sg_httpupld *upld) {
  if (upld)
    return upld->field;
  errno = EINVAL;
  return NULL;
}

const char *sg_httpupld_name(struct sg_httpupld *upld) {
  if (upld)
    return upld->name;
  errno = EINVAL;
  return NULL;
}

const char *sg_httpupld_mime(struct sg_httpupld *upld) {
  if (upld)
    return upld->mime;
  errno = EINVAL;
  return NULL;
}

const char *sg_httpupld_encoding(struct sg_httpupld *upld) {
  if (upld)
    return upld->encoding;
  errno = EINVAL;
  return NULL;
}

uint64_t sg_httpupld_size(struct sg_httpupld *upld) {
  if (upld)
    return upld->size;
  errno = EINVAL;
  return 0;
}

int sg_httpupld_save(struct sg_httpupld *upld, bool overwritten) {
  if (upld)
    return upld->save_cb(upld->handle, overwritten);
  return EINVAL;
}

int sg_httpupld_save_as(struct sg_httpupld *upld, const char *path,
                        bool overwritten) {
  if (upld && path)
    return upld->save_as_cb(upld->handle, path, overwritten);
  return EINVAL;
}
