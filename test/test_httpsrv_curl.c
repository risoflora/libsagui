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
#include <sagui.h>

#define CURL_LOG(e)                                                     \
do {                                                                    \
    if ((e) != CURLE_OK) {                                              \
        fprintf(stderr, "CURL ERROR: %s\n", curl_easy_strerror((e)));   \
        fflush(stderr);                                                 \
    }                                                                   \
} while (0)

#ifndef TEST_HTTPSRV_CURL_PORT
#define TEST_HTTPSRV_CURL_PORT 8080
#endif

#ifndef TEST_HTTPSRV_CURL_BASE_PATH
#ifdef __ANDROID__
#define TEST_HTTPSRV_CURL_BASE_PATH SG_ANDROID_TESTS_DEST_DIR "/"
#else
#define TEST_HTTPSRV_CURL_BASE_PATH ""
#endif
#endif

#define OK_MSG "libsagui [OK]"
#define ERROR_MSG "libsagui [ERROR]"
#define DENIED_MSG "Denied"

static bool strmatch(const char *s1, const char *s2) {
    if (!s1 || !s2)
        return false;
    return strcmp(s1, s2) == 0;
}

static ssize_t sg_httpres_sendstream_read_cb(void *handle, uint64_t offset, char *buf, size_t size) {
    size_t read_size;
    (void) offset;
    ASSERT((read_size = fread(buf, 1, size, handle)) == size);
    return read_size;
}

static void sg_httpres_sendstream_free_cb(void *handle) {
    ASSERT(fclose(handle) == 0);
}

static bool srv_auth_cb(__SG_UNUSED void *cls, struct sg_httpauth *auth, struct sg_httpreq *req,
                        __SG_UNUSED struct sg_httpres *res) {
    char *data = strdup("abc123");
    bool pass;
    ASSERT(sg_httpauth_set_realm(auth, "My realm") == 0);
    ASSERT(sg_httpreq_set_user_data(req, data) == 0);
    if (!(pass = strmatch(sg_httpauth_usr(auth), "foo") && strmatch(sg_httpauth_pwd(auth), "bar"))) {
        sg_free(data);
        ASSERT(sg_httpauth_deny(auth, DENIED_MSG, "text/plain") == 0);
    }
    return pass;
}

static void srv_err_cb(__SG_UNUSED void *cls, const char *err) {
    fprintf(stderr, "%s", err);
    fflush(stderr);
}

static void srv_req_cb(__SG_UNUSED void *cls, struct sg_httpreq *req, struct sg_httpres *res) {
    const char *filename1 = TEST_HTTPSRV_CURL_BASE_PATH "foo_uploaded.txt";
    const char *filename2 = TEST_HTTPSRV_CURL_BASE_PATH "bar_uploaded.txt";
    const size_t len = 3;
    struct sg_strmap **headers;
    struct sg_strmap **params;
    struct sg_strmap **cookies;
    struct sg_strmap **fields;
    struct sg_str *payload;
    struct sg_httpupld *upld;
    FILE *tmp_file;
    char filename[PATH_MAX];
    char text[4];
    char *data;

    ASSERT(data = sg_httpreq_user_data(req));
    ASSERT(strcmp(data, "abc123") == 0);
    sg_free(data);
    ASSERT(strcmp(sg_httpreq_version(req), "HTTP/1.1") == 0);

    if (strcmp(sg_httpreq_path(req), "/") == 0) {
        ASSERT(strcmp(sg_httpreq_method(req), "GET") == 0);
        ASSERT(!sg_httpreq_is_uploading(req));
        ASSERT(*(headers = sg_httpreq_headers(req)));
        ASSERT(*(cookies = sg_httpreq_cookies(req)));
        ASSERT(*(params = sg_httpreq_params(req)));
        ASSERT(strcmp(sg_strmap_get(*headers, "header1"), "header-value1") == 0);
        ASSERT(strcmp(sg_strmap_get(*headers, "header2"), "header-value2") == 0);
        ASSERT(strcmp(sg_strmap_get(*cookies, "cookie1"), "cookie-value1") == 0);
        ASSERT(strcmp(sg_strmap_get(*cookies, "cookie2"), "cookie-value2") == 0);
        ASSERT(strcmp(sg_strmap_get(*params, "param1"), "param-value1") == 0);
        ASSERT(strcmp(sg_strmap_get(*params, "param2"), "param-value2") == 0);
        sg_httpres_send(res, OK_MSG, "text/plain", 200);
        return;
    }

    if (strcmp(sg_httpreq_path(req), "/form") == 0) {
        ASSERT(strcmp(sg_httpreq_method(req), "POST") == 0);
        ASSERT(sg_httpreq_is_uploading(req));
        ASSERT(*(fields = sg_httpreq_fields(req)));
        ASSERT(strcmp(sg_strmap_get(*fields, "field1"), "field-value1") == 0);
        ASSERT(strcmp(sg_strmap_get(*fields, "field2"), "field-value2") == 0);
        sg_httpres_send(res, OK_MSG, "text/plain", 200);
        return;
    }

    if (strcmp(sg_httpreq_path(req), "/payload") == 0) {
        ASSERT(strcmp(sg_httpreq_method(req), "POST") == 0);
        ASSERT(sg_httpreq_is_uploading(req));
        ASSERT(payload = sg_httpreq_payload(req));
        ASSERT(strcmp(sg_str_content(payload), "{\"foo\":\"bar\"}") == 0);
        sg_httpres_send(res, OK_MSG, "text/plain", 200);
        return;
    }

    if (strcmp(sg_httpreq_path(req), "/upload") == 0) {
        ASSERT(strcmp(sg_httpreq_method(req), "POST") == 0);
        ASSERT(sg_httpreq_is_uploading(req));
        ASSERT(*(fields = sg_httpreq_fields(req)));
        ASSERT(strcmp(sg_strmap_get(*fields, "form-field1"), "form-field-value1") == 0);
        ASSERT(strcmp(sg_strmap_get(*fields, "form-field2"), "form-field-value2") == 0);
        unlink(filename1);
        ASSERT(access(filename1, F_OK) == -1);
        unlink(filename2);
        ASSERT(access(filename2, F_OK) == -1);
        upld = sg_httpreq_uploads(req);
        while (upld) {
            if (strcmp(sg_httpupld_name(upld), "foo.txt") == 0) {
                ASSERT(sg_httpupld_save_as(upld, filename1, true) == 0);
                ASSERT(access(filename1, F_OK) == 0);
                ASSERT(tmp_file = fopen(filename1, "r"));
                memset(text, 0, sizeof(text));
                ASSERT(fread(text, 1, len, tmp_file) == len);
                ASSERT(fclose(tmp_file) == 0);
                ASSERT(strcmp(text, "foo") == 0);
            } else if (strcmp(sg_httpupld_name(upld), "bar.txt") == 0) {
                ASSERT(sg_httpupld_save_as(upld, filename2, true) == 0);
                ASSERT(access(filename2, F_OK) == 0);
                ASSERT(tmp_file = fopen(filename2, "r"));
                memset(text, 0, sizeof(text));
                ASSERT(fread(text, 1, len, tmp_file) == len);
                ASSERT(fclose(tmp_file) == 0);
                ASSERT(strcmp(text, "bar") == 0);
            }
            sg_httpuplds_next(&upld);
        }
        sg_httpres_send(res, OK_MSG, "text/plain", 200);
        return;
    }

    if (strcmp(sg_httpreq_path(req), "/download") == 0) {
        ASSERT(strcmp(sg_httpreq_method(req), "GET") == 0);
        snprintf(filename, sizeof(filename), "%s", sg_strmap_get(*sg_httpreq_params(req), "filename"));
        ASSERT(sg_httpres_sendfile(res, 4096, 0, filename, false, 200) == 0);
        return;
    }

    if (strcmp(sg_httpreq_path(req), "/data") == 0) {
        ASSERT(strcmp(sg_httpreq_method(req), "GET") == 0);
        memset(text, 0, sizeof(text));
        snprintf(text, sizeof(text), "abc");
        ASSERT(sg_httpres_sendbinary(res, text, len, "text/plain", 200) == 0);
        return;
    }

    if (strcmp(sg_httpreq_path(req), "/stream") == 0) {
        ASSERT(strcmp(sg_httpreq_method(req), "GET") == 0);
        ASSERT(access(filename1, F_OK) == 0);
        ASSERT(tmp_file = fopen(filename1, "r"));
        sg_httpres_sendstream(res, len, 256,
                              sg_httpres_sendstream_read_cb, tmp_file, sg_httpres_sendstream_free_cb, 200);
        return;
    }

    sg_httpres_send(res, ERROR_MSG, "text/plain", 500);
}

static size_t curl_write_func(void *ptr, size_t size, size_t nmemb, struct sg_str *res) {
    sg_str_write(res, ptr, size * nmemb);
    return size * nmemb;
}

int main(void) {
    const char *filename1 = TEST_HTTPSRV_CURL_BASE_PATH "foo.txt";
    const char *filename2 = TEST_HTTPSRV_CURL_BASE_PATH "bar.txt";
    const size_t len = 3;
    struct sg_httpsrv *srv;
    CURLcode *curl;
    CURLcode ret;
    struct sg_str *res;
    struct curl_slist *headers;
    curl_mime *form;
    curl_mimepart *field;
    FILE *tmp_file;
    char url[100];
    char text[4];
    long status;

    curl_global_init(CURL_GLOBAL_ALL);

    ASSERT(srv = sg_httpsrv_new2(srv_auth_cb, NULL, srv_req_cb, NULL, srv_err_cb, NULL));
    ASSERT(curl = curl_easy_init());
    ASSERT(res = sg_str_new());

    ASSERT(sg_httpsrv_listen(srv, TEST_HTTPSRV_CURL_PORT, false));

    ASSERT(curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1) == CURLE_OK);
    snprintf(url, sizeof(url), "http://localhost:%d?param1=param-value1&param2=param-value2", TEST_HTTPSRV_CURL_PORT);
    ASSERT(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK);
    ASSERT(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func) == CURLE_OK);
    ASSERT(curl_easy_setopt(curl, CURLOPT_WRITEDATA, res) == CURLE_OK);
    ASSERT(headers = curl_slist_append(NULL, "header1: header-value1"));
    ASSERT(headers = curl_slist_append(headers, "header2: header-value2"));
    ASSERT(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, (struct curl_slist *) headers) == CURLE_OK);
    ASSERT(curl_easy_setopt(curl, CURLOPT_COOKIE, "cookie1=cookie-value1; cookie2=cookie-value2;") == CURLE_OK);

    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 401);
    ASSERT(strcmp(sg_str_content(res), DENIED_MSG) == 0);

    ASSERT(curl_easy_setopt(curl, CURLOPT_USERPWD, "wrong:pass") == CURLE_OK);
    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 401);
    ASSERT(strcmp(sg_str_content(res), DENIED_MSG) == 0);

    ASSERT(curl_easy_setopt(curl, CURLOPT_USERPWD, "foo:bar") == CURLE_OK);
    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 200);
    ASSERT(strcmp(sg_str_content(res), OK_MSG) == 0);

    snprintf(url, sizeof(url), "http://localhost:%d/wrong", TEST_HTTPSRV_CURL_PORT);
    ASSERT(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK);
    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 500);
    ASSERT(strcmp(sg_str_content(res), ERROR_MSG) == 0);

    snprintf(url, sizeof(url), "http://localhost:%d/form", TEST_HTTPSRV_CURL_PORT);
    ASSERT(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK);
    ASSERT(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "field1=field-value1&field2=field-value2") == CURLE_OK);
    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 200);
    ASSERT(strcmp(sg_str_content(res), OK_MSG) == 0);

    snprintf(url, sizeof(url), "http://localhost:%d/payload", TEST_HTTPSRV_CURL_PORT);
    ASSERT(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK);
    curl_slist_free_all(headers);
    ASSERT(headers = curl_slist_append(NULL, "Content-Type: application/json"));
    ASSERT(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, (struct curl_slist *) headers) == CURLE_OK);
    ASSERT(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"foo\":\"bar\"}") == CURLE_OK);

    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 200);
    ASSERT(strcmp(sg_str_content(res), OK_MSG) == 0);

    snprintf(url, sizeof(url), "http://localhost:%d/upload", TEST_HTTPSRV_CURL_PORT);
    ASSERT(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK);
    ASSERT(form = curl_mime_init(curl));
    ASSERT(field = curl_mime_addpart(form));
    ASSERT(curl_mime_name(field, "form-field1") == CURLE_OK);
    ASSERT(curl_mime_data(field, "form-field-value1", CURL_ZERO_TERMINATED) == CURLE_OK);
    ASSERT(field = curl_mime_addpart(form));
    ASSERT(curl_mime_name(field, "form-field2") == CURLE_OK);
    ASSERT(curl_mime_data(field, "form-field-value2", CURL_ZERO_TERMINATED) == CURLE_OK);

    unlink(filename1);
    ASSERT(access(filename1, F_OK) == -1);
    ASSERT(tmp_file = fopen(filename1, "w"));
    memset(text, 0, sizeof(text));
    snprintf(text, sizeof(text), "foo");
    ASSERT(fwrite(text, 1, len, tmp_file) == len);
    ASSERT(fclose(tmp_file) == 0);
    ASSERT(access(filename1, F_OK) == 0);

    unlink(filename2);
    ASSERT(access(filename2, F_OK) == -1);
    ASSERT(tmp_file = fopen(filename2, "w"));
    memset(text, 0, sizeof(text));
    snprintf(text, sizeof(text), "bar");
    ASSERT(fwrite(text, 1, len, tmp_file) == len);
    ASSERT(fclose(tmp_file) == 0);
    ASSERT(access(filename2, F_OK) == 0);

    ASSERT(field = curl_mime_addpart(form));
    ASSERT(curl_mime_name(field, "file1") == CURLE_OK);
    ASSERT(curl_mime_filedata(field, filename1) == CURLE_OK);
    ASSERT(field = curl_mime_addpart(form));
    ASSERT(curl_mime_name(field, "file2") == CURLE_OK);
    ASSERT(curl_mime_filedata(field, filename2) == CURLE_OK);

    ASSERT(curl_easy_setopt(curl, CURLOPT_MIMEPOST, form) == CURLE_OK);
    curl_slist_free_all(headers);
    ASSERT(headers = curl_slist_append(NULL, "Content-Type: multipart/form-data"));
    ASSERT(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, (struct curl_slist *) headers) == CURLE_OK);

    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 200);
    ASSERT(strcmp(sg_str_content(res), OK_MSG) == 0);
    curl_mime_free(form);

    curl_easy_reset(curl);
    ASSERT(curl_easy_setopt(curl, CURLOPT_USERPWD, "foo:bar") == CURLE_OK);
    snprintf(url, sizeof(url), "http://localhost:%d/download?filename=%s", TEST_HTTPSRV_CURL_PORT, filename1);
    ASSERT(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK);
    ASSERT(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func) == CURLE_OK);
    ASSERT(curl_easy_setopt(curl, CURLOPT_WRITEDATA, res) == CURLE_OK);

    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 200);
    ASSERT(strcmp(sg_str_content(res), "foo") == 0);

    snprintf(url, sizeof(url), "http://localhost:%d/download?filename=%s", TEST_HTTPSRV_CURL_PORT, filename2);
    ASSERT(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK);

    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 200);
    ASSERT(strcmp(sg_str_content(res), "bar") == 0);

    snprintf(url, sizeof(url), "http://localhost:%d/data", TEST_HTTPSRV_CURL_PORT);
    ASSERT(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK);

    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 200);
    ASSERT(strcmp(sg_str_content(res), "abc") == 0);

    snprintf(url, sizeof(url), "http://localhost:%d/stream", TEST_HTTPSRV_CURL_PORT);
    ASSERT(curl_easy_setopt(curl, CURLOPT_URL, url) == CURLE_OK);

    ASSERT(sg_str_clear(res) == 0);
    ret = curl_easy_perform(curl);
    CURL_LOG(ret);
    ASSERT(ret == CURLE_OK);
    ASSERT(curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status) == CURLE_OK);
    ASSERT(status == 200);
    ASSERT(strcmp(sg_str_content(res), "foo") == 0);

    ASSERT(sg_httpsrv_shutdown(srv) == 0);

    curl_slist_free_all(headers);
    sg_str_free(res);
    curl_easy_cleanup(curl);

    curl_global_cleanup();

    sg_httpsrv_free(srv);
    return EXIT_SUCCESS;
}
