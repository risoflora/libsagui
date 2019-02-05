/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 *   –– cross-platform library which helps to develop web servers or frameworks.
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

#define SG_EXTERN

#include "sg_assert.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "sg_macros.h"
#include "sg_entrypoint.h"
#include "sg_entrypoints.c"
#include <sagui.h>

static int entrypoints_iter_empty(__SG_UNUSED void *cls, __SG_UNUSED struct sg_entrypoint *entrypoint) {
    return 0;
}

static int entrypoints_iter_123(void *cls, struct sg_entrypoint *entrypoint) {
    ASSERT(strcmp(cls, "foo") == 0);
    ASSERT(entrypoint);
    return 123;
}

static int entrypoints_iter_concat(void *cls, struct sg_entrypoint *entrypoint) {
    strcat(cls, sg_entrypoint_name(entrypoint));
    return 0;
}

static void test__entrypoints_add(struct sg_entrypoints *entrypoints) {
    struct sg_entrypoint entrypoint, *item;

    ASSERT(!entrypoints->list);
    ASSERT(entrypoints->count == 0);
    entrypoint.name = strdup("ghi");
    entrypoint.user_data = NULL;
    ASSERT(sg__entrypoints_add(entrypoints, &entrypoint, "foo") == 0);
    ASSERT(entrypoints->list);
    ASSERT(entrypoints->count == 1);
    item = entrypoints->list;
    ASSERT(strcmp(item->name, "ghi") == 0);
    ASSERT(strcmp(item->user_data, "foo") == 0);

    entrypoint.name = strdup("def");
    entrypoint.user_data = NULL;
    ASSERT(sg__entrypoints_add(entrypoints, &entrypoint, "bar") == 0);
    ASSERT(entrypoints->count == 2);
    item = entrypoints->list;
    ASSERT(strcmp(item->name, "def") == 0);
    ASSERT(strcmp(item->user_data, "bar") == 0);
    item = entrypoints->list + 1;
    ASSERT(strcmp(item->name, "ghi") == 0);
    ASSERT(strcmp(item->user_data, "foo") == 0);

    entrypoint.name = strdup("abc");
    entrypoint.user_data = NULL;
    ASSERT(sg__entrypoints_add(entrypoints, &entrypoint, "foobar") == 0);
    ASSERT(entrypoints->count == 3);
    item = entrypoints->list;
    ASSERT(strcmp(item->name, "abc") == 0);
    ASSERT(strcmp(item->user_data, "foobar") == 0);
    item = entrypoints->list + 1;
    ASSERT(strcmp(item->name, "def") == 0);
    ASSERT(strcmp(item->user_data, "bar") == 0);
    item = entrypoints->list + 2;
    ASSERT(strcmp(item->name, "ghi") == 0);
    ASSERT(strcmp(item->user_data, "foo") == 0);

    entrypoint.name = strdup("abc");
    ASSERT(sg__entrypoints_add(entrypoints, &entrypoint, NULL) == EALREADY);
    sg_free(entrypoint.name);
    entrypoint.name = strdup("def");
    ASSERT(sg__entrypoints_add(entrypoints, &entrypoint, NULL) == EALREADY);
    sg_free(entrypoint.name);
}

static void test__entrypoints_rm(struct sg_entrypoints *entrypoints) {
    struct sg_entrypoint entrypoint, *item;
    sg_entrypoints_clear(entrypoints);
    ASSERT(sg__entrypoints_rm(entrypoints, "") == ENOENT);

    entrypoint.name = strdup("/foo");
    ASSERT(sg__entrypoints_add(entrypoints, &entrypoint, NULL) == 0);
    entrypoint.name = strdup("/bar");
    ASSERT(sg__entrypoints_add(entrypoints, &entrypoint, NULL) == 0);
    entrypoint.name = strdup("/foobar");
    ASSERT(sg__entrypoints_add(entrypoints, &entrypoint, NULL) == 0);
    ASSERT(entrypoints->count == 3);
    ASSERT(sg__entrypoints_rm(entrypoints, "/foo") == 0);
    ASSERT(entrypoints->count == 2);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "/foo") == ENOENT);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "/bar") == 0);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "/foobar") == 0);
    ASSERT(sg__entrypoints_rm(entrypoints, "/bar") == 0);
    ASSERT(entrypoints->count == 1);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "/foo") == ENOENT);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "/bar") == ENOENT);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "/foobar") == 0);
    ASSERT(sg__entrypoints_rm(entrypoints, "/foobar") == 0);
    ASSERT(entrypoints->count == 0);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "/foo") == ENOENT);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "/bar") == ENOENT);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "/foobar") == ENOENT);
    ASSERT(sg__entrypoints_rm(entrypoints, "/foobar") == ENOENT);
}

static void test__entrypoints_find(struct sg_entrypoints *entrypoints) {
    struct sg_entrypoint entrypoint, *item;
    sg_entrypoints_clear(entrypoints);
    entrypoint.name = strdup("foo");
    ASSERT(sg__entrypoints_find(entrypoints, &entrypoint, &item) == ENOENT);
    sg_free(entrypoint.name);
    entrypoint.name = strdup("bar");
    ASSERT(sg__entrypoints_find(entrypoints, &entrypoint, &item) == ENOENT);
    sg_free(entrypoint.name);

    entrypoint.name = strdup("foo");
    ASSERT(sg__entrypoints_add(entrypoints, &entrypoint, NULL) == 0);
    entrypoint.name = strdup("bar");
    ASSERT(sg__entrypoints_add(entrypoints, &entrypoint, NULL) == 0);
    entrypoint.name = strdup("foo");
    ASSERT(sg__entrypoints_find(entrypoints, &entrypoint, &item) == 0);
    sg_free(entrypoint.name);
    entrypoint.name = strdup("bar");
    ASSERT(sg__entrypoints_find(entrypoints, &entrypoint, &item) == 0);
    sg_free(entrypoint.name);
}

static void test_entrypoints_add(struct sg_entrypoints *entrypoints) {
    struct sg_entrypoint *item;
    sg_entrypoints_clear(entrypoints);
    ASSERT(sg_entrypoints_add(NULL, "foo", "bar") == EINVAL);
    ASSERT(sg_entrypoints_add(entrypoints, NULL, "bar") == EINVAL);
    ASSERT(entrypoints->count == 0);

    ASSERT(sg_entrypoints_add(entrypoints, "foo", NULL) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "bar", NULL) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "foobar", NULL) == 0);
    ASSERT(entrypoints->count == 3);
    item = entrypoints->list;
    ASSERT(strcmp(item->name, "/bar") == 0);
    item = entrypoints->list + 1;
    ASSERT(strcmp(item->name, "/foo") == 0);
    item = entrypoints->list + 2;
    ASSERT(strcmp(item->name, "/foobar") == 0);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "foo") == 0);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "bar") == 0);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "foobar") == 0);
}

static void test_entrypoints_rm(struct sg_entrypoints *entrypoints) {
    struct sg_entrypoint *item;
    ASSERT(sg_entrypoints_rm(NULL, "") == EINVAL);
    ASSERT(sg_entrypoints_rm(entrypoints, NULL) == EINVAL);

    ASSERT(sg_entrypoints_clear(entrypoints) == 0);
    ASSERT(entrypoints->count == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "foo", NULL) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "bar", NULL) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "foobar", NULL) == 0);
    ASSERT(entrypoints->count == 3);
    ASSERT(sg_entrypoints_rm(entrypoints, "foo") == 0);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "foo") == ENOENT);
    ASSERT(sg_entrypoints_rm(entrypoints, "bar") == 0);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "bar") == ENOENT);
    ASSERT(sg_entrypoints_rm(entrypoints, "foobar") == 0);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "foobar") == ENOENT);
    ASSERT(sg_entrypoints_rm(entrypoints, "foobar") == ENOENT);
}

static void test_entrypoints_iter(struct sg_entrypoints *entrypoints) {
    char str[100];
    ASSERT(sg_entrypoints_iter(NULL, entrypoints_iter_empty, "foo") == EINVAL);
    ASSERT(sg_entrypoints_iter(entrypoints, NULL, "foo") == EINVAL);

    sg_entrypoints_clear(entrypoints);
    ASSERT(sg_entrypoints_iter(entrypoints, entrypoints_iter_empty, "foo") == 0);
    ASSERT(sg_entrypoints_iter(entrypoints, entrypoints_iter_123, "foo") == 0);

    ASSERT(sg_entrypoints_add(entrypoints, "foo", NULL) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "bar", NULL) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "foobar", NULL) == 0);
    ASSERT(entrypoints->count == 3);
    ASSERT(sg_entrypoints_iter(entrypoints, entrypoints_iter_empty, "foo") == 0);
    ASSERT(sg_entrypoints_iter(entrypoints, entrypoints_iter_123, "foo") == 123);

    memset(str, 0, sizeof(str));
    ASSERT(strcmp(str, "") == 0);
    ASSERT(sg_entrypoints_iter(entrypoints, entrypoints_iter_concat, str) == 0);
    ASSERT(strcmp(str, "/bar/foo/foobar") == 0);
}

static void test_entrypoints_clear(struct sg_entrypoints *entrypoints) {
    struct sg_entrypoint *item;
    ASSERT(sg_entrypoints_clear(NULL) == EINVAL);

    ASSERT(sg_entrypoints_clear(entrypoints) == 0);
    ASSERT(entrypoints->count == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "foo", NULL) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "bar", NULL) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "foobar", NULL) == 0);
    ASSERT(entrypoints->count == 3);
    ASSERT(sg_entrypoints_clear(entrypoints) == 0);
    ASSERT(entrypoints->count == 0);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "foo") == ENOENT);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "bar") == ENOENT);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "foobar") == ENOENT);
}

static void test_entrypoints_find(struct sg_entrypoints *entrypoints) {
    struct sg_entrypoint *item;
    ASSERT(sg_entrypoints_find(NULL, &item, "") == EINVAL);
    ASSERT(sg_entrypoints_find(entrypoints, NULL, "") == EINVAL);
    ASSERT(sg_entrypoints_find(entrypoints, &item, NULL) == EINVAL);

    ASSERT(sg_entrypoints_clear(entrypoints) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "foo", NULL) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "bar", NULL) == 0);
    ASSERT(sg_entrypoints_add(entrypoints, "foobar", NULL) == 0);
    ASSERT(entrypoints->count == 3);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "foo") == 0);
    ASSERT(item);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "bar") == 0);
    ASSERT(item);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "foobar") == 0);
    ASSERT(item);

    ASSERT(sg_entrypoints_clear(entrypoints) == 0);
    ASSERT(entrypoints->count == 0);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "foo") == ENOENT);
    ASSERT(!item);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "bar") == ENOENT);
    ASSERT(!item);
    ASSERT(sg_entrypoints_find(entrypoints, &item, "foobar") == ENOENT);
    ASSERT(!item);
}

int main(void) {
    struct sg_entrypoints *entrypoints = sg_entrypoints_new();
    ASSERT(entrypoints != NULL);
    test__entrypoints_add(entrypoints);
    test__entrypoints_rm(entrypoints);
    test__entrypoints_find(entrypoints);
    test_entrypoints_add(entrypoints);
    test_entrypoints_rm(entrypoints);
    test_entrypoints_iter(entrypoints);
    test_entrypoints_clear(entrypoints);
    test_entrypoints_find(entrypoints);
    sg_entrypoints_free(entrypoints);
    return EXIT_SUCCESS;
}
