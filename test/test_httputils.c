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

#include "sg_assert.h"

#include <string.h>
#include <microhttpd.h>
#include <sagui.h>
#include "sg_strmap.h"
#include "sg_httputils.h"

static void test__httpcon_iter(void) {
    struct sg_strmap *map = NULL;
    ASSERT(sg__httpcon_iter(NULL, MHD_HEADER_KIND, "foo", "bar") == MHD_YES);
    ASSERT(!map);
    ASSERT(sg__httpcon_iter(&map, MHD_HEADER_KIND, "foo", "bar") == MHD_YES);
    ASSERT(map);
    ASSERT(strcmp(sg_strmap_get(map, "foo"), "bar") == 0);
    sg_strmap_cleanup(&map);
}

static void test__httpheaders_iter(void) {
    struct sg_strmap *header = sg_alloc(sizeof(struct sg_strmap));
    struct MHD_Response *res = sg_alloc(1);
    ASSERT(sg__httpheaders_iter(NULL, header) == 0);
    ASSERT(sg__httpheaders_iter(res, header) == 0);
    sg_free(header);
    sg_free(res);
}

static void test_httpread_end(void) {
    ASSERT(sg_httpread_end(false) == (ssize_t) MHD_CONTENT_READER_END_OF_STREAM);
    ASSERT(sg_httpread_end(true) == (ssize_t) MHD_CONTENT_READER_END_WITH_ERROR);
}

int main(void) {
    test__httpcon_iter();
    test__httpheaders_iter();
    test_httpread_end();
    return EXIT_SUCCESS;
}
