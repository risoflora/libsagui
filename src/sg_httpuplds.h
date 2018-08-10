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

#ifndef SG_HTTPUPLDS_H
#define SG_HTTPUPLDS_H

#include <stdint.h>
#include "sg_macros.h"
#include "utlist.h"
#include "microhttpd.h"
#include "sg_httpreq.h"
#include "sg_httpsrv.h"

struct sg_httpupld {
    struct sg_httpupld *next;
    sg_save_cb save_cb;
    sg_save_as_cb save_as_cb;
    void *handle;
    char *dir;
    char *field;
    char *name;
    char *mime;
    char *encoding;
    uint64_t size;
};

struct sg__httpupld {
    struct sg_httpsrv *srv;
    FILE *file;
    char *path;
    char *dest_path;
};

struct sg__httpupld_holder {
    struct sg_httpsrv *srv;
    struct sg_httpreq *req;
};

SG__EXTERN bool sg__httpuplds_process(struct sg_httpsrv *srv, struct sg_httpreq *req, struct MHD_Connection *con,
                                      const char *upld_data, size_t *upld_data_size, int *ret);

SG__EXTERN void sg__httpuplds_cleanup(struct sg_httpsrv *srv, struct sg_httpreq *req);

SG__EXTERN int sg__httpupld_cb(void *cls, void **handle, const char *dir, const char *field, const char *name,
                               const char *mime, const char *encoding);

SG__EXTERN size_t sg__httpupld_write_cb(void *handle, uint64_t offset, const char *buf, size_t size);

SG__EXTERN void sg__httpupld_free_cb(void *handle);

SG__EXTERN int sg__httpupld_save_cb(void *handle, bool overwritten);

SG__EXTERN int sg__httpupld_save_as_cb(void *handle, const char *path, bool overwritten);

#endif /* SG_HTTPUPLDS_H */
