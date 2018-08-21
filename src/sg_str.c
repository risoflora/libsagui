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
#include "sagui.h"
#include "sg_str.h"

struct sg_str *sg_str_new(void) {
    struct sg_str *str;
    sg__new(str);
    utstring_new(str->buf);
    return str;
}

void sg_str_free(struct sg_str *str) {
    if (!str)
        return;
    utstring_free(str->buf);
    sg__free(str);
}

int sg_str_write(struct sg_str *str, const char *val, size_t len) {
    if (!str || !val || (len < 1))
        return EINVAL;
    utstring_bincpy(str->buf, val, len);
    return 0;
}

int sg_str_printf_va(struct sg_str *str, const char *fmt, va_list ap) {
    if (!str || !fmt
#ifndef __arm__
        || !ap
#endif
            )
        return EINVAL;
    utstring_printf_va(str->buf, fmt, ap);
    return 0;
}

int sg_str_printf(struct sg_str *str, const char *fmt, ...) {
    va_list ap;
    if (!str || !fmt)
        return EINVAL;
    va_start(ap, fmt);
    utstring_printf_va(str->buf, fmt, ap);
    va_end(ap);
    return 0;
}

const char *sg_str_content(struct sg_str *str) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    return utstring_body(str->buf);
}

size_t sg_str_length(struct sg_str *str) {
    if (!str) {
        errno = EINVAL;
        return 0;
    }
    return utstring_len(str->buf);
}

int sg_str_clear(struct sg_str *str) {
    if (!str)
        return EINVAL;
    utstring_clear(str->buf);
    return 0;
}
