/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 *   –– cross-platform library which helps to develop web servers or frameworks.
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

#ifdef _WIN32
#include <windows.h>
#include <wchar.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#ifdef SG_HTTP_COMPRESSION
#include <zlib.h>
#endif
#include "sg_macros.h"
#include "sagui.h"
#include "sg_utils.h"

#define SG_VERSION_STR xstr(SG_VERSION_MAJOR) "." xstr(SG_VERSION_MINOR) "." xstr(SG_VERSION_PATCH)

/* Platform. */

#ifdef _WIN32

char *strndup(const char *s, size_t n) {
    char *cpy;
    size_t len = strlen(s);
    if (n < len)
        len = n;
    if (!(cpy = sg__malloc(len + 1)))
        return NULL;
    cpy[len] = '\0';
    return memcpy(cpy, s, len);
}

/* Maps a character string to a UTF-16 (wide character) string. */
static wchar_t *stow(const char *str) {
    wchar_t *res = NULL;
    int len;
    if (str) {
        if ((len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, -1, NULL, 0)) > 0) {
            sg__alloc(res, len * sizeof(wchar_t));
            if (res) {
                if (MultiByteToWideChar(CP_UTF8, 0, str, -1, res, len) == 0) {
                    sg__free(res);
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
        if ((len = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL)) > 0) {
            sg__alloc(res, len * sizeof(wchar_t));
            if (res) {
                if (WideCharToMultiByte(CP_UTF8, 0, str, -1, res, len, NULL, FALSE) == 0) {
                    sg__free(res);
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
    if (!(o = stow(old)))
        return ENOMEM;
    if (!(n = stow(new))) {
        ret = ENOMEM;
        goto done;
    }
    ret = _wrename(o, n);
    sg__free(n);
done:
    sg__free(o);
    return ret;
}

#endif

#if defined(_WIN32) || defined(__ANDROID__)

char *basename(const char *path) {
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

#endif

char *sg__strdup(const char *str) {
    return str ? strdup(str) : NULL;
}

void sg__toasciilower(char *str) {
    while (*str) {
        if (/*isascii(*str) &&*/isupper(*str))
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
        if (!(str = sg__malloc(len)))
            return NULL;
        snprintf(str, len, "%s%s", a, b);
    } else if (strlen(b) == 0) {
        len += 1;
        if (!(str = sg__malloc(len)))
            return NULL;
        memcpy(str, a, len);
    } else {
        len += 1 + strlen(b) + 1;
        if (!(str = sg__malloc(len)))
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

#ifdef SG_HTTP_COMPRESSION

int sg__compress(const void *src, size_t src_size, void *dest, size_t *dest_size) {
    z_stream stream;
    const uInt max = (uInt) -1;
    uLong left;
    int errnum;
    left = *dest_size;
    *dest_size = 0;
    stream.zalloc = NULL;
    stream.zfree = NULL;
    stream.opaque = NULL;
    if ((errnum = deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL,
                               Z_DEFAULT_STRATEGY)) != Z_OK)
        return errnum;
    stream.next_out = dest;
    stream.avail_out = 0;
    stream.next_in = (z_const Bytef *) src;
    stream.avail_in = 0;
    do {
        if (stream.avail_out == 0) {
            stream.avail_out = left > (uLong) max ? max : (uInt) left;
            left -= stream.avail_out;
        }
        if (stream.avail_in == 0) {
            stream.avail_in = src_size > (uLong) max ? max : (uInt) src_size;
            src_size -= stream.avail_in;
        }
        errnum = deflate(&stream, src_size ? Z_NO_FLUSH : Z_FINISH);
    } while (errnum == Z_OK);
    *dest_size = stream.total_out;
    deflateEnd(&stream);
    return errnum == Z_STREAM_END ? Z_OK : errnum;
}

#endif

/* Version. */

unsigned int sg_version(void) {
    return SG_VERSION_HEX;
}

const char *sg_version_str(void) {
    return SG_VERSION_STR;
}

/* Memory. */

void *sg_alloc(size_t size) {
    void *ptr;
    sg__alloc2(ptr, size);
    return ptr;
}

void *sg_realloc(void *ptr, size_t size) {
    return sg__realloc(ptr, size);
}

void sg_free(void *ptr) {
    sg__free(ptr);
}

/* String. */

char *sg_strerror(int errnum, char *errmsg, size_t errlen) {
#if defined(_WIN32) || defined(__ANDROID__)
    int saved_errno;
#else
    char *res;
#endif
    if (!errmsg || errlen < 1)
        return NULL;
#if defined(_WIN32) || defined(__ANDROID__)
    saved_errno = errno;
#else
    res = strerror_r(errnum, errmsg, errlen - 1);
    memcpy(errmsg, res, errlen - 1);
    errmsg[errlen - 1] = '\0';
    return errmsg;
#endif
#ifdef _WIN32
    errnum = strerror_s(errmsg, errlen, errnum);
    errno = saved_errno;
    if ((errnum != 0) && (errnum != EINVAL))
        return "?";
    return errmsg;
#endif
#ifdef __ANDROID__
    errnum = strerror_r(errnum, errmsg, errlen);
    errno = saved_errno;
    if ((errnum != 0) && (errnum != EINVAL) && (errnum != ERANGE))
        return "?";
    return errmsg;
#endif
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
    str = sg__malloc(len + 1);
    if (!str) {
        errno = EINVAL;
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
#else
    char *buf;
    size_t len;
#define SG_TMPDIR_TRY_ENV(name) \
do {                            \
    buf = getenv(name);         \
    if (buf)                    \
        goto done;              \
}                               \
while (0)
    SG_TMPDIR_TRY_ENV("TMPDIR");
    SG_TMPDIR_TRY_ENV("TMP");
    SG_TMPDIR_TRY_ENV("TEMP");
    SG_TMPDIR_TRY_ENV("TEMPDIR");
#undef SG_TMPDIR_TRY_ENV
#ifdef __ANDROID__
    buf = "/data/local/tmp";
#else
    buf = "/tmp";
#endif
done:
    buf = strdup(buf);
    if (!buf)
        return NULL;
    len = strlen(buf);
    if (len > 1 && buf[len - 1] == '/')
        len--;
    buf[len] = '\0';
    return buf;
#endif
}

/* Errors. */

void sg__err_cb(__SG_UNUSED void *cls, const char *err) {
    if (isatty(fileno(stderr)) && (fprintf(stderr, "%s", err) > 0))
        fflush(stderr);
}
