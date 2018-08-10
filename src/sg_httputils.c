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

#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "microhttpd.h"
#include "sg_strmap.h"
#include "sagui.h"
#include "sg_httputils.h"

int sg__httpcon_iter(void *cls, __SG_UNUSED enum MHD_ValueKind kind, const char *key, const char *val) {
    sg_strmap_add(cls, key, val);
    return MHD_YES;
}

int sg__httpheaders_iter(void *cls, struct sg_strmap *header) {
    MHD_add_response_header(cls, header->name, header->val);
    return 0;
}

ssize_t sg_httpread_end(bool err) {
    return
#ifdef __ANDROID__
        (ssize_t)
#endif
            err ? MHD_CONTENT_READER_END_WITH_ERROR : MHD_CONTENT_READER_END_OF_STREAM;
}
