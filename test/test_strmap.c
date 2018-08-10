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

#define SG_EXTERN

#include "sg_assert.h"

#include <string.h>
#include <errno.h>
#include "sg_strmap.c"

static void test__strmap_new(void) {
    struct sg_strmap *pair;
    sg__strmap_new(&pair, "ABC", "123");
    ASSERT(pair);
    ASSERT(strcmp(pair->name, "ABC") == 0);
    ASSERT(strcmp(pair->val, "123") == 0);
    ASSERT(strcmp(pair->key, "abc") == 0);
    sg__strmap_free(pair);
}

static void test__strmap_free(void) {
    sg__strmap_free(NULL);
}

static void test_strmap_name(struct sg_strmap *pair) {
    errno = 0;
    ASSERT(sg_strmap_name(NULL) == NULL);
    ASSERT(errno == EINVAL);
    ASSERT(strcmp(sg_strmap_name(pair), "abç") == 0);
}

static void test_strmap_val(struct sg_strmap *pair) {
    errno = 0;
    ASSERT(sg_strmap_val(NULL) == NULL);
    ASSERT(errno == EINVAL);
    ASSERT(strcmp(sg_strmap_val(pair), "déf") == 0);
}

static void test_strmap_add(struct sg_strmap **map, const char *name, const char *val) {
    ASSERT(sg_strmap_add(NULL, name, val) == EINVAL);
    ASSERT(sg_strmap_add(map, NULL, val) == EINVAL);
    ASSERT(sg_strmap_add(map, name, NULL) == EINVAL);

    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_count(*map) == 0);
    ASSERT(sg_strmap_add(map, "", val) == 0);
    ASSERT(sg_strmap_count(*map) == 1);
    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_add(map, name, "") == 0);
    ASSERT(sg_strmap_count(*map) == 1);

    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_add(map, name, val) == 0);
    ASSERT(sg_strmap_add(map, name, val) == 0);
    ASSERT(sg_strmap_count(*map) == 2);
}

static void test_strmap_set(struct sg_strmap **map, const char *name, const char *val) {
    ASSERT(sg_strmap_set(NULL, name, val) == EINVAL);
    ASSERT(sg_strmap_set(map, NULL, val) == EINVAL);
    ASSERT(sg_strmap_set(map, name, NULL) == EINVAL);

    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_count(*map) == 0);
    ASSERT(sg_strmap_set(map, "", val) == 0);
    ASSERT(sg_strmap_count(*map) == 1);
    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_set(map, name, "") == 0);
    ASSERT(sg_strmap_count(*map) == 1);
    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_set(map, name, val) == 0);
    ASSERT(sg_strmap_set(map, name, val) == 0);
    ASSERT(sg_strmap_count(*map) == 1);
}

static void test_strmap_find(struct sg_strmap **map, const char *name, const char *val) {
    struct sg_strmap *pair;
    ASSERT(sg_strmap_find(NULL, name, &pair) == EINVAL);
    ASSERT(sg_strmap_find(*map, NULL, &pair) == EINVAL);
    ASSERT(sg_strmap_find(*map, name, NULL) == EINVAL);

    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_count(*map) == 0);
    sg_strmap_add(map, name, val);
    ASSERT(sg_strmap_count(*map) == 1);
    ASSERT(sg_strmap_find(*map, "", &pair) == ENOENT);
    ASSERT(!pair);
    ASSERT(sg_strmap_find(*map, "xxx", &pair) == ENOENT);
    ASSERT(!pair);
    ASSERT(sg_strmap_find(*map, "yyy", &pair) == ENOENT);
    ASSERT(!pair);

    sg_strmap_add(map, "", "");
    sg_strmap_add(map, "xxx", "yyy");
    sg_strmap_add(map, "yyy", "xxx");
    ASSERT(sg_strmap_count(*map) == 4);
    ASSERT(sg_strmap_find(*map, name, &pair) == 0);
    ASSERT(pair);
    ASSERT(strcmp(sg_strmap_name(pair), name) == 0 && strcmp(sg_strmap_val(pair), val) == 0);
    ASSERT(sg_strmap_find(*map, "", &pair) == 0);
    ASSERT(pair);
    ASSERT(strcmp(sg_strmap_name(pair), "") == 0 && strcmp(sg_strmap_val(pair), "") == 0);
    ASSERT(sg_strmap_find(*map, "xxx", &pair) == 0);
    ASSERT(pair);
    ASSERT(strcmp(sg_strmap_name(pair), "xxx") == 0 && strcmp(sg_strmap_val(pair), "yyy") == 0);
    ASSERT(sg_strmap_find(*map, "yyy", &pair) == 0);
    ASSERT(pair);
    ASSERT(strcmp(sg_strmap_name(pair), "yyy") == 0 && strcmp(sg_strmap_val(pair), "xxx") == 0);
}

static void test_strmap_get(struct sg_strmap **map, const char *name, const char *val) {
    ASSERT(!sg_strmap_get(NULL, name));
    ASSERT(!sg_strmap_get(*map, NULL));

    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_count(*map) == 0);
    sg_strmap_add(map, name, val);
    ASSERT(sg_strmap_count(*map) == 1);
    ASSERT(!sg_strmap_get(*map, ""));
    ASSERT(!sg_strmap_get(*map, "xxx"));
    ASSERT(!sg_strmap_get(*map, "yyy"));

    sg_strmap_add(map, "", "");
    sg_strmap_add(map, "xxx", "yyy");
    sg_strmap_add(map, "yyy", "xxx");
    ASSERT(sg_strmap_count(*map) == 4);
    ASSERT(strcmp(sg_strmap_get(*map, name), val) == 0);
    ASSERT(strcmp(sg_strmap_get(*map, ""), "") == 0);
    ASSERT(strcmp(sg_strmap_get(*map, "xxx"), "yyy") == 0);
    ASSERT(strcmp(sg_strmap_get(*map, "yyy"), "xxx") == 0);
}

static void test_strmap_rm(struct sg_strmap **map, const char *name, const char *val) {
    struct sg_strmap *pair;
    ASSERT(sg_strmap_rm(NULL, name) == EINVAL);
    ASSERT(sg_strmap_rm(map, NULL) == EINVAL);

    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_count(*map) == 0);
    sg_strmap_add(map, name, val);
    ASSERT(sg_strmap_count(*map) == 1);
    ASSERT(sg_strmap_rm(map, "") == ENOENT);
    ASSERT(sg_strmap_count(*map) == 1);
    ASSERT(sg_strmap_rm(map, "xxx") == ENOENT);
    ASSERT(sg_strmap_count(*map) == 1);
    ASSERT(sg_strmap_rm(map, "yyy") == ENOENT);
    ASSERT(sg_strmap_count(*map) == 1);

    sg_strmap_add(map, "", "");
    sg_strmap_add(map, "xxx", "yyy");
    sg_strmap_add(map, "yyy", "xxx");
    ASSERT(sg_strmap_count(*map) == 4);

    ASSERT(sg_strmap_find(*map, name, &pair) == 0);
    ASSERT(sg_strmap_rm(map, name) == 0);
    ASSERT(sg_strmap_count(*map) == 3);
    ASSERT(sg_strmap_find(*map, name, &pair) == ENOENT);

    ASSERT(sg_strmap_find(*map, "", &pair) == 0);
    ASSERT(sg_strmap_rm(map, "") == 0);
    ASSERT(sg_strmap_count(*map) == 2);
    ASSERT(sg_strmap_find(*map, "", &pair) == ENOENT);

    ASSERT(sg_strmap_find(*map, "xxx", &pair) == 0);
    ASSERT(sg_strmap_rm(map, "xxx") == 0);
    ASSERT(sg_strmap_count(*map) == 1);
    ASSERT(sg_strmap_find(*map, "xxx", &pair) == ENOENT);

    ASSERT(sg_strmap_find(*map, "yyy", &pair) == 0);
    ASSERT(sg_strmap_rm(map, "yyy") == 0);
    ASSERT(sg_strmap_count(*map) == 0);
    ASSERT(sg_strmap_find(*map, "yyy", &pair) == EINVAL);
}

static int strmap_iter_empty(void *cls, struct sg_strmap *pair) {
    (void) cls;
    (void) pair;
    return 0;
}

static int strmap_iter_123(void *cls, struct sg_strmap *pair) {
    (void) cls;
    (void) pair;
    return 123;
}

static int strmap_iter_concat(void *cls, struct sg_strmap *pair) {
    strcat(cls, sg_strmap_name(pair));
    strcat(cls, sg_strmap_val(pair));
    return 0;
}

static void test_strmap_iter(struct sg_strmap **map) {
    char str[100];
    ASSERT(sg_strmap_iter(NULL, strmap_iter_empty, "") == EINVAL);
    ASSERT(sg_strmap_iter(*map, NULL, "") == EINVAL);

    sg_strmap_cleanup(map);
    sg_strmap_add(map, "abc", "123");
    sg_strmap_add(map, "def", "456");

    ASSERT(sg_strmap_iter(*map, strmap_iter_empty, NULL) == 0);
    ASSERT(sg_strmap_iter(*map, strmap_iter_123, NULL) == 123);

    memset(str, 0, sizeof(str));
    ASSERT(strcmp(str, "") == 0);
    ASSERT(sg_strmap_iter(*map, strmap_iter_concat, str) == 0);
    ASSERT(strcmp(str, "abc123def456") == 0);
}

static int strmap_sort_empty(void *cls, struct sg_strmap *pair_a, struct sg_strmap *pair_b) {
    sprintf(cls, "%s%s", (char *) cls, (char *) cls);
    (void) pair_a;
    (void) pair_b;
    return 0;
}

static int strmap_sort_name_desc(void *cls, struct sg_strmap *pair_a, struct sg_strmap *pair_b) {
    (void) cls;
    return strcmp(sg_strmap_name(pair_b), sg_strmap_name(pair_a));
}

static int strmap_sort_name_asc(void *cls, struct sg_strmap *pair_a, struct sg_strmap *pair_b) {
    (void) cls;
    return strcmp(sg_strmap_name(pair_a), sg_strmap_name(pair_b));
}

static int strmap_sort_val_desc(void *cls, struct sg_strmap *pair_a, struct sg_strmap *pair_b) {
    (void) cls;
    return strcmp(sg_strmap_val(pair_b), sg_strmap_val(pair_a));
}

static int strmap_sort_val_asc(void *cls, struct sg_strmap *pair_a, struct sg_strmap *pair_b) {
    (void) cls;
    return strcmp(sg_strmap_val(pair_a), sg_strmap_val(pair_b));
}

static void test_strmap_sort(struct sg_strmap **map) {
    char str[100];
    ASSERT(sg_strmap_sort(NULL, strmap_sort_empty, "") == EINVAL);
    ASSERT(sg_strmap_sort(map, NULL, "") == EINVAL);

    sg_strmap_cleanup(map);
    sg_strmap_add(map, "abc", "123");
    sg_strmap_add(map, "def", "456");

    memset(str, 0, sizeof(str));
    strcpy(str, "abc");
    ASSERT(strcmp(str, "abc") == 0);
    ASSERT(sg_strmap_sort(map, strmap_sort_empty, str) == 0);
    ASSERT(strcmp(str, "abcabc") == 0);

    memset(str, 0, sizeof(str));
    ASSERT(strcmp(str, "") == 0);
    sg_strmap_iter(*map, strmap_iter_concat, str);
    ASSERT(strcmp(str, "abc123def456") == 0);
    ASSERT(sg_strmap_sort(map, strmap_sort_name_desc, NULL) == 0);
    memset(str, 0, sizeof(str));
    sg_strmap_iter(*map, strmap_iter_concat, str);
    ASSERT(strcmp(str, "def456abc123") == 0);

    ASSERT(sg_strmap_sort(map, strmap_sort_name_asc, NULL) == 0);
    memset(str, 0, sizeof(str));
    sg_strmap_iter(*map, strmap_iter_concat, str);
    ASSERT(strcmp(str, "abc123def456") == 0);

    ASSERT(sg_strmap_sort(map, strmap_sort_val_desc, NULL) == 0);
    memset(str, 0, sizeof(str));
    sg_strmap_iter(*map, strmap_iter_concat, str);
    ASSERT(strcmp(str, "def456abc123") == 0);

    ASSERT(sg_strmap_sort(map, strmap_sort_val_asc, NULL) == 0);
    memset(str, 0, sizeof(str));
    sg_strmap_iter(*map, strmap_iter_concat, str);
    ASSERT(strcmp(str, "abc123def456") == 0);
}

static void test_strmap_count(struct sg_strmap **map, const char *name, const char *val) {
    ASSERT(sg_strmap_count(NULL) == 0);
    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_count(*map) == 0);
    sg_strmap_add(map, name, val);
    ASSERT(sg_strmap_count(*map) == 1);
    sg_strmap_add(map, "xxx", "yyy");
    ASSERT(sg_strmap_count(*map) == 2);
    sg_strmap_add(map, "yyy", "xxx");
    ASSERT(sg_strmap_count(*map) == 3);
    sg_strmap_add(map, name, val);
    ASSERT(sg_strmap_count(*map) == 4);
    sg_strmap_rm(map, name);
    ASSERT(sg_strmap_count(*map) == 3);
    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_count(*map) == 0);
}

static void test_strmap_next(struct sg_strmap **map) {
    struct sg_strmap *pair;
    char str[100];
    ASSERT(sg_strmap_next(NULL) == EINVAL);
    sg_strmap_cleanup(map);
    pair = NULL;
    ASSERT(sg_strmap_next(&pair) == 0);
    ASSERT(!pair);
    sg_strmap_add(map, "abc", "123");
    sg_strmap_add(map, "def", "456");
    sg_strmap_add(map, "xxx", "yyy");
    memset(str, 0, sizeof(str));
    ASSERT(strcmp(str, "") == 0);
    pair = *map;
    while (pair) {
        strcat(str, sg_strmap_name(pair));
        strcat(str, sg_strmap_val(pair));
        sg_strmap_next(&pair);
    }
    ASSERT(strcmp(str, "abc123def456xxxyyy") == 0);
}

static void test_strmap_cleanup(struct sg_strmap **map) {
    sg_strmap_cleanup(NULL);
    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_count(*map) == 0);
    sg_strmap_add(map, "abc", "123");
    sg_strmap_add(map, "def", "456");
    sg_strmap_add(map, "xxx", "yyy");
    ASSERT(sg_strmap_count(*map) == 3);
    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_count(*map) == 0);
    sg_strmap_cleanup(map);
    ASSERT(sg_strmap_count(*map) == 0);
}

int main(void) {
    struct sg_strmap *map = NULL, *pair;
    char name[] = "abç";
    char val[] = "déf";

    sg_strmap_add(&map, name, val);
    sg_strmap_find(map, name, &pair);
    ASSERT(pair);

    test__strmap_new();
    test__strmap_free();
    test_strmap_name(pair);
    test_strmap_val(pair);
    test_strmap_add(&map, name, val);
    test_strmap_set(&map, name, val);
    test_strmap_find(&map, name, val);
    test_strmap_get(&map, name, val);
    test_strmap_rm(&map, name, val);
    test_strmap_iter(&map);
    test_strmap_sort(&map);
    test_strmap_count(&map, name, val);
    test_strmap_next(&map);
    test_strmap_cleanup(&map);

    sg_strmap_cleanup(&map);
    return EXIT_SUCCESS;
}
