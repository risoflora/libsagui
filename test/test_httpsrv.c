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

#ifndef TEST_HTTPSRV_PORT
#define TEST_HTTPSRV_PORT 8080
#endif

#include "sg_assert.h"

#include <sagui.h>
#include "sg_httpuplds.h"
#include "sg_httpsrv.c"

const char private_key[] =
    "-----BEGIN PRIVATE KEY-----\n"
    "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\n"
    "ZXLHRNJloH4aoqAMpMIldWST/VRLB+Pn\n"
    "-----END PRIVATE KEY-----";

const char certificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIE/jCCAuagAwIBAgIJAKktAyUaaqdMMA0GCSqGSIb3DQEBCwUAMBQxEjAQBgNV\n"
    "BAMMCWxvY2FsaG9zdDAeFw0xODA3MjkwNTIyMzhaFw0yODA3MjYwNTIyMzhaMBQx\n"
    "EjAQBgNVBAMMCWxvY2FsaG9zdDCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoC\n"
    "ggIBAMZZPIJo+7zPEsmmscgYZu7EL7V6BVvCHFSP7vyKTULCYEo5o+i7z8aJAmiv\n"
    "sIgj4KFaewoGTWT+CRkVFNrq5QErmTlOm4goVcki72vZY2Bz/89vh2yvHrnfQ+sj\n"
    "y1ka5zWGPWXRfE6rzrBbZmb4rmdz06ZnMsuzJs7ceWEI/Ok9HdWdnRIC2IL6jw6J\n"
    "q1AXVRcGsNpGTQ5M2AbEfQLVXWHlhD4vDCMOlrmapcy8NYhCdAvjrw5S3Ta9NqCY\n"
    "/joGmk3cGdvbNmphtsUrfmmCQdh3dCnKCVOSmkm/bh72s1CfUj6Y1ID9SG84IagT\n"
    "yhlPF9EBwV4QpUWIR+LwqULpKit6aa2SS5qXnqHnRmYUaRZSCbNoXsEE9Kmc7+Yf\n"
    "cAzLnQkFxgqzUWp0YXVdbJbfQLW/xwlXJd+Njm01uNcebngpK332Z7nH0xG5u5Ge\n"
    "kBlchJlzZIV9ke747HSnaADD2KE/gFE6oA31yKcOqqT4CnDMvjsDs3SWqoeDXQ9v\n"
    "FX6E2U9JBdi5qWPxS2fBv5CETvGMdMWdJ8HYejX7N65LvfLxWMQXU840VWBgbjhj\n"
    "x3ovzon4JiXSbh6E6qVMEIHgBmfFOU4tjh/6EJNuPXqeSgXrJomvu+mE9OwvQMiP\n"
    "YiQeqGfxOjtyZJ+fRd0zw7caBFTCY4OZ5+TwqH0czH/W4iydAgMBAAGjUzBRMB0G\n"
    "A1UdDgQWBBSPovbCDHE3rtOPd4V8x5dFWsvj9jAfBgNVHSMEGDAWgBSPovbCDHE3\n"
    "rtOPd4V8x5dFWsvj9jAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IC\n"
    "AQAe3WAiEueLt3nNUBHQBlnQF8DRgtdXWuy/0wNWiXXqk59pzHyWo/RGaeFLU055\n"
    "l5WPkF2t/7cdVP4sPqYYBfWRLcVWBGLG6X+t7svqawq9z8rAv9Ok8LNnFLEdYYK7\n"
    "bqqbIQSneyJq7s31+FEyDWJMUMcWR2GFhNe1NVWUvSwvnkDrr7oFlcf9UA8yxxPL\n"
    "lydnOsLQxL9sgpbzvuWomFXORSXfQAkx6NWzVnlBzKG+ikZXWOt98M2Ufc58KHUP\n"
    "KfOebuPktoi2e7DKlRw5sl3zSNqqHDd3hMTRnW3foHG/dAki50P/lv2iCrrldUY8\n"
    "qAzV8Cvy4EyHOOi9GwDrGqqKeItAv0BWPk707rnqUq9bfB/w9gms/DUIL7Xk6C18\n"
    "7oR12ypWJvrgFlTH+x9fkN+S0sPw7BW3ycK3dBpzCn8lhpOs8ScALBcmPpjZLEXK\n"
    "4+Q+g01hzFZUT6NJvIq4V4xxkiklrNXmbTNhxuzju5iKJE7C+Vq4pVMGEAfVZjdK\n"
    "xC7EUyKBXrM9oJQIC5pmRKxauXE1iuQ1YxqtD3LmETH8X7z/UT+1oPawHVWuLL39\n"
    "rS6424vBDc2LPk3EM2m8pXSU7wm2X9/QS1ba7DeGTS5Z3X90e78nbWgsT6WSFXPc\n"
    "0TVbmdKpoMAfw8u+1NAmY6DrpHRTUOzJ6bsjwi9pUbP79A==\n"
    "-----END CERTIFICATE-----";

static void dummy_httpreq_cb(void *cls, struct sg_httpreq *req, struct sg_httpres *res) {
    (void) cls;
    (void) req;
    (void) res;
}

static void dummy_httpreq_err_cb(void *cls, const char *err) {
    (void) cls;
    (void) err;
}

static void dummy_httpreq_err_cpy_cb(void *cls, const char *err) {
    strcpy(cls, err);
}

static bool dummy_httpreq_httpauth_cb(void *cls, struct sg_httpauth *auth, struct sg_httpreq *req,
                                      struct sg_httpres *res) {
    (void) cls;
    (void) auth;
    (void) req;
    (void) res;
    return true;
}

static int dummy_httpupld_cb(void *cls, void **handle, const char *dir, const char *field, const char *name,
                             const char *mime, const char *encoding) {
    (void) cls;
    (void) handle;
    (void) dir;
    (void) field;
    (void) name;
    (void) mime;
    (void) encoding;
    return 0;
}

static size_t dummy_httpupld_write_cb(void *handle, uint64_t offset, const char *buf, size_t size) {
    (void) handle;
    (void) offset;
    (void) buf;
    (void) size;
    return 0;
}

static void dummy_httpupld_free_cb(void *handle) {
    (void) handle;
}

static int dummy_httpupld_save_cb(void *handle, bool overwritten) {
    (void) handle;
    (void) overwritten;
    return 0;
}

static int dummy_httpupld_save_as_cb(void *handle, const char *path, bool overwritten) {
    (void) handle;
    (void) path;
    (void) overwritten;
    return 0;
}

static void test__httpsrv_oel(const char *fmt, ...) {
    struct sg_httpsrv *srv;
    char err[256];
    va_list ap;
    ASSERT(srv = sg_httpsrv_new2(dummy_httpreq_httpauth_cb, NULL, dummy_httpreq_cb, NULL,
                                 dummy_httpreq_err_cpy_cb, err));
    va_start(ap, fmt);
    memset(err, 0, sizeof(err));
    sg__httpsrv_oel(srv, fmt, ap);
    va_end(ap);
    ASSERT(strcmp(err, "abc123") == 0);
    sg_httpsrv_free(srv);
}

static void test__httpsrv_ahc(struct sg_httpsrv *srv) {
    struct sg_httpreq *req = NULL;
    size_t size = 0;
    ASSERT(sg__httpsrv_ahc(srv, NULL, "abc", "def", "ghi", NULL, &size, (void **) &req) == MHD_YES);
    ASSERT(sg__httpsrv_ahc(srv, NULL, "abc", "def", "ghi", NULL, &size, (void **) &req) == MHD_NO);
    ASSERT(req);
    sg__httpreq_free(req);
}

static void test__httpsrv_rcc(void) {
    struct sg_httpreq *req = sg__httpreq_new(NULL, NULL, NULL, NULL);
    sg__httpsrv_rcc(NULL, NULL, (void **) &req, MHD_REQUEST_TERMINATED_COMPLETED_OK);
    ASSERT(!req);
}

static void test__httpsrv_addopt(void) {
    struct MHD_OptionItem ops[8];
    unsigned char pos = 0;
    int dummy = 123;
    memset(ops, 0, sizeof(ops));
    sg__httpsrv_addopt(ops, &pos, MHD_OPTION_EXTERNAL_LOGGER, 456, &dummy);
    ASSERT(ops[0].option == MHD_OPTION_EXTERNAL_LOGGER);
    ASSERT(ops[0].value == 456);
    ASSERT(*((int *) ops[0].ptr_value) == 123);
    ASSERT(pos == 1);
    dummy = 321;
    sg__httpsrv_addopt(ops, &pos, MHD_OPTION_NOTIFY_COMPLETED, 789, &dummy);
    ASSERT(ops[1].option == MHD_OPTION_NOTIFY_COMPLETED);
    ASSERT(ops[1].value == 789);
    ASSERT(*((int *) ops[1].ptr_value) == 321);
    ASSERT(pos == 2);
    sg__httpsrv_addopt(ops, &pos, MHD_OPTION_END, 0, NULL);
    ASSERT(ops[2].option == MHD_OPTION_END);
    ASSERT(ops[2].value == 0);
    ASSERT(!ops[2].ptr_value);
}

static void test_httpsrv_new2(void) {
    struct sg_httpsrv *srv;
    int dummy = 0;
    char *tmp;

    errno = 0;
    ASSERT(!sg_httpsrv_new2(dummy_httpreq_httpauth_cb, &dummy, NULL, &dummy, dummy_httpreq_err_cb, &dummy));
    ASSERT(errno == EINVAL);
    errno = 0;
    ASSERT(!sg_httpsrv_new2(dummy_httpreq_httpauth_cb, &dummy, dummy_httpreq_cb, &dummy, NULL, &dummy));
    ASSERT(errno == EINVAL);

    ASSERT(srv = sg_httpsrv_new2(dummy_httpreq_httpauth_cb, &dummy, dummy_httpreq_cb, &dummy,
                                 dummy_httpreq_err_cb, &dummy));
    ASSERT(srv->auth_cb == dummy_httpreq_httpauth_cb);
    ASSERT(srv->auth_cls == &dummy);
    ASSERT(srv->upld_cb == sg__httpupld_cb);
    ASSERT(srv->upld_cls == srv);
    ASSERT(srv->upld_write_cb == sg__httpupld_write_cb);
    ASSERT(srv->upld_free_cb == sg__httpupld_free_cb);
    ASSERT(srv->upld_save_cb == sg__httpupld_save_cb);
    ASSERT(srv->upld_save_as_cb == sg__httpupld_save_as_cb);
    ASSERT(srv->req_cb == dummy_httpreq_cb);
    ASSERT(srv->req_cls == &dummy);
    ASSERT(srv->err_cb == dummy_httpreq_err_cb);
    ASSERT(srv->err_cls == &dummy);
    tmp = sg_tmpdir();
    ASSERT(strcmp(srv->uplds_dir, tmp) == 0);
    sg_free(tmp);
#ifdef __arm__
    ASSERT(srv->post_buf_size == 1024);
    ASSERT(srv->payld_limit == 1048576);
    ASSERT(srv->uplds_limit == 16777216);
#else
    ASSERT(srv->post_buf_size == 4096);
    ASSERT(srv->payld_limit == 4194304);
    ASSERT(srv->uplds_limit == 67108864);
#endif
    sg_httpsrv_free(srv);
}

static void test_httpsrv_new(void) {
    struct sg_httpsrv *srv;
    int dummy = 0;

    errno = 0;
    ASSERT(!sg_httpsrv_new(NULL, &dummy));
    ASSERT(errno == EINVAL);

    errno = 0;
    ASSERT(srv = sg_httpsrv_new(dummy_httpreq_cb, NULL));
    ASSERT(errno == 0);
    sg_httpsrv_free(srv);
    errno = 0;
    ASSERT(srv = sg_httpsrv_new(dummy_httpreq_cb, &dummy));
    ASSERT(errno == 0);
    sg_httpsrv_free(srv);
}

static void test_httpsrv_free(void) {
    sg_httpsrv_free(NULL);
}

static void test_httpsrv_listen(struct sg_httpsrv *srv) {
    struct sg_httpsrv *dummy_srv;

    errno = 0;
    ASSERT(!sg_httpsrv_listen(NULL, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    dummy_srv->upld_cb = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_listen(dummy_srv, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    dummy_srv->upld_write_cb = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_listen(dummy_srv, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    dummy_srv->upld_save_cb = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_listen(dummy_srv, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    dummy_srv->upld_save_as_cb = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_listen(dummy_srv, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    sg_free(dummy_srv->uplds_dir);
    dummy_srv->uplds_dir = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_listen(dummy_srv, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    dummy_srv->post_buf_size = 255;
    errno = 0;
    ASSERT(!sg_httpsrv_listen(dummy_srv, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);

    ASSERT(sg_httpsrv_listen(srv, 0, true));
    ASSERT(sg_httpsrv_shutdown(srv) == 0);

    ASSERT(sg_httpsrv_listen(srv, TEST_HTTPSRV_PORT, false));
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_DUAL_STACK);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_ERROR_LOG);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_AUTO_INTERNAL_THREAD);
    ASSERT(!(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_THREAD_PER_CONNECTION));
    ASSERT(sg_httpsrv_shutdown(srv) == 0);
    ASSERT(sg_httpsrv_listen(srv, TEST_HTTPSRV_PORT, true));
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_DUAL_STACK);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_ERROR_LOG);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_AUTO_INTERNAL_THREAD);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_THREAD_PER_CONNECTION);
#ifdef __linux__
    dummy_srv = sg_httpsrv_new2(NULL, NULL, dummy_httpreq_cb, NULL, dummy_httpreq_err_cb, NULL);
    errno = 0;
    ASSERT(!sg_httpsrv_listen(dummy_srv, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EADDRINUSE);
    sg_httpsrv_free(dummy_srv);
#endif
}

#ifdef SG_HTTPS_SUPPORT

static void test_httpsrv_tls_listen(struct sg_httpsrv *srv) {
    struct sg_httpsrv *dummy_srv;

    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen(NULL, private_key, certificate, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen(srv, NULL, certificate, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen(srv, private_key, NULL, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    dummy_srv->upld_cb = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen(dummy_srv, private_key, certificate, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    dummy_srv->upld_write_cb = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen(dummy_srv, private_key, certificate, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    dummy_srv->upld_save_cb = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen(dummy_srv, private_key, certificate, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    dummy_srv->upld_save_as_cb = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen(dummy_srv, private_key, certificate, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    sg_free(dummy_srv->uplds_dir);
    dummy_srv->uplds_dir = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen(dummy_srv, private_key, certificate, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);
    dummy_srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    dummy_srv->post_buf_size = 255;
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen(dummy_srv, private_key, certificate, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EINVAL);
    sg_httpsrv_free(dummy_srv);

    ASSERT(sg_httpsrv_shutdown(srv) == 0);
    ASSERT(sg_httpsrv_tls_listen(srv, private_key, certificate, 0, true));
    ASSERT(sg_httpsrv_shutdown(srv) == 0);

    ASSERT(sg_httpsrv_tls_listen(srv, private_key, certificate, TEST_HTTPSRV_PORT, false));
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_DUAL_STACK);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_ERROR_LOG);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_AUTO_INTERNAL_THREAD);
    ASSERT(!(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_THREAD_PER_CONNECTION));
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_TLS);
    ASSERT(sg_httpsrv_shutdown(srv) == 0);
    ASSERT(sg_httpsrv_tls_listen(srv, private_key, certificate, TEST_HTTPSRV_PORT, true));
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_DUAL_STACK);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_ERROR_LOG);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_AUTO_INTERNAL_THREAD);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_THREAD_PER_CONNECTION);
    ASSERT(MHD_get_daemon_info(srv->handle, MHD_DAEMON_INFO_FLAGS)->flags & MHD_USE_TLS);
#ifdef __linux__
    dummy_srv = sg_httpsrv_new2(NULL, NULL, dummy_httpreq_cb, NULL, dummy_httpreq_err_cb, NULL);
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen(dummy_srv, private_key, certificate, TEST_HTTPSRV_PORT, true));
    ASSERT(errno == EADDRINUSE);
    sg_httpsrv_free(dummy_srv);
#endif
    ASSERT(sg_httpsrv_shutdown(srv) == 0);
    ASSERT(sg_httpsrv_listen(srv, 0, false));
    ASSERT(sg_httpsrv_shutdown(srv) == 0);
}

static void test_httpsrv_tls_listen2(struct sg_httpsrv *srv) {
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen2(srv, NULL, "", "", "", "", TEST_HTTPSRV_PORT, false));
    ASSERT(errno == EINVAL);
    errno = 0;
    ASSERT(!sg_httpsrv_tls_listen2(srv, "", "", NULL, "", "", TEST_HTTPSRV_PORT, false));
    ASSERT(errno == EINVAL);
}

#endif

static void test_httpsrv_shutdown(struct sg_httpsrv *srv) {
    ASSERT(sg_httpsrv_shutdown(NULL) == EINVAL);

    ASSERT(sg_httpsrv_shutdown(srv) == 0);
    ASSERT(!srv->handle);
    ASSERT(sg_httpsrv_shutdown(srv) == 0);
    ASSERT(sg_httpsrv_listen(srv, TEST_HTTPSRV_PORT, true));
}

static void test_httpsrv_port(struct sg_httpsrv *srv) {
    void *saved_handle;

    errno = 0;
    ASSERT(sg_httpsrv_port(NULL) == 0);
    ASSERT(errno == EINVAL);

    saved_handle = srv->handle;
    srv->handle = NULL;
    errno = 0;
    ASSERT(sg_httpsrv_port(srv) == 0);
    ASSERT(errno == 0);
    srv->handle = saved_handle;

    errno = 0;
    ASSERT(sg_httpsrv_port(srv) == TEST_HTTPSRV_PORT);
    ASSERT(errno == 0);
}

static void test_httpsrv_is_threaded(struct sg_httpsrv *srv) {
    void *saved_handle;

    errno = 0;
    ASSERT(!sg_httpsrv_is_threaded(NULL));
    ASSERT(errno == EINVAL);
    errno = 0;

    saved_handle = srv->handle;
    srv->handle = NULL;
    errno = 0;
    ASSERT(!sg_httpsrv_is_threaded(srv));
    ASSERT(errno == 0);
    srv->handle = saved_handle;

    ASSERT(sg_httpsrv_shutdown(srv) == 0);
    ASSERT(sg_httpsrv_listen(srv, TEST_HTTPSRV_PORT, false));
    errno = 0;
    ASSERT(!sg_httpsrv_is_threaded(srv));
    ASSERT(errno == 0);

    ASSERT(sg_httpsrv_shutdown(srv) == 0);
    ASSERT(sg_httpsrv_listen(srv, TEST_HTTPSRV_PORT, true));
    errno = 0;
    ASSERT(sg_httpsrv_is_threaded(srv));
    ASSERT(errno == 0);
}

static void test__httpsrv_set_upld_cbs(struct sg_httpsrv *srv) {
    int dummy = 123;

    ASSERT(sg_httpsrv_set_upld_cbs(NULL, dummy_httpupld_cb, &dummy, dummy_httpupld_write_cb, dummy_httpupld_free_cb,
                                   dummy_httpupld_save_cb, dummy_httpupld_save_as_cb) == EINVAL);
    ASSERT(sg_httpsrv_set_upld_cbs(srv, NULL, &dummy, dummy_httpupld_write_cb, dummy_httpupld_free_cb,
                                   dummy_httpupld_save_cb, dummy_httpupld_save_as_cb) == EINVAL);
    ASSERT(sg_httpsrv_set_upld_cbs(srv, dummy_httpupld_cb, &dummy, NULL, dummy_httpupld_free_cb,
                                   dummy_httpupld_save_cb, dummy_httpupld_save_as_cb) == EINVAL);
    ASSERT(sg_httpsrv_set_upld_cbs(srv, dummy_httpupld_cb, &dummy, dummy_httpupld_write_cb, dummy_httpupld_free_cb,
                                   NULL, dummy_httpupld_save_as_cb) == EINVAL);
    ASSERT(sg_httpsrv_set_upld_cbs(srv, dummy_httpupld_cb, &dummy, dummy_httpupld_write_cb, dummy_httpupld_free_cb,
                                   dummy_httpupld_save_cb, NULL) == EINVAL);

    ASSERT(sg_httpsrv_set_upld_cbs(srv, dummy_httpupld_cb, &dummy, dummy_httpupld_write_cb, dummy_httpupld_free_cb,
                                   dummy_httpupld_save_cb, dummy_httpupld_save_as_cb) == 0);
    ASSERT(srv->upld_cb == dummy_httpupld_cb);
    ASSERT(srv->upld_write_cb == dummy_httpupld_write_cb);
    ASSERT(srv->upld_write_cb == dummy_httpupld_write_cb);
    ASSERT(srv->upld_free_cb == dummy_httpupld_free_cb);
    ASSERT(srv->upld_save_cb == dummy_httpupld_save_cb);
    ASSERT(srv->upld_save_as_cb == dummy_httpupld_save_as_cb);
    ASSERT(*((int *) srv->upld_cls) == 123);
}

static void test_httpsrv_set_upld_dir(struct sg_httpsrv *srv) {
    ASSERT(sg_httpsrv_set_upld_dir(NULL, "foo") == EINVAL);
    ASSERT(sg_httpsrv_set_upld_dir(srv, NULL) == EINVAL);

    ASSERT(sg_httpsrv_set_upld_dir(srv, "foo") == 0);
}

static void test_httpsrv_upld_dir(struct sg_httpsrv *srv) {
    errno = 0;
    ASSERT(!sg_httpsrv_upld_dir(NULL));
    ASSERT(errno == EINVAL);

    ASSERT(sg_httpsrv_set_upld_dir(srv, "foo") == 0);
    errno = 0;
    ASSERT(strcmp(sg_httpsrv_upld_dir(srv), "foo") == 0);
    ASSERT(errno == 0);
}

static void test_httpsrv_set_post_buf_size(struct sg_httpsrv *srv) {
    ASSERT(sg_httpsrv_set_post_buf_size(NULL, 256) == EINVAL);
    ASSERT(sg_httpsrv_set_post_buf_size(srv, 255) == EINVAL);

    ASSERT(sg_httpsrv_set_post_buf_size(srv, 256) == 0);
}

static void test_httpsrv_post_buf_size(struct sg_httpsrv *srv) {
    errno = 0;
    ASSERT(sg_httpsrv_post_buf_size(NULL) == 0);
    ASSERT(errno == EINVAL);

    ASSERT(sg_httpsrv_set_post_buf_size(srv, 256) == 0);
    errno = 0;
    ASSERT(sg_httpsrv_post_buf_size(srv) == 256);
    ASSERT(errno == 0);
}

static void test_httpsrv_set_payld_limit(struct sg_httpsrv *srv) {
    ASSERT(sg_httpsrv_set_payld_limit(NULL, 123) == EINVAL);

    ASSERT(sg_httpsrv_set_payld_limit(srv, 0) == 0);
    ASSERT(sg_httpsrv_set_payld_limit(srv, 123) == 0);
}

static void test_httpsrv_payld_limit(struct sg_httpsrv *srv) {
    errno = 0;
    ASSERT(sg_httpsrv_payld_limit(NULL) == 0);
    ASSERT(errno == EINVAL);

    ASSERT(sg_httpsrv_set_payld_limit(srv, 123) == 0);
    errno = 0;
    ASSERT(sg_httpsrv_payld_limit(srv) == 123);
    ASSERT(errno == 0);
}

static void test_httpsrv_set_uplds_limit(struct sg_httpsrv *srv) {
    ASSERT(sg_httpsrv_set_uplds_limit(NULL, 123) == EINVAL);

    ASSERT(sg_httpsrv_set_uplds_limit(srv, 0) == 0);
    ASSERT(sg_httpsrv_set_uplds_limit(srv, 123) == 0);
}

static void test_httpsrv_uplds_limit(struct sg_httpsrv *srv) {
    errno = 0;
    ASSERT(sg_httpsrv_uplds_limit(NULL) == 0);
    ASSERT(errno == EINVAL);

    ASSERT(sg_httpsrv_set_uplds_limit(srv, 123) == 0);
    errno = 0;
    ASSERT(sg_httpsrv_uplds_limit(srv) == 123);
    ASSERT(errno == 0);
}

static void test_httpsrv_set_thr_pool_size(struct sg_httpsrv *srv) {
    ASSERT(sg_httpsrv_set_thr_pool_size(NULL, 123) == EINVAL);

    ASSERT(sg_httpsrv_set_thr_pool_size(srv, 0) == 0);
    ASSERT(sg_httpsrv_set_thr_pool_size(srv, 123) == 0);
}

static void test_httpsrv_thr_pool_size(struct sg_httpsrv *srv) {
    errno = 0;
    ASSERT(sg_httpsrv_thr_pool_size(NULL) == 0);
    ASSERT(errno == EINVAL);

    ASSERT(sg_httpsrv_set_thr_pool_size(srv, 123) == 0);
    errno = 0;
    ASSERT(sg_httpsrv_thr_pool_size(srv) == 123);
    ASSERT(errno == 0);
}

static void test_httpsrv_set_con_timeout(struct sg_httpsrv *srv) {
    ASSERT(sg_httpsrv_set_con_timeout(NULL, 123) == EINVAL);

    ASSERT(sg_httpsrv_set_con_timeout(srv, 0) == 0);
    ASSERT(sg_httpsrv_set_con_timeout(srv, 123) == 0);
}

static void test_httpsrv_con_timeout(struct sg_httpsrv *srv) {
    errno = 0;
    ASSERT(sg_httpsrv_con_timeout(NULL) == 0);
    ASSERT(errno == EINVAL);

    ASSERT(sg_httpsrv_set_con_timeout(srv, 123) == 0);
    errno = 0;
    ASSERT(sg_httpsrv_con_timeout(srv) == 123);
    ASSERT(errno == 0);
}

static void test_httpsrv_set_con_limit(struct sg_httpsrv *srv) {
    ASSERT(sg_httpsrv_set_con_limit(NULL, 123) == EINVAL);

    ASSERT(sg_httpsrv_set_con_limit(srv, 0) == 0);
    ASSERT(sg_httpsrv_set_con_limit(srv, 123) == 0);
}

static void test_httpsrv_con_limit(struct sg_httpsrv *srv) {
    errno = 0;
    ASSERT(sg_httpsrv_con_limit(NULL) == 0);
    ASSERT(errno == EINVAL);

    ASSERT(sg_httpsrv_set_con_limit(srv, 123) == 0);
    errno = 0;
    ASSERT(sg_httpsrv_con_limit(srv) == 123);
    ASSERT(errno == 0);
}

int main(void) {
    struct sg_httpsrv *srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
    /* test__httperr_cb() */
    test__httpsrv_oel("%s%d", "abc", 123);
    test__httpsrv_ahc(srv);
    test__httpsrv_rcc();
    test__httpsrv_addopt();
    test_httpsrv_new2();
    test_httpsrv_new();
    test_httpsrv_free();
    test_httpsrv_listen(srv);
#ifdef SG_HTTPS_SUPPORT
    test_httpsrv_tls_listen(srv);
    test_httpsrv_tls_listen2(srv);
#endif
    test_httpsrv_shutdown(srv);
    test_httpsrv_port(srv);
    test_httpsrv_is_threaded(srv);
    test__httpsrv_set_upld_cbs(srv);
    test_httpsrv_set_upld_dir(srv);
    test_httpsrv_upld_dir(srv);
    test_httpsrv_set_post_buf_size(srv);
    test_httpsrv_post_buf_size(srv);
    test_httpsrv_set_payld_limit(srv);
    test_httpsrv_payld_limit(srv);
    test_httpsrv_set_uplds_limit(srv);
    test_httpsrv_uplds_limit(srv);
    test_httpsrv_set_thr_pool_size(srv);
    test_httpsrv_thr_pool_size(srv);
    test_httpsrv_set_con_timeout(srv);
    test_httpsrv_con_timeout(srv);
    test_httpsrv_set_con_limit(srv);
    test_httpsrv_con_limit(srv);
    sg_httpsrv_free(srv);
    return EXIT_SUCCESS;
}
