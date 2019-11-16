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

#ifndef SG_HTTPAUTH_H
#define SG_HTTPAUTH_H

#include <stdbool.h>
#include "sg_macros.h"
#include "microhttpd.h"
#include "sg_httpres.h"

struct sg_httpauth {
  struct sg_httpres *res;
  char *realm;
  char *usr;
  char *pwd;
  bool canceled;
};

SG__EXTERN struct sg_httpauth *sg__httpauth_new(struct sg_httpres *res);

SG__EXTERN void sg__httpauth_free(struct sg_httpauth *auth);

SG__EXTERN bool sg__httpauth_dispatch(struct sg_httpauth *auth);

#endif /* SG_HTTPAUTH_H */
