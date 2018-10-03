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

#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include "sg_macros.h"
#include "ffi.h"
#include "sg_utils.h"
#include "sg_ffi.h"
#include "sagui.h"

static ffi_type *sg__ffi_otot(char opt) {
    switch (opt) {
        case 'b':
            return &ffi_type_sint8;
        case 'B':
            return &ffi_type_uint8;
        case 'i':
            return &ffi_type_sint;
        case 'I':
            return &ffi_type_uint;
        case 's':
            return &ffi_type_sint16;
        case 'S':
            return &ffi_type_uint16;
        case 'l':
            return &ffi_type_sint64;
        case 'L':
            return &ffi_type_uint64;
        case 'f':
            return &ffi_type_float;
        case 'd':
            return &ffi_type_double;
        case 'D':
            return &ffi_type_longdouble;
        case 'p':
            return &ffi_type_pointer;
        case 'v':
            return &ffi_type_void;
        default:;
    }
    return NULL;
}

static enum ffi_abi sg__ffi_otoa(char opt) {
    switch (opt) {
        case 'd':
            return FFI_DEFAULT_ABI;
        case 's':
            return FFI_STDCALL;
        case 'p':
            return FFI_PASCAL;
        case 'r':
            return FFI_REGISTER;
        default:;
    }
    return -1;
}

static bool sg__ffi_prep(struct sg_ffi *ffi, const char *opts) {
    while (*opts != ':') {
        if (!(*opts))
            return true;
        ffi->atypes = sg__realloc(ffi->atypes, (ffi->nargs + 1) * sizeof(ffi_type));
        if (!(*(ffi->atypes + ffi->nargs++) = sg__ffi_otot(*opts)))
            return false;
        opts++;
    }
    if (*(opts++) != ':')
        return false;
    if (!(ffi->rtype = sg__ffi_otot(*opts)))
        return false;
    if (!(*(++opts)))
        return true;
    if (*(opts++) != ':')
        return false;
    return (int) (ffi->abi = sg__ffi_otoa(*opts)) != -1;
}

struct sg_ffi *sg_ffi_new(const char *opts) {
    struct sg_ffi *ffi;
    if (!opts) {
        errno = EINVAL;
        return NULL;
    }
    sg__new(ffi);
    if (sg__ffi_prep(ffi, opts) &&
        (ffi_prep_cif(&ffi->cif, ffi->abi, ffi->nargs, ffi->rtype, ffi->atypes) == FFI_OK))
        return ffi;
    sg_ffi_free(ffi);
    errno = EINVAL;
    return NULL;
}

void sg_ffi_free(struct sg_ffi *ffi) {
    if (!ffi)
        return;
    sg__free(ffi->atypes);
    sg__free(ffi);
}

int sg_ffi_call(struct sg_ffi *ffi, sg_ffi_fn fn, void **args, void *ret) {
    if (!ffi || !fn || !args)
        return EINVAL;
    ffi_call(&ffi->cif, FFI_FN(fn), ret, args);
    return 0;
}
