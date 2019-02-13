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

#include <string.h>
#include <errno.h>
#include "sg_entrypoint.h"

void sg__entrypoint_prepare(struct sg_entrypoint *entrypoint, char *name, void *user_data) {
    entrypoint->name = name;
    entrypoint->user_data = user_data;
}

int sg__entrypoint_cmp(const void *a, const void *b) {
    return strcmp(((struct sg_entrypoint *) a)->name, ((struct sg_entrypoint *) b)->name);
}

const char *sg_entrypoint_name(struct sg_entrypoint *entrypoint) {
    if (entrypoint)
        return entrypoint->name;
    errno = EINVAL;
    return NULL;
}

int sg_entrypoint_set_user_data(struct sg_entrypoint *entrypoint, void *data) {
    if (!entrypoint)
        return EINVAL;
    entrypoint->user_data = data;
    return 0;
}

void *sg_entrypoint_user_data(struct sg_entrypoint *entrypoint) {
    if (entrypoint)
        return entrypoint->user_data;
    errno = EINVAL;
    return NULL;
}
