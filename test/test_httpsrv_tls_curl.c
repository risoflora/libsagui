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

#include "sg_assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <gnutls/gnutls.h>
#include <sagui.h>

#define CURL_LOG(e)                                                     \
do {                                                                    \
    if ((e) != CURLE_OK) {                                              \
        fprintf(stderr, "CURL ERROR: %s\n", curl_easy_strerror((e)));   \
        fflush(stderr);                                                 \
    }                                                                   \
} while (0)

#ifndef TEST_HTTPSRV_TLS_CURL_PORT
#define TEST_HTTPSRV_TLS_CURL_PORT 8080
#endif

#define OK_MSG "libsagui [OK]"

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

static void srv_req_cb(__SG_UNUSED void *cls, struct sg_httpreq *req, struct sg_httpres *res) {
    ASSERT(sg_httpreq_tls_session(req));
    ASSERT(strcmp(sg_httpreq_version(req), "HTTP/1.1") == 0);
    ASSERT(strcmp(sg_httpreq_method(req), "GET") == 0);
    sg_httpres_send(res, OK_MSG, "text/plain", 200);
}

static size_t curl_write_func(void *ptr, size_t size, size_t nmemb, struct sg_str *res) {
    sg_str_write(res, ptr, size * nmemb);
    return size * nmemb;
}

int main(void) {
    struct sg_httpsrv *srv;
    CURLcode *curl;
    CURLcode ret;
    struct sg_str *res;
    char url[100];
    long status;

    curl_global_init(CURL_GLOBAL_ALL);

    ASSERT(srv = sg_httpsrv_new(srv_req_cb, NULL));
    ASSERT(curl = curl_easy_init());
    ASSERT(res = sg_str_new());

    ASSERT(sg_httpsrv_tls_listen(srv, private_key, certificate, TEST_HTTPSRV_TLS_CURL_PORT, false));

    ASSERT(curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1) == CURLE_OK);
    snprintf(url, sizeof(url), "https://localhost:%d", TEST_HTTPSRV_TLS_CURL_PORT);
    ASSERT(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK);
    ASSERT(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func) == CURLE_OK);
    ASSERT(curl_easy_setopt(curl, CURLOPT_WRITEDATA, res) == CURLE_OK);
    ASSERT(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L) == CURLE_OK);

    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 200);
    ASSERT(strcmp(sg_str_content(res), OK_MSG) == 0);

    ASSERT(sg_httpsrv_shutdown(srv) == 0);

    sg_str_free(res);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    sg_httpsrv_free(srv);
    return EXIT_SUCCESS;
}
