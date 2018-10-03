/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 *   –– cross-platform library which helps to develop web servers or frameworks.
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
#include <stdbool.h>
#include <string.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted to make it clear. */

static char *my_function(bool b, int i, const char *s) {
    const char *fmt = "%s|%d|%s";
    const char *tf = (b ? "TRUE" : "FALSE");
    char *str;
    size_t len = (size_t) snprintf(NULL, 0, fmt, tf, i, s) + 1;
    str = sg_alloc(len);
    snprintf(str, len, fmt, tf, i, s);
    return str;
}

int main(void) {
    struct sg_ffi *ffi;
    void *args[3];
    char *ret = NULL;
    bool b = true;
    int i = 123;
    const char *s = "abc";
    args[0] = &b;
    args[1] = &i;
    args[2] = &s;
    /* bis:s+p
     * b = param 1: bool
     * i = param 2: int
     * p = param 3: pointer
     * p = return: pointer
     * d = default ABI
     */
    //ffi = sg_ffi_new("bip:p:d");
    ffi = sg_ffi_new("bip:p:d");
    sg_ffi_call(ffi, (sg_ffi_fn) my_function, args, &ret);
    printf("my_function: %s\n", ret);
    sg_free(ret);
    sg_ffi_free(ffi);
    return EXIT_SUCCESS;
}
