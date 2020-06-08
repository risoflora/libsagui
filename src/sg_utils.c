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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include "sg_macros.h"
#ifdef _WIN32
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#include <wchar.h>
#else /* _WIN32 */
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif /* HAVE_ARPA_INET_H */
#endif /* _WIN32 */
#ifndef HAVE_INET_NTOP
#include "inet.h"
#endif /* HAVE_INET_NTOP */
#include "sagui.h"
#include "sg_utils.h"

#define SG__VERSION_STR                                                        \
  xstr(SG_VERSION_MAJOR) "." xstr(SG_VERSION_MINOR) "." xstr(SG_VERSION_PATCH)

/* Platform. */

#ifdef _WIN32

char *strndup(const char *s, size_t n) {
  char *cpy;
  size_t len = strlen(s);
  if (n < len)
    len = n;
  cpy = sg_malloc(len + 1);
  if (!cpy)
    return NULL;
  cpy[len] = '\0';
  return memcpy(cpy, s, len);
}

/* Maps a character string to a UTF-16 (wide character) string. */
static wchar_t *stow(const char *str) {
  wchar_t *res = NULL;
  int len;
  if (str) {
    len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, NULL, 0);
    if (len > 0) {
      res = sg_malloc(len * sizeof(wchar_t));
      if (res) {
        if (MultiByteToWideChar(CP_UTF8, 0, str, -1, res, len) == 0) {
          sg_free(res);
          return NULL;
        }
      }
    }
  }
  return res;
}

/* Maps a UTF-16 (wide character) string to a character string. */
static char *wtos(const wchar_t *str) {
  char *res = NULL;
  int len;
  if (str) {
    len = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    if (len > 0) {
      res = sg_malloc(len * sizeof(wchar_t));
      if (res) {
        if (WideCharToMultiByte(CP_UTF8, 0, str, -1, res, len, NULL, FALSE) ==
            0) {
          sg_free(res);
          return NULL;
        }
      }
    }
  }
  return res;
}

int sg__rename(const char *old, const char *new) {
  int ret = 0;
  wchar_t *o, *n;
  if (*old == '\0' || *new == '\0') {
    errno = ENOENT;
    return -1;
  }
  o = stow(old);
  if (!o) {
    errno = ENOMEM;
    return -1;
  }
  n = stow(new);
  if (!n) {
    sg_free(o);
    errno = ENOMEM;
    return -1;
  }
  ret = _wrename(o, n);
  sg_free(n);
  sg_free(o);
  return ret;
}

#endif /* _WIN32 */

#if defined(_WIN32) || defined(__ANDROID__) ||                                 \
  (defined(__linux__) && !defined(__gnu_linux__))

char *sg__basename(const char *path) {
  char *s1 = strrchr(path, '/');
  char *s2 = strrchr(path, '\\');
  if (s1 && s2)
    path = (s1 > s2) ? s1 + 1 : s2 + 1;
  else if (s1)
    path = s1 + 1;
  else if (s2)
    path = s2 + 1;
  return (char *) path;
}

#endif /* _WIN32 || __ANDROID__ || (__linux__ && !__gnu_linux__) */

char *sg__strdup(const char *str) {
  return str ? strdup(str) : NULL;
}

void sg__toasciilower(char *str) {
  while (*str) {
    if (/*isascii(*str) &&*/ isupper(*str))
      *str = (char) tolower(*str);
    str++;
  }
}

char *sg__strjoin(char sep, const char *a, const char *b) {
  char *str;
  size_t len;
  if (!sep || !a || !b)
    return NULL;
  len = strlen(a);
  if ((len == 0) || (a[len - 1] == sep)) {
    len += strlen(b) + 1;
    str = sg_malloc(len);
    if (!str)
      return NULL;
    snprintf(str, len, "%s%s", a, b);
  } else if (strlen(b) == 0) {
    len += 1;
    str = sg_malloc(len);
    if (!str)
      return NULL;
    memcpy(str, a, len);
  } else {
    len += 1 + strlen(b) + 1;
    str = sg_malloc(len);
    if (!str)
      return NULL;
    snprintf(str, len, "%s%c%s", a, sep, b);
  }
  str[len - 1] = '\0'; /* Null terminate, just to be safe. */
  return str;
}

bool sg__is_cookie_name(const char *name) {
  while (*name) {
    if ((*name != '_') && !isalnum(*name))
      return false;
    name++;
  }
  return true;
}

bool sg__is_cookie_val(const char *val) {
  while (*val) {
    if (!isascii(*val) || iscntrl(*val))
      return false;
    val++;
  }
  return true;
}

void sg__err_cb(__SG_UNUSED void *cls, const char *err) {
  if (isatty(fileno(stderr)) && (fprintf(stderr, "%s", err) > 0))
    fflush(stderr);
}

/* Version. */

unsigned int sg_version(void) {
  return SG_VERSION_HEX;
}

const char *sg_version_str(void) {
  return SG__VERSION_STR;
}

/* Memory. */

static struct sg__memory_manager sg__mm = {malloc, realloc, free};

int sg_mm_set(sg_malloc_func malloc_func, sg_realloc_func realloc_func,
              sg_free_func free_func) {
  if (!malloc_func || !realloc_func || !free_func)
    return EINVAL;
  sg__mm.malloc = malloc_func;
  sg__mm.realloc = realloc_func;
  sg__mm.free = free_func;
  return 0;
}

void *sg_malloc(size_t size) {
  return sg__mm.malloc(size);
}

void *sg_alloc(size_t size) {
  void *ptr;
  ptr = sg__mm.malloc(size);
  if (ptr)
    memset(ptr, 0, size);
  return ptr;
}

void *sg_realloc(void *ptr, size_t size) {
  return sg__mm.realloc(ptr, size);
}

void sg_free(void *ptr) {
  sg__mm.free(ptr);
}

/* Math. */

static double sg__expr_pow(double x, double y) {
  (void) x;
  (void) y;
  return 0;
}

static double sg__expr_fmod(double x, double y) {
  (void) x;
  (void) y;
  return 0;
}

static struct sg__math_manager sg__math = {sg__expr_pow, sg__expr_fmod};

int sg_math_set(sg_pow_func pow_func, sg_fmod_func fmod_func) {
  if (!pow_func || !fmod_func)
    return EINVAL;
  sg__math.pow = pow_func;
  sg__math.fmod = fmod_func;
  return 0;
}

double sg__pow(double x, double y) {
  return sg__math.pow(x, y);
}

double sg__fmod(double x, double y) {
  return sg__math.fmod(x, y);
}

/* String. */

char *sg_strerror(int errnum, char *errmsg, size_t errlen) {
#if defined(_WIN32) || defined(__ANDROID__) ||                                 \
  (defined(__linux__) && !defined(__gnu_linux__))
  int saved_errno;
#else /* _WIN32 || __ANDROID__ || (__linux__ && !__gnu_linux__) */
  char *res;
#endif /* _WIN32 || __ANDROID__ || (__linux__ && !__gnu_linux__) */
  if (!errmsg || errlen < 1)
    return NULL;
#if defined(_WIN32) || defined(__ANDROID__) ||                                 \
  (defined(__linux__) && !defined(__gnu_linux__))
  saved_errno = errno;
#else /* _WIN32 || __ANDROID__ || (__linux__ && !__gnu_linux__) */
  res = strerror_r(errnum, errmsg, errlen - 1);
  memcpy(errmsg, res, errlen - 1);
  errmsg[errlen - 1] = '\0';
  return errmsg;
#endif /* _WIN32 || __ANDROID__ || (__linux__ && !__gnu_linux__) */
#ifdef _WIN32
  errnum = strerror_s(errmsg, errlen, errnum);
  errno = saved_errno;
  if ((errnum != 0) && (errnum != EINVAL))
    return "?";
  return errmsg;
#endif /* _WIN32 */
#if defined(__ANDROID__) || (defined(__linux__) && !defined(__gnu_linux__))
  errnum = strerror_r(errnum, errmsg, errlen);
  errno = saved_errno;
  if ((errnum != 0) && (errnum != EINVAL) && (errnum != ERANGE))
    return "?";
  return errmsg;
#endif /* __ANDROID__ || (__linux__ && !__gnu_linux__) */
}

bool sg_is_post(const char *method) {
  const char *mts[] = {"POST", "PUT", "DELETE", "OPTIONS", NULL};
  const char **mt;
  if (!method) {
    errno = EINVAL;
    return false;
  }
  for (mt = mts; *mt; mt++)
    if (strcmp(method, *mt) == 0)
      return true;
  return false;
}

char *sg_extract_entrypoint(const char *path) {
  char *str;
  size_t len, start, end;
  if (!path) {
    errno = EINVAL;
    return NULL;
  }
  len = strlen(path);
  start = 0;
  while (*(path + start) == '/')
    start++;
  for (end = start; end < len; end++)
    if (*(path + end) == '/')
      break;
  len = (end - start) + 1;
  str = sg_malloc(len + 1);
  if (!str) {
    errno = ENOMEM;
    return NULL;
  }
  snprintf(str, len + 1, "/%s", start + path);
  str[len] = '\0';
  return str;
}

/* File/directory. */

char *sg_tmpdir() {
#ifdef _WIN32
  wchar_t path[MAX_PATH + 1]; /* max 261 */
  size_t len = GetTempPathW(MAX_PATH + 1, path);
  if (!len)
    return NULL;
  if (path[len - 1] == L'\\' && !(len == 3 && path[1] == L':')) {
    len--;
    path[len] = L'\0';
  }
  return wtos(path);
#else /* _WIN32 */
  char *buf;
  size_t len;
#define SG__TMPDIR_TRY_ENV(name)                                               \
  do {                                                                         \
    buf = getenv(name);                                                        \
    if (buf)                                                                   \
      goto done;                                                               \
  } while (0)
  SG__TMPDIR_TRY_ENV("TMPDIR");
  SG__TMPDIR_TRY_ENV("TMP");
  SG__TMPDIR_TRY_ENV("TEMP");
  SG__TMPDIR_TRY_ENV("TEMPDIR");
#undef SG__TMPDIR_TRY_ENV
#ifdef __ANDROID__
  buf = "/data/local/tmp";
#else /* __ANDROID__ */
  buf = "/tmp";
#endif /* __ANDROID__ */
done:
  buf = strdup(buf);
  if (!buf)
    return NULL;
  len = strlen(buf);
  if (len > 1 && buf[len - 1] == '/')
    len--;
  buf[len] = '\0';
  return buf;
#endif /* _WIN32 */
}

/* Sockets */

int sg_ip(const void *socket, char *buf, size_t size) {
  const struct sockaddr *sa;
  const struct in6_addr *addr6;
  size_t len;
  if (!socket || !buf || (ssize_t) size < 0)
    return EINVAL;
  sa = socket;
  if (sa->sa_family == AF_INET6) {
    addr6 = &(((struct sockaddr_in6 *) sa)->sin6_addr);
    if (!inet_ntop(AF_INET6, addr6, buf, size))
      return errno;
    len = strlen("::ffff:");
    if (IN6_IS_ADDR_V4MAPPED(addr6) && (size > len))
      memcpy(buf, buf + len, strlen(buf + len) + 1);
    return 0;
  }
  if (!inet_ntop(AF_INET, &(((struct sockaddr_in *) sa)->sin_addr), buf, size))
    return errno;
  return 0;
}
