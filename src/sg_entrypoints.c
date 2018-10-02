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
#include <errno.h>
#include "sg_macros.h"
#include "sg_entrypoint.h"
#include "sg_entrypoints.h"
#include "sagui.h"

static int sg__entrypoints_add(struct sg_entrypoints *entrypoints, struct sg_entrypoint *entrypoint, void *user_data) {
    struct sg_entrypoint *list;
    if (bsearch(entrypoint, entrypoints->list, entrypoints->count, sizeof(struct sg_entrypoint), sg__entrypoint_cmp))
        return EALREADY;
    if (!(list = sg__realloc(entrypoints->list, (entrypoints->count + 1) * sizeof(struct sg_entrypoint)))) {
        sg_entrypoints_free(entrypoints);
        oom();
    }
    entrypoints->list = list;
    sg__entrypoint_prepare(entrypoints->list + entrypoints->count++, entrypoint->name, user_data);
    qsort(entrypoints->list, entrypoints->count, sizeof(struct sg_entrypoint), sg__entrypoint_cmp);
    return 0;
}

static int sg__entrypoints_rm(struct sg_entrypoints *entrypoints, const char *name) {
    struct sg_entrypoint *entrypoint;
    for (unsigned int i = 0; i < entrypoints->count; i++) {
        entrypoint = entrypoints->list + i;
        if (strcmp(entrypoint->name, name) == 0) {
            entrypoints->count--;
            sg__free(entrypoint->name);
            memcpy(entrypoint, entrypoint + 1, (entrypoints->count - i) * sizeof(struct sg_entrypoint));
            if ((entrypoint = sg__realloc(entrypoints->list, entrypoints->count * sizeof(struct sg_entrypoint))))
                entrypoints->list = entrypoint;
            else
                entrypoints->list = NULL;
            return 0;
        }
    }
    return ENOENT;
}

static int sg__entrypoints_find(struct sg_entrypoints *entrypoints, struct sg_entrypoint *key,
                                struct sg_entrypoint **entrypoint) {
    if ((entrypoints->count > 0) && (*entrypoint = bsearch(key, entrypoints->list, entrypoints->count,
                                                           sizeof(struct sg_entrypoint), sg__entrypoint_cmp)))
        return 0;
    *entrypoint = NULL;
    return ENOENT;
}

struct sg_entrypoints *sg_entrypoints_new(void) {
    return sg_alloc(sizeof(struct sg_entrypoints));
}

void sg_entrypoints_free(struct sg_entrypoints *entrypoints) {
    sg_entrypoints_clear(entrypoints);
    sg__free(entrypoints);
}

int sg_entrypoints_add(struct sg_entrypoints *entrypoints, const char *path, void *user_data) {
    struct sg_entrypoint entrypoint;
    int ret;
    if (!entrypoints || !path)
        return EINVAL;
    if (!(entrypoint.name = sg_extract_entrypoint(path)))
        oom();
    if ((ret = sg__entrypoints_add(entrypoints, &entrypoint, user_data)) != 0)
        sg__free(entrypoint.name);
    return ret;
}

int sg_entrypoints_rm(struct sg_entrypoints *entrypoints, const char *path) {
    char *name;
    int ret;
    if (!entrypoints || !path)
        return EINVAL;
    if (!(name = sg_extract_entrypoint(path)))
        oom();
    ret = sg__entrypoints_rm(entrypoints, name);
    sg__free(name);
    return ret;
}

int sg_entrypoints_iter(struct sg_entrypoints *entrypoints, sg_entrypoints_iter_cb cb, void *cls) {
    int ret;
    if (!entrypoints || !cb)
        return EINVAL;
    for (unsigned int i = 0; i < entrypoints->count; i++)
        if ((ret = cb(cls, entrypoints->list + i)) != 0)
            return ret;
    return 0;
}

int sg_entrypoints_clear(struct sg_entrypoints *entrypoints) {
    if (!entrypoints)
        return EINVAL;
    for (unsigned int i = 0; i < entrypoints->count; i++)
        sg__free((entrypoints->list + i)->name);
    sg__free(entrypoints->list);
    entrypoints->list = NULL;
    entrypoints->count = 0;
    return 0;
}

int sg_entrypoints_find(struct sg_entrypoints *entrypoints, struct sg_entrypoint **entrypoint, const char *path) {
    struct sg_entrypoint key;
    int ret;
    if (!entrypoints || !entrypoint || !path)
        return EINVAL;
    if (!(key.name = sg_extract_entrypoint(path)))
        oom();
    ret = sg__entrypoints_find(entrypoints, &key, entrypoint);
    sg__free(key.name);
    return ret;
}
