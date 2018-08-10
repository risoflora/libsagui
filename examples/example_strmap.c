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

/* NOTE: Error checking has been omitted for clarity. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sagui.h>

static int map_sort(__SG_UNUSED void *cls, struct sg_strmap *pair_a, struct sg_strmap *pair_b) {
    return strcmp(sg_strmap_val(pair_b), sg_strmap_val(pair_a)); /* desc */
}

static int map_iter(__SG_UNUSED void *cls, struct sg_strmap *pair) {
    const char *name = sg_strmap_name(pair);
    printf("\t%c: %s\n", *name, name);
    return 0;
}

static void chat(struct sg_strmap **map, const char *name, const char *msg) {
    struct sg_strmap *pair;
    sg_strmap_set(map, name, msg);
    if (msg && (sg_strmap_find(*map, name, &pair) == 0))
        printf("%c:\t%s\n", *sg_strmap_name(pair), sg_strmap_val(pair));
}

int main(void) {
    struct sg_strmap *map = NULL;
    chat(&map, "Clecio", "Hello!");
    chat(&map, "Paim", "Hello. How are you?");
    chat(&map, "Clecio", "I'm fine. And you?");
    chat(&map, "Paim", "Me too.");
    printf("\nChatters:\n");
    sg_strmap_sort(&map, &map_sort, NULL);
    sg_strmap_iter(map, &map_iter, NULL);
    sg_strmap_cleanup(&map);
    return EXIT_SUCCESS;
}
