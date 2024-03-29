/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2024 Silvio Clecio <silvioprog@gmail.com>
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

#define SG_EXTERN

#include "sg_assert.h"

#include <string.h>
#include <fcntl.h>
#include <sagui.h>
#include "sg_utils.h"
#include "sg_httpuplds.c"

#ifndef TEST_HTTPUPLDS_BASE_PATH
#ifdef __ANDROID__
#define TEST_HTTPUPLDS_BASE_PATH SG_ANDROID_TESTS_DEST_DIR "/"
#else /* __ANDROID__ */
#ifdef _WIN32
#define TEST_HTTPUPLDS_BASE_PATH BINARY_DIR "/"
#else /* _WIN32 */
#define TEST_HTTPUPLDS_BASE_PATH "/tmp/"
#endif /* _WIN32 */
#endif /* __ANDROID__ */
#endif /* TEST_HTTPUPLDS_BASE_PATH */

#ifndef TEST_HTTPUPLDS_OPEN_MODE
#define TEST_HTTPUPLDS_OPEN_MODE                                               \
  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#endif /* TEST_HTTPUPLDS_OPEN_MODE */

#ifndef TEST_HTTPUPLDS_OPEN_WFLAGS
#define TEST_HTTPUPLDS_OPEN_WFLAGS O_RDWR | O_CREAT | O_TRUNC
#endif /* TEST_HTTPUPLDS_OPEN_WFLAGS */

static int dummy_httpuplds_save_cb(void *handle, bool overwritten) {
  (void) handle;
  (void) overwritten;
  return 123;
}

static int dummy_httpuplds_save_as_cb(void *handle, const char *path,
                                      bool overwritten) {
  (void) handle;
  (void) path;
  (void) overwritten;
  return 123;
}

static int dummy_httpuplds_iter_cb(void *cls, struct sg_httpupld *upld) {
  const char *str = cls;
  if (strcmp(upld->name, "foo") == 0 || strcmp(upld->name, "bar") == 0) {
    sprintf(cls, "%s%s", str, upld->name);
    return 0;
  }
  return 123;
}

static void dummy_httpreq_cb(void *cls, struct sg_httpreq *req,
                             struct sg_httpres *res) {
  (void) cls;
  (void) req;
  (void) res;
}

static void dummy_err_cb(void *cls, const char *err) {
  strcpy(cls, err);
}

static int empty_httpupld_cb(void *cls, void **handle, const char *dir,
                             const char *field, const char *name,
                             const char *mime, const char *encoding) {
  (void) cls;
  (void) handle;
  (void) dir;
  (void) field;
  (void) name;
  (void) mime;
  (void) encoding;
  return -1;
}

static ssize_t empty_httpupld_write_cb(void *handle, uint64_t offset,
                                       const char *buf, size_t size) {
  (void) handle;
  (void) offset;
  (void) buf;
  (void) size;
  return -1;
}

static void test__httpuplds_add(struct MHD_Connection *con) {
  char err[256];
  struct sg_httpsrv *srv =
    sg_httpsrv_new2(NULL, dummy_httpreq_cb, dummy_err_cb, err);
  struct sg_httpreq *req = sg__httpreq_new(srv, con, "", "", "");
  ASSERT(sg_httpuplds_count(sg_httpreq_uploads(req)) == 0);
  sg__httpuplds_add(srv, req, "abc", "def", "ghi", "jkl");
  ASSERT(strcmp(sg_httpupld_field(req->curr_upld), "abc") == 0);
  ASSERT(strcmp(sg_httpupld_name(req->curr_upld), "def") == 0);
  ASSERT(strcmp(sg_httpupld_mime(req->curr_upld), "ghi") == 0);
  ASSERT(strcmp(sg_httpupld_encoding(req->curr_upld), "jkl") == 0);
  ASSERT(sg_httpuplds_count(sg_httpreq_uploads(req)) == 1);
  sg__httpuplds_free(srv, req);
  sg__httpreq_free(req);
  sg_httpsrv_free(srv);
}

static void test__httpuplds_free(void) {
  sg__httpuplds_free(NULL, NULL);
}

static void test__httpuplds_iter(struct MHD_Connection *con) {
  const char *filename = "foo.txt";
  const size_t len = 3;
  char err[256], str[256];
  struct sg_httpsrv *srv =
    sg_httpsrv_new2(NULL, dummy_httpreq_cb, dummy_err_cb, err);
  struct sg_httpreq *req = sg__httpreq_new(srv, con, "", "", "");
  struct sg__httpupld_holder holder = {srv, req};
  struct sg_strmap **fields;
  sg_httpupld_cb saved_upld_cb;
  sg_write_cb saved_upld_write_cb;
  char *dir, *dest_path;

  ASSERT(sg__httpuplds_iter(NULL, MHD_POSTDATA_KIND, NULL, NULL, NULL, NULL,
                            NULL, 0, 0) == MHD_YES);

  saved_upld_cb = srv->upld_cb;
  srv->upld_cb = empty_httpupld_cb;
  ASSERT(sg__httpuplds_iter(&holder, MHD_POSTDATA_KIND, NULL, filename, NULL,
                            NULL, "foo", 0, len) == MHD_NO);
  srv->upld_cb = saved_upld_cb;

  saved_upld_write_cb = srv->upld_write_cb;
  srv->upld_write_cb = empty_httpupld_write_cb;
  ASSERT(sg__httpuplds_iter(&holder, MHD_POSTDATA_KIND, NULL, filename, NULL,
                            NULL, "foo", 0, len) == MHD_NO);
  srv->upld_write_cb = saved_upld_write_cb;

  dir = sg_tmpdir();
  ASSERT(dir);
  dest_path = sg__strjoin(PATH_SEP, dir, filename);
  sg_free(dir);
  ASSERT(dest_path);
  unlink(dest_path);
  ASSERT(access(dest_path, F_OK) == -1);
  ASSERT(sg__httpuplds_iter(&holder, MHD_POSTDATA_KIND, NULL, filename, NULL,
                            NULL, "foo", 0, len) == MHD_YES);
  ASSERT(sg_httpupld_save(holder.req->curr_upld, true) == 0);
  ASSERT(access(dest_path, F_OK) == 0);
  sg_free(dest_path);

  ASSERT(sg_httpsrv_set_uplds_limit(srv, 1) == 0);
  memset(err, 0, sizeof(err));
  ASSERT(sg__httpuplds_iter(&holder, MHD_POSTDATA_KIND, NULL, filename, NULL,
                            NULL, "foo", 0, len) == MHD_NO);
  memset(str, 0, sizeof(str));
  snprintf(str, sizeof(str), _("Upload too large.\n"));
  ASSERT(strcmp(err, str) == 0);

  fields = sg_httpreq_fields(req);
  ASSERT(!sg_strmap_get(*fields, "foo"));
  ASSERT(sg__httpuplds_iter(&holder, MHD_POSTDATA_KIND, "foo", NULL, NULL, NULL,
                            "bar", 0, len) == MHD_YES);
  ASSERT(strcmp(sg_strmap_get(*fields, "foo"), "bar") == 0);

  ASSERT(sg_httpsrv_set_payld_limit(srv, 1) == 0);
  memset(err, 0, sizeof(err));
  ASSERT(sg__httpuplds_iter(&holder, MHD_POSTDATA_KIND, "foo", NULL, NULL, NULL,
                            "bar", 0, len) == MHD_NO);
  memset(str, 0, sizeof(str));
  snprintf(str, sizeof(str), _("Payload too large.\n"));
  ASSERT(strcmp(err, str) == 0);

  sg__httpuplds_cleanup(srv, req);
  sg__httpreq_free(req);
  sg_httpsrv_free(srv);
}

static void test__httpuplds_process(struct MHD_Connection *con) {
  const size_t len = 3;
  char err[256], str[256];
  struct sg_httpsrv *srv =
    sg_httpsrv_new2(NULL, dummy_httpreq_cb, dummy_err_cb, err);
  struct sg_httpreq *req = sg__httpreq_new(srv, con, "", "", "");
  int ret = 0;
  size_t size = 0;

  ASSERT(!sg__httpuplds_process(NULL, NULL, NULL, NULL, &size, &ret));

  ASSERT(sg_httpsrv_set_payld_limit(srv, 1) == 0);
  size = len;
  ret = MHD_YES;
  memset(err, 0, sizeof(err));
  ASSERT(sg__httpuplds_process(srv, req, con, "foo", &size, &ret));
  ASSERT(ret == MHD_NO);
  memset(str, 0, sizeof(str));
  snprintf(str, sizeof(str), _("Payload too large.\n"));
  ASSERT(strcmp(err, str) == 0);

  ASSERT(sg_httpsrv_set_payld_limit(srv, len) == 0);
  ret = MHD_NO;
  ASSERT(sg__httpuplds_process(srv, req, con, "foo", &size, &ret));
  ASSERT(ret == MHD_YES);
  ASSERT(strcmp(sg_str_content(req->payload), "foo") == 0);

  sg__httpreq_free(req);
  sg_httpsrv_free(srv);
}

static void test__httpuplds_cleanup(struct MHD_Connection *con) {
  struct sg_httpsrv *srv = sg_httpsrv_new(dummy_httpreq_cb, NULL);
  struct sg_httpreq *req = sg__httpreq_new(srv, con, "", "", "");
  struct sg_httpupld *tmp;
  size_t count = 0;
  ASSERT(srv);
  ASSERT(req);

  sg__httpuplds_cleanup(srv, req);
  LL_COUNT(req->uplds, tmp, count);
  ASSERT(count == 0);

  req->curr_upld = sg_alloc(sizeof(struct sg_httpupld));
  LL_APPEND(req->uplds, req->curr_upld);
  req->curr_upld = NULL; /* fix PVS-Studio analysis */
  req->curr_upld = sg_alloc(sizeof(struct sg_httpupld));
  LL_APPEND(req->uplds, req->curr_upld);
  LL_COUNT(req->uplds, tmp, count);
  ASSERT(count == 2);

  sg__httpuplds_cleanup(srv, req);
  LL_COUNT(req->uplds, tmp, count);
  ASSERT(count == 0);

  sg__httpreq_free(req);
  sg_httpsrv_free(srv);
}

static void test__httpupld_cb(void) {
  const char *dummy_path = TEST_HTTPUPLDS_BASE_PATH "foo.txt",
             *filename = "foo.txt";
  const ssize_t len = 3;
  char err[256], str[256];
  void *handle = NULL;
  struct sg__httpupld *h;
  struct sg_httpsrv *srv;
  char *dir, *dest_path;
  int fd;
  memset(err, 0, sizeof(err));
  srv = sg_httpsrv_new2(NULL, dummy_httpreq_cb, dummy_err_cb, err);

  ASSERT(sg__httpupld_cb(srv, &handle, "", "", "", "", "") == ENOENT);
  ASSERT(!handle);
  snprintf(str, sizeof(str), _("Cannot find uploads directory \"%s\": %s.\n"),
           "", strerror(ENOENT));
  ASSERT(strcmp(err, str) == 0);

  unlink(dummy_path);
  fd = open(dummy_path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);
  ASSERT(fd > -1);
  ASSERT(close(fd) == 0);
  ASSERT(access(dummy_path, F_OK) == 0);

  memset(err, 0, sizeof(err));
  ASSERT(sg__httpupld_cb(srv, &handle, dummy_path, "", "", "", "") == ENOTDIR);
  unlink(dummy_path);
  ASSERT(!handle);
  memset(str, 0, sizeof(str));
  snprintf(str, sizeof(str), _("Cannot access uploads directory \"%s\": %s.\n"),
           dummy_path, strerror(ENOTDIR));
  ASSERT(strcmp(err, str) == 0);
#if defined(__linux__) && !defined(__ANDROID__)
  memset(err, 0, sizeof(err));
  ASSERT(sg__httpupld_cb(srv, &handle, "/", "", "", "", "") == EACCES);
  ASSERT(!handle);
  memset(str, 0, sizeof(str));
  snprintf(str, sizeof(str),
           _("Cannot create temporary upload file in \"%s\": %s.\n"), "/",
           strerror(EACCES));
  ASSERT(strcmp(err, str) == 0);
#endif /* __linux__ && !__ANDROID__ */

  dir = sg_tmpdir();
  dest_path = sg__strjoin(PATH_SEP, dir, filename);
  ASSERT(dest_path);
  unlink(dest_path);
  ASSERT(access(dest_path, F_OK) == -1);
  ASSERT(sg__httpupld_cb(srv, &handle, dir, "foo", "foo.txt", "", "") == 0);
  sg_free(dir);
  h = handle;
  ASSERT(access(h->path, F_OK) == 0);
  ASSERT(sg__httpupld_write_cb(handle, 0, "foo", len) == len);
  ASSERT(sg__httpupld_save_cb(handle, true) == 0);
  sg__httpupld_free_cb(handle);
  ASSERT(access(dest_path, F_OK) == 0);
  fd = open(dest_path, O_RDONLY);
  sg_free(dest_path);
  ASSERT(fd > -1);
  memset(str, 0, sizeof(str));
  ASSERT(read(fd, str, len) == len);
  ASSERT(close(fd) == 0);
  ASSERT(strcmp(str, "foo") == 0);

  sg_httpsrv_free(srv);
}

static void test__httpupld_write_cb(void) {
  const ssize_t len = 3;
  char str[4];
  struct sg__httpupld *handle = sg_alloc(sizeof(struct sg__httpupld));
  handle->path = TEST_HTTPUPLDS_BASE_PATH "foo.txt";

  unlink(handle->path);

  ASSERT(access(handle->path, F_OK) == -1);
  handle->fd =
    open(handle->path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);
  ASSERT(handle->fd > -1);
  ASSERT(sg__httpupld_write_cb(handle, 0, "foo", len - 1) == (len - 1));
  ASSERT(close(handle->fd) == 0);
  ASSERT(access(handle->path, F_OK) == 0);
  handle->fd = open(handle->path, O_RDONLY);
  ASSERT(handle->fd);
  memset(str, 0, sizeof(str));
  ASSERT(read(handle->fd, str, len) == (len - 1));
  ASSERT(close(handle->fd) == 0);
  ASSERT(strcmp(str, "fo") == 0);

  sg_free(handle);
}

static void test__httpupld_free_cb(void) {
  const char *path = TEST_HTTPUPLDS_BASE_PATH "foo.txt";
  char err[256];
  struct sg_httpsrv *saved_srv;
  struct sg__httpupld *handle = sg_alloc(sizeof(struct sg__httpupld));
  memset(err, 0, sizeof(err));
  handle->srv = sg_httpsrv_new2(NULL, dummy_httpreq_cb, dummy_err_cb, err);
  handle->path = sg__strdup(path);
  handle->dest = NULL;

  unlink(handle->path);

  handle->fd =
    open(handle->path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);

  ASSERT(handle->fd > -1);
#ifdef _WIN32
  ASSERT(close(handle->fd) == 0);
#endif /* _WIN32 */
  ASSERT(unlink(handle->path) == 0);
  ASSERT(strcmp(err, "") == 0);
  saved_srv = handle->srv;
  sg__httpupld_free_cb(handle);
  sg_httpsrv_free(saved_srv);
}

static void test__httpupld_save_cb(void) {
  const ssize_t len = 3;
  char str[4];
  struct sg__httpupld *handle = sg_alloc(sizeof(struct sg__httpupld));
  handle->path = TEST_HTTPUPLDS_BASE_PATH "foo.txt";
  handle->dest = TEST_HTTPUPLDS_BASE_PATH "bar.txt";

  unlink(handle->dest);
  unlink(handle->path);

  ASSERT(sg__httpupld_save_cb(NULL, false) == EINVAL);

  handle->fd =
    open(handle->path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);
  ASSERT(handle->fd > -1);
  ASSERT(write(handle->fd, "foo", len) == len);
  ASSERT(access(handle->dest, F_OK) == -1);
  ASSERT(sg__httpupld_save_cb(handle, true) == 0);
  ASSERT(access(handle->dest, F_OK) == 0);
  handle->fd = open(handle->dest, O_RDONLY);
  ASSERT(handle->fd > -1);
  memset(str, 0, sizeof(str));
  ASSERT(read(handle->fd, str, len) == len);
  ASSERT(close(handle->fd) == 0);
  ASSERT(strcmp(str, "foo") == 0);

  sg_free(handle);
}

static void test__httpupld_save_as_cb(void) {
  const ssize_t len = 3;
  const char *bar_path = TEST_HTTPUPLDS_BASE_PATH "bar.txt";
  char str[4];
  char *dir;
  struct sg__httpupld *handle = sg_alloc(sizeof(struct sg__httpupld));
  handle->path = TEST_HTTPUPLDS_BASE_PATH "foo.txt";
  handle->fd = -1;

  unlink(bar_path);
  unlink(handle->path);

  ASSERT(sg__httpupld_save_as_cb(NULL, "foo", false) == EINVAL);
  ASSERT(sg__httpupld_save_as_cb(handle, "foo", false) == EINVAL);
  handle->fd =
    open(handle->path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);
  ASSERT(handle->fd > -1);
  ASSERT(sg__httpupld_save_as_cb(handle, NULL, false) == EINVAL);
  close(handle->fd);

  handle->fd =
    open(bar_path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);
  ASSERT(handle->fd > -1);
  ASSERT(write(handle->fd, "bar", len) == len);
  ASSERT(close(handle->fd) == 0);
  ASSERT(access(bar_path, F_OK) == 0);
  handle->fd = open(bar_path, O_RDONLY);
  ASSERT(handle->fd > -1);
  memset(str, 0, sizeof(str));
  ASSERT(read(handle->fd, str, len) == len);
  ASSERT(close(handle->fd) == 0);
  ASSERT(strcmp(str, "bar") == 0);

  handle->fd =
    open(handle->path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);
  ASSERT(handle->fd > -1);
  ASSERT(write(handle->fd, "foo", len) == len);

  ASSERT(sg__httpupld_save_as_cb(handle, bar_path, false) == EEXIST);
  ASSERT(access(bar_path, F_OK) == 0);
  handle->fd = open(bar_path, O_RDONLY);
  ASSERT(handle->fd > -1);
  memset(str, 0, sizeof(str));
  ASSERT(read(handle->fd, str, len) == len);
  ASSERT(close(handle->fd) == 0);
  ASSERT(strcmp(str, "bar") == 0);

  dir = sg_tmpdir();
  ASSERT(dir);
  ASSERT(access(dir, F_OK) == 0);
  handle->fd =
    open(handle->path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);
  ASSERT(handle->fd > -1);
  ASSERT(write(handle->fd, "foo", len) == len);
  ASSERT(sg__httpupld_save_as_cb(handle, dir, false) == EISDIR);
  sg_free(dir);

  handle->fd =
    open(handle->path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);
  ASSERT(handle->fd > -1);
  ASSERT(write(handle->fd, "foo", len) == len);
  ASSERT(sg__httpupld_save_as_cb(handle, "", false) == ENOENT);

  handle->fd =
    open(handle->path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);
  ASSERT(handle->fd > -1);
  ASSERT(write(handle->fd, "foo", len) == len);
  ASSERT(sg__httpupld_save_as_cb(handle, bar_path, true) == 0);
  ASSERT(access(bar_path, F_OK) == 0);
  handle->fd = open(bar_path, O_RDONLY);
  ASSERT(handle->fd > -1);
  memset(str, 0, sizeof(str));
  ASSERT(read(handle->fd, str, len) == len);
  ASSERT(close(handle->fd) == 0);
  ASSERT(strcmp(str, "foo") == 0);

  unlink(bar_path);
  ASSERT(access(bar_path, F_OK) == -1);
  handle->fd =
    open(handle->path, TEST_HTTPUPLDS_OPEN_WFLAGS, TEST_HTTPUPLDS_OPEN_MODE);
  ASSERT(handle->fd > -1);
  ASSERT(write(handle->fd, "foo", len) == len);
  ASSERT(sg__httpupld_save_as_cb(handle, bar_path, false) == 0);
  ASSERT(access(bar_path, F_OK) == 0);
  handle->fd = open(bar_path, O_RDONLY);
  ASSERT(handle->fd > -1);
  memset(str, 0, sizeof(str));
  ASSERT(read(handle->fd, str, len) == len);
  ASSERT(close(handle->fd) == 0);
  ASSERT(strcmp(str, "foo") == 0);

  sg_free(handle);
  unlink(bar_path);
}

static void test_httpuplds_iter(void) {
  struct sg_httpupld *tmp, *upld, *uplds = NULL;
  char str[100];
  ASSERT(sg_httpuplds_iter(uplds, NULL, str) == EINVAL);

  ASSERT(sg_httpuplds_iter(NULL, dummy_httpuplds_iter_cb, str) == 0);
  upld = sg_alloc(sizeof(struct sg_httpupld));
  upld->name = "foo";
  LL_APPEND(uplds, upld);
  upld = sg_alloc(sizeof(struct sg_httpupld));
  upld->name = "bar";
  LL_APPEND(uplds, upld);
  memset(str, 0, sizeof(str));
  ASSERT(sg_httpuplds_iter(uplds, dummy_httpuplds_iter_cb, str) == 0);
  ASSERT(strcmp(str, "foobar") == 0);
  upld = sg_alloc(sizeof(struct sg_httpupld));
  upld->name = "xxx";
  LL_APPEND(uplds, upld);
  ASSERT(sg_httpuplds_iter(uplds, dummy_httpuplds_iter_cb, str) == 123);
  LL_FOREACH_SAFE(uplds, upld, tmp) {
    LL_DELETE(uplds, upld);
    sg_free(upld);
  }
}

static void test_httpuplds_next(void) {
  struct sg_httpupld *tmp, *upld, *uplds = NULL;
  ASSERT(sg_httpuplds_next(NULL) == EINVAL);

  upld = sg_alloc(sizeof(struct sg_httpupld));
  upld->name = "abc123";
  LL_APPEND(uplds, upld);
  upld = sg_alloc(sizeof(struct sg_httpupld));
  upld->name = "def456";
  LL_APPEND(uplds, upld);
  upld = sg_alloc(sizeof(struct sg_httpupld));
  upld->name = "ghi789";
  LL_APPEND(uplds, upld);
  upld = uplds;
  ASSERT(strcmp(sg_httpupld_name(upld), "abc123") == 0);
  ASSERT(sg_httpuplds_next(&upld) == 0);
  ASSERT(strcmp(sg_httpupld_name(upld), "def456") == 0);
  ASSERT(sg_httpuplds_next(&upld) == 0);
  ASSERT(strcmp(sg_httpupld_name(upld), "ghi789") == 0);
  LL_FOREACH_SAFE(uplds, upld, tmp) {
    LL_DELETE(uplds, upld);
    sg_free(upld);
  }
}

static void test_httpuplds_count(void) {
  struct sg_httpupld *tmp, *upld, *uplds = NULL;
  errno = 0;
  ASSERT(sg_httpuplds_count(NULL) == 0);
  ASSERT(errno == 0);
  upld = sg_alloc(sizeof(struct sg_httpupld));
  LL_APPEND(uplds, upld);
  ASSERT(sg_httpuplds_count(uplds) == 1);
  upld = sg_alloc(sizeof(struct sg_httpupld));
  LL_APPEND(uplds, upld);
  ASSERT(sg_httpuplds_count(uplds) == 2);
  LL_FOREACH_SAFE(uplds, upld, tmp) {
    LL_DELETE(uplds, upld);
    sg_free(upld);
  }
}

static void test_httpupld_handle(struct sg_httpupld *upld) {
  errno = 0;
  ASSERT(sg_httpupld_handle(NULL) == 0);
  ASSERT(errno == EINVAL);

  upld->handle = NULL;
  errno = 0;
  ASSERT(!sg_httpupld_handle(upld));
  ASSERT(errno == 0);
  upld->handle = upld;
  ASSERT(sg_httpupld_handle(upld) == upld);
}

static void test_httpupld_dir(struct sg_httpupld *upld) {
  errno = 0;
  ASSERT(sg_httpupld_dir(NULL) == 0);
  ASSERT(errno == EINVAL);

  upld->dir = NULL;
  errno = 0;
  ASSERT(!sg_httpupld_dir(upld));
  ASSERT(errno == 0);
  upld->dir = "K7eFIFhFJmwSI601";
  ASSERT(strcmp(sg_httpupld_dir(upld), "K7eFIFhFJmwSI601") == 0);
}

static void test_httpupld_field(struct sg_httpupld *upld) {
  errno = 0;
  ASSERT(sg_httpupld_field(NULL) == 0);
  ASSERT(errno == EINVAL);

  upld->field = NULL;
  errno = 0;
  ASSERT(!sg_httpupld_field(upld));
  ASSERT(errno == 0);
  upld->field = "ZgJwfUrXVAHSV4pb";
  ASSERT(strcmp(sg_httpupld_field(upld), "ZgJwfUrXVAHSV4pb") == 0);
}

static void test_httpupld_name(struct sg_httpupld *upld) {
  errno = 0;
  ASSERT(sg_httpupld_name(NULL) == 0);
  ASSERT(errno == EINVAL);

  upld->name = NULL;
  errno = 0;
  ASSERT(!sg_httpupld_name(upld));
  ASSERT(errno == 0);
  upld->name = "04wSe5er0FCCI1JG";
  ASSERT(strcmp(sg_httpupld_name(upld), "04wSe5er0FCCI1JG") == 0);
}

static void test_httpupld_mime(struct sg_httpupld *upld) {
  errno = 0;
  ASSERT(sg_httpupld_mime(NULL) == 0);
  ASSERT(errno == EINVAL);

  upld->mime = NULL;
  errno = 0;
  ASSERT(!sg_httpupld_mime(upld));
  ASSERT(errno == 0);
  upld->mime = "diOx11XHmluaYegV";
  ASSERT(strcmp(sg_httpupld_mime(upld), "diOx11XHmluaYegV") == 0);
}

static void test_httpupld_encoding(struct sg_httpupld *upld) {
  errno = 0;
  ASSERT(sg_httpupld_encoding(NULL) == 0);
  ASSERT(errno == EINVAL);

  upld->encoding = NULL;
  errno = 0;
  ASSERT(!sg_httpupld_encoding(upld));
  ASSERT(errno == 0);
  upld->encoding = "uwJtxi8hSMIDMdO7";
  ASSERT(strcmp(sg_httpupld_encoding(upld), "uwJtxi8hSMIDMdO7") == 0);
}

static void test_httpupld_size(struct sg_httpupld *upld) {
  errno = 0;
  ASSERT(sg_httpupld_size(NULL) == 0);
  ASSERT(errno == EINVAL);

  upld->size = 0;
  errno = 0;
  ASSERT(sg_httpupld_size(upld) == 0);
  ASSERT(errno == 0);
  upld->size = 123;
  ASSERT(sg_httpupld_size(upld) == 123);
}

static void test_httpupld_save(struct sg_httpupld *upld) {
  ASSERT(sg_httpupld_save(NULL, false) == EINVAL);

  upld->save_cb = dummy_httpuplds_save_cb;
  ASSERT(sg_httpupld_save(upld, true) == 123);
}

static void test_httpupld_save_as(struct sg_httpupld *upld) {
  ASSERT(sg_httpupld_save_as(NULL, "", false) == EINVAL);
  ASSERT(sg_httpupld_save_as(upld, NULL, false) == EINVAL);

  upld->save_as_cb = dummy_httpuplds_save_as_cb;
  ASSERT(sg_httpupld_save_as(upld, "abc", true) == 123);
}

int main(void) {
  struct sg_httpupld *upld = sg_alloc(sizeof(struct sg_httpupld));
  struct MHD_Connection *con = sg_alloc(256);
  test__httpuplds_add(con);
  test__httpuplds_free();
  test__httpuplds_iter(con);
  test__httpuplds_process(con);
  test__httpuplds_cleanup(con);
  test__httpupld_cb();
  test__httpupld_write_cb();
  test__httpupld_free_cb();
  test__httpupld_save_cb();
  test__httpupld_save_as_cb();
  test_httpuplds_iter();
  test_httpuplds_next();
  test_httpuplds_count();
  test_httpupld_handle(upld);
  test_httpupld_dir(upld);
  test_httpupld_field(upld);
  test_httpupld_name(upld);
  test_httpupld_mime(upld);
  test_httpupld_encoding(upld);
  test_httpupld_size(upld);
  test_httpupld_save(upld);
  test_httpupld_save_as(upld);
  sg_free(upld);
  sg_free(con);
  return EXIT_SUCCESS;
}
