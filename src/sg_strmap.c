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

#include <errno.h>
#include "sg_macros.h"
#include "sagui.h"
#include "sg_utils.h"
#include "sg_strmap.h"

struct sg_strmap *sg__strmap_new(const char *name, const char *val) {
    struct sg_strmap *pair = sg_alloc(sizeof(struct sg_strmap));
    if (!pair)
        return NULL;
    pair->key = strdup(name);
    if (!pair->key)
        goto fail;
    pair->name = strdup(name);
    if (!pair->name)
        goto fail;
    pair->val = strdup(val);
    if (!pair->val)
        goto fail;
    sg__toasciilower(pair->key);
    return pair;
fail:
    sg__strmap_free(pair);
    return NULL;
}

void sg__strmap_free(struct sg_strmap *pair) {
    if (!pair)
        return;
    sg_free(pair->key);
    sg_free(pair->name);
    sg_free(pair->val);
    sg_free(pair);
}

const char *sg_strmap_name(struct sg_strmap *pair) {
    if (!pair) {
        errno = EINVAL;
        return NULL;
    }
    return pair->name;
}

const char *sg_strmap_val(struct sg_strmap *pair) {
    if (!pair) {
        errno = EINVAL;
        return NULL;
    }
    return pair->val;
}

int sg_strmap_add(struct sg_strmap **map, const char *name, const char *val) {
    struct sg_strmap *pair;
    if (!map || !name || !val)
        return EINVAL;
    pair = sg__strmap_new(name, val);
    if (!pair)
        return ENOMEM;
    HASH_ADD_STR(*map, key, pair);
    return 0;
}

int sg_strmap_set(struct sg_strmap **map, const char *name, const char *val) {
    struct sg_strmap *pair, *tmp;
    if (!map || !name || !val)
        return EINVAL;
    pair = sg__strmap_new(name, val);
    if (!pair)
        return ENOMEM;
    HASH_REPLACE_STR(*map, key, pair, tmp);
    sg__strmap_free(tmp);
    return 0;
}

int sg_strmap_find(struct sg_strmap *map, const char *name, struct sg_strmap **pair) {
    char *key;
    if (!map || !pair || !name)
        return EINVAL;
    key = strdup(name);
    if (!key)
        return ENOMEM;
    sg__toasciilower(key);
    HASH_FIND_STR(map, key, *pair);
    if (!*pair) {
        sg_free(key);
        return ENOENT;
    }
    sg_free(key);
    return 0;
}

const char *sg_strmap_get(struct sg_strmap *map, const char *name) {
    struct sg_strmap *pair;
    if (!map || !name)
        return NULL;
    return sg_strmap_find(map, name, &pair) == 0 ? pair->val : NULL;
}

int sg_strmap_rm(struct sg_strmap **map, const char *name) {
    struct sg_strmap *pair;
    char *key;
    if (!map || !name)
        return EINVAL;
    key = strdup(name);
    if (!key)
        return ENOMEM;
    sg__toasciilower(key);
    HASH_FIND_STR(*map, key, pair);
    if (!pair) {
        sg_free(key);
        return ENOENT;
    }
    sg_free(key);
    HASH_DELETE_HH(hh, *map, &pair->hh);
    sg__strmap_free(pair);
    return 0;
}

int sg_strmap_iter(struct sg_strmap *map, sg_strmap_iter_cb cb, void *cls) {
    struct sg_strmap *pair, *tmp;
    int ret;
    if (!map || !cb)
        return EINVAL;
    HASH_ITER(hh, map, pair, tmp) {
        if ((ret = cb(cls, pair)) != 0)
            return ret;
    }
    return 0;
}

int sg_strmap_sort(struct sg_strmap **map, sg_strmap_sort_cb cb, void *cls) {
    if (!map || !cb)
        return EINVAL;
#define SG_STRMAP_CMP(a, b) cb(cls, (a), (b))
    HASH_SORT(*map, SG_STRMAP_CMP);
#undef SG_STRMAP_CMP
    return 0;
}

unsigned int sg_strmap_count(struct sg_strmap *map) {
    return HASH_COUNT(map);
}

int sg_strmap_next(struct sg_strmap **next) {
    if (!next)
        return EINVAL;
    *next = *next ? (*next)->hh.next : NULL;
    return 0;
}

void sg_strmap_cleanup(struct sg_strmap **map) {
    struct sg_strmap *pair, *tmp;
    if (map && *map) {
        HASH_ITER(hh, *map, pair, tmp) {
            HASH_DEL(*map, pair);
            sg__strmap_free(pair);
        }
        *map = NULL;
    }
}
