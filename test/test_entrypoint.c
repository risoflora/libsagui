/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2019 Silvio Clecio <silvioprog@gmail.com>
 *
 * Sagui library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Sagui library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sagui library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "sg_assert.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "sg_entrypoint.h"
#include <sagui.h>

static void test__entrypoint_prepare(struct sg_entrypoint *entrypoint) {
  sg__entrypoint_prepare(entrypoint, NULL, NULL);
  ASSERT(!entrypoint->name);
  ASSERT(!entrypoint->user_data);
  sg__entrypoint_prepare(entrypoint, "foo", "bar");
  ASSERT(strcmp(entrypoint->name, "foo") == 0);
  ASSERT(strcmp(entrypoint->user_data, "bar") == 0);
}

static void test__entrypoint_cmp(struct sg_entrypoint *entrypoint1) {
  struct sg_entrypoint *entrypoint2 = sg_alloc(sizeof(struct sg_entrypoint));
  ASSERT(entrypoint2);
  entrypoint1->name = "foo";
  entrypoint2->name = "bar";
  ASSERT(sg__entrypoint_cmp(entrypoint1, entrypoint2) > 0);
  entrypoint1->name = "bar";
  entrypoint2->name = "foo";
  ASSERT(sg__entrypoint_cmp(entrypoint1, entrypoint2) < 0);
  entrypoint1->name = "foo";
  entrypoint2->name = "foo";
  ASSERT(sg__entrypoint_cmp(entrypoint1, entrypoint2) == 0);
  sg_free(entrypoint2);
}

static void test_entrypoint_name(struct sg_entrypoint *entrypoint) {
  errno = 0;
  ASSERT(!sg_entrypoint_name(NULL));
  ASSERT(errno == EINVAL);

  entrypoint->name = "foo";
  ASSERT(strcmp(sg_entrypoint_name(entrypoint), "foo") == 0);
}

static void test_entrypoint_set_user_data(struct sg_entrypoint *entrypoint) {
  const char *dummy = "foo";
  ASSERT(sg_entrypoint_set_user_data(NULL, (void *) dummy) == EINVAL);

  ASSERT(sg_entrypoint_set_user_data(entrypoint, (void *) dummy) == 0);
  ASSERT(strcmp(sg_entrypoint_user_data(entrypoint), "foo") == 0);
  dummy = "bar";
  ASSERT(sg_entrypoint_set_user_data(entrypoint, (void *) dummy) == 0);
  ASSERT(strcmp(sg_entrypoint_user_data(entrypoint), "bar") == 0);
}

static void test_entrypoint_user_data(struct sg_entrypoint *entrypoint) {
  errno = 0;
  ASSERT(!sg_entrypoint_user_data(NULL));
  ASSERT(errno == EINVAL);

  errno = 0;
  sg_entrypoint_set_user_data(entrypoint, NULL);
  ASSERT(!sg_entrypoint_user_data(entrypoint));
  ASSERT(errno == 0);
  sg_entrypoint_set_user_data(entrypoint, "foo");
  ASSERT(strcmp(sg_entrypoint_user_data(entrypoint), "foo") == 0);
  sg_entrypoint_set_user_data(entrypoint, "bar");
  ASSERT(strcmp(sg_entrypoint_user_data(entrypoint), "bar") == 0);
}

int main(void) {
  struct sg_entrypoint *entrypoint = sg_alloc(sizeof(struct sg_entrypoint));
  ASSERT(entrypoint);
  test__entrypoint_prepare(entrypoint);
  test__entrypoint_cmp(entrypoint);
  test_entrypoint_name(entrypoint);
  test_entrypoint_set_user_data(entrypoint);
  test_entrypoint_user_data(entrypoint);
  sg_free(entrypoint);
  return EXIT_SUCCESS;
}
