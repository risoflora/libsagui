/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2020 Silvio Clecio <silvioprog@gmail.com>
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

#ifndef SG_HTTPRES_H
#define SG_HTTPRES_H

#include "sg_macros.h"
#ifdef SG_HTTP_COMPRESSION
#include <stdint.h>
#include "zlib.h"
#endif /* SG_HTTP_COMPRESSION */
#include "microhttpd.h"
#include "sagui.h"

struct sg_httpres {
  struct MHD_Connection *con;
  struct MHD_Response *handle;
  struct sg_strmap *headers;
  unsigned int status;
  int ret;
};

#ifdef SG_HTTP_COMPRESSION

enum sg__httpres_zstatus {
  SG__HTTPRES_ZPROCESSING = 0,
  SG__HTTPRES_ZWRITING = 1,
  SG__HTTPRES_ZFINISHED = 2
};

struct sg__httpres_zholder {
  z_stream stream;
  sg_read_cb read_cb;
  sg_free_cb free_cb;
  Bytef *buf_in;
  Bytef *buf_out;
  uint64_t size_in;
  uint64_t size_out;
  uint64_t offset_in;
  uint64_t offset_out;
  void *handle;
  enum sg__httpres_zstatus status;
};

enum sg__httpres_gzstatus {
  SG__HTTPRES_GZNONE = 0,
  SG__HTTPRES_GZPROCESSING = 1,
  SG__HTTPRES_GZWRITING = 2,
  SG__HTTPRES_GZFINISHING = 3,
  SG__HTTPRES_GZFINISHED = 4
};

struct sg__httpres_gzholder {
  z_stream stream;
  Bytef *buf_in;
  Bytef *buf_out;
  uint64_t size_in;
  uint64_t size_out;
  uint64_t offset_in;
  uint64_t offset_out;
  uLong crc;
  int *handle;
  enum sg__httpres_gzstatus status;
};

#endif /* SG_HTTP_COMPRESSION */

SG__EXTERN struct sg_httpres *sg__httpres_new(struct MHD_Connection *con);

SG__EXTERN void sg__httpres_free(struct sg_httpres *res);

SG__EXTERN int sg__httpres_dispatch(struct sg_httpres *res);

#endif /* SG_HTTPRES_H */
