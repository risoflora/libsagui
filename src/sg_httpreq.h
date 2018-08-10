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

#ifndef SG_HTTPREQ_H
#define SG_HTTPREQ_H

#include <stdbool.h>
#include "sg_macros.h"
#include "microhttpd.h"
#include "sagui.h"
#include "sg_httpuplds.h"
#include "sg_httpres.h"

struct sg_httpreq {
    struct MHD_Connection *con;
    struct MHD_PostProcessor *pp;
    struct sg_httpauth *auth;
    struct sg_httpres *res;
    struct sg_httpupld *uplds;
    struct sg_httpupld *curr_upld;
    struct sg_strmap *curr_field;
    struct sg_strmap *headers;
    struct sg_strmap *cookies;
    struct sg_strmap *params;
    struct sg_strmap *fields;
    struct sg_str *payload;
    const char *version;
    const char *method;
    const char *path;
    void *user_data;
    uint64_t total_uplds_size;
    size_t total_fields_size;
    bool is_uploading;
};

SG__EXTERN struct sg_httpreq *sg__httpreq_new(struct MHD_Connection *con, const char *version, const char *method,
                                              const char *path);

SG__EXTERN void sg__httpreq_free(struct sg_httpreq *req);

#endif /* SG_HTTPREQ_H */
