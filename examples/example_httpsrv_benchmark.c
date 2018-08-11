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


#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <sagui.h>

/* NOTE: Error checking has been omitted for clarity. */

static unsigned int get_cpu_count(void) {
#ifdef _WIN32
#ifndef _SC_NPROCESSORS_ONLN
    SYSTEM_INFO info;
    GetSystemInfo(&info);
#define sysconf(void) info.dwNumberOfProcessors
#define _SC_NPROCESSORS_ONLN
#endif
#endif
#ifdef _SC_NPROCESSORS_ONLN
    return (unsigned int) sysconf(_SC_NPROCESSORS_ONLN);
#else
    return 0;
#endif
}

static void req_cb(__SG_UNUSED void *cls, __SG_UNUSED struct sg_httpreq *req, struct sg_httpres *res) {
    sg_httpres_send(res, "<html><head><title>Hello world</title></head><body>Hello world</body></html>",
                    "text/html", 200);
}

int main(void) {
    const unsigned int cpu_count = get_cpu_count();
    const unsigned int con_limit = 1000; /* Change to 10000 for C10K problem. */
    struct sg_httpsrv *srv = sg_httpsrv_new(req_cb, NULL);
    sg_httpsrv_set_thr_pool_size(srv, cpu_count);
    sg_httpsrv_set_con_limit(srv, con_limit);
    if (!sg_httpsrv_listen(srv, 0 /* 0 = port chosen randomly */, false)) {
        sg_httpsrv_free(srv);
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Number of processors: %d\n", cpu_count);
    fprintf(stdout, "Connections limit: %d\n", con_limit);
    fprintf(stdout, "Server running at http://localhost:%d\n", sg_httpsrv_port(srv));
    fflush(stdout);
    getchar();
    sg_httpsrv_free(srv);
    return EXIT_SUCCESS;
}
