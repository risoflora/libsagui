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

#ifndef SG_HTTPRES_H
#define SG_HTTPRES_H

#include "sg_macros.h"
#ifdef SG_HTTP_COMPRESSION
#include <stdint.h>
#include "zlib.h"
#endif
#include "microhttpd.h"
#include "sagui.h"

struct sg_httpres {
    struct MHD_Connection *con;
    struct MHD_Response *handle;
    struct sg_strmap *headers;
    unsigned int status;
    int ret;
};

#ifdef SG_HTTP_COMPRESSION

struct sg__httpres_zholder {
    z_stream stream;
    sg_read_cb read_cb;
    sg_free_cb free_cb;
    uint64_t offset;
    void *handle;
    void *buf;
};

enum sg__httpres_gzip_status {
    SG__HTTPRES_GZ_NONE = 0,
    SG__HTTPRES_GZ_STARDED = 1,
    SG__HTTPRES_GZ_FINISHING = 2
};

struct sg__httpres_gzholder {
    z_stream stream;
    uint64_t offset;
    uLong crc;
    void *buf;
    int *handle;
    enum sg__httpres_gzip_status status;
};

#endif

SG__EXTERN struct sg_httpres *sg__httpres_new(struct MHD_Connection *con);

SG__EXTERN void sg__httpres_free(struct sg_httpres *res);

SG__EXTERN int sg__httpres_dispatch(struct sg_httpres *res);

#endif /* SG_HTTPRES_H */
