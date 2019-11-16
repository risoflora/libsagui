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

#ifndef SG_HTTPSRV_H
#define SG_HTTPSRV_H

#include <stdint.h>
#include "microhttpd.h"
#include "sagui.h"

struct sg_httpsrv {
  struct MHD_Daemon *handle;
  sg_httpauth_cb auth_cb;
  sg_httpupld_cb upld_cb;
  sg_write_cb upld_write_cb;
  sg_free_cb upld_free_cb;
  sg_save_cb upld_save_cb;
  sg_save_as_cb upld_save_as_cb;
  sg_httpreq_cb req_cb;
  sg_err_cb err_cb;
  void *upld_cls;
  void *cls;
  char *uplds_dir;
  size_t post_buf_size;
  size_t payld_limit;
  uint64_t uplds_limit;
  unsigned int thr_pool_size;
  unsigned int con_timeout;
  unsigned int con_limit;
};

#endif /* SG_HTTPSRV_H */
