/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 * Cross-platform library which helps to develop web servers or frameworks.
 *
 * Copyright (C) 2016-2021 Silvio Clecio <silvioprog@gmail.com>
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

/**
 * \mainpage
 * \li \ref sg_api
 */

/** \defgroup sg_api API reference
 * The API reference grouped by feature.
 **/

#ifndef SAGUI_H
#define SAGUI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#ifndef SG_EXTERN
#ifdef _WIN32
#ifdef BUILDING_LIBSAGUI
#define SG_EXTERN __declspec(dllexport) extern
#else /* _WIN32 */
#define SG_EXTERN __declspec(dllimport) extern
#endif /* _WIN32 */
#else /* BUILDING_LIBSAGUI */
#define SG_EXTERN extern
#endif /* BUILDING_LIBSAGUI */
#endif /* SG_EXTERN */

#ifndef __SG_UNUSED
#define __SG_UNUSED __attribute__((unused))
#endif /* __SG_UNUSED */

#ifndef __SG_MALLOC
#define __SG_MALLOC __attribute__((malloc))
#endif /* __SG_MALLOC */

#ifndef __SG_FORMAT
#define __SG_FORMAT(...) __attribute__((format(printf, __VA_ARGS__)))
#endif /* __SG_FORMAT */

#define SG_VERSION_MAJOR 3
#define SG_VERSION_MINOR 4
#define SG_VERSION_PATCH 0
#define SG_VERSION_HEX                                                         \
  ((SG_VERSION_MAJOR << 16) | (SG_VERSION_MINOR << 8) | (SG_VERSION_PATCH))

#define SG_ERR_SIZE 256

/**
 * \ingroup sg_api
 * \defgroup sg_utils Utilities
 * All utility functions of the library.
 * \{
 */

/**
 * Callback signature used to override the function which allocates a new
 * memory space.
 * \param[in] size Memory size to be allocated.
 * \return Pointer of the allocated memory.
 * \retval NULL If size is `0` or no memory space.
 */
typedef void *(*sg_malloc_func)(size_t size);

/**
 * Callback signature used to override the function which reallocates an
 * existing memory block.
 * \param[in] ptr Pointer of the memory to be reallocated.
 * \param[in] size Memory size to be reallocated.
 * \return Pointer of the reallocated memory.
 */
typedef void *(*sg_realloc_func)(void *ptr, size_t size);

/**
 * Callback signature used to override the function which frees a memory space
 * previously allocated by #sg_malloc(), #sg_alloc() or #sg_realloc().
 * \param[in] ptr Pointer of the memory to be freed.
 */
typedef void (*sg_free_func)(void *ptr);

/**
 * Callback signature used to override the function which returns the value of
 * `x` raised to the power of `y`.
 * \param[in] x Floating point base value.
 * \param[in] y Floating point power value.
 * \return Value of `x` raised to the power of `y`.
 */
typedef double (*sg_pow_func)(double x, double y);

/**
 * Callback signature used to override the function which returns the remainder
 * of `x` divided by `y`.
 * \param[in] x Floating point value with the division numerator.
 * \param[in] y Floating point value with the division denominator.
 * \return Remainder of `x` divided by `y`.
 */
typedef double (*sg_fmod_func)(double x, double y);

/**
 * Callback signature used by functions that handle errors.
 * \param[out] cls User-defined closure.
 * \param[out] err Error message.
 */
typedef void (*sg_err_cb)(void *cls, const char *err);

/**
 * Callback signature used by functions that write streams.
 * \param[out] handle Stream handle.
 * \param[out] offset Current stream offset.
 * \param[out] buf Current buffer to be written.
 * \param[out] size Size of the current buffer to be written.
 * \return Total written buffer.
 */
typedef ssize_t (*sg_write_cb)(void *handle, uint64_t offset, const char *buf,
                               size_t size);

/**
 * Callback signature used by functions that read streams.
 * \param[out] handle Stream handle.
 * \param[out] offset Current stream offset.
 * \param[out] buf Current read buffer.
 * \param[out] size Size of the current read buffer.
 * \return Total read buffer.
 */
typedef ssize_t (*sg_read_cb)(void *handle, uint64_t offset, char *buf,
                              size_t size);

/**
 * Callback signature used by functions that free streams.
 * \param[out] handle Stream handle.
 */
typedef void (*sg_free_cb)(void *handle);

/**
 * Callback signature used by functions that save streams.
 * \param[out] handle Stream handle.
 * \param[out] overwritten Overwrite an already existed stream.
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to abort the saving.
 */
typedef int (*sg_save_cb)(void *handle, bool overwritten);

/**
 * Callback signature used by functions that save streams. It allows to specify
 * the destination file path.
 * \param[out] handle Stream handle.
 * \param[out] path Absolute path to store the stream.
 * \param[out] overwritten Overwrite an already existed stream.
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to abort the saving.
 */
typedef int (*sg_save_as_cb)(void *handle, const char *path, bool overwritten);

/**
 * Returns the library version number.
 * \return Library version packed into a single integer.
 */
SG_EXTERN unsigned int sg_version(void);

/**
 * Returns the library version number as string in the format
 * `<MAJOR>.<MINOR>.<PATCH>`.
 * \return Library version packed into a null-terminated string.
 */
SG_EXTERN const char *sg_version_str(void);

/**
 * Overrides the standard functions
 * [malloc(3)](https://linux.die.net/man/3/malloc),
 * [realloc(3)](https://linux.die.net/man/3/realloc) and
 * [free(3)](https://linux.die.net/man/3/free) set by default in the memory
 * manager.
 * \param[in] malloc_func Reference to override the function `malloc()`.
 * \param[in] realloc_func Reference to override the function `realloc()`.
 * \param[in] free_func Reference to override the function `free()`.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \note It must be called before any other Sagui function or after all
 * resources have been freed.
 */
SG_EXTERN int sg_mm_set(sg_malloc_func malloc_func,
                        sg_realloc_func realloc_func, sg_free_func free_func);

/**
 * Allocates a new memory space.
 * \param[in] size Memory size to be allocated.
 * \return Pointer of the allocated memory.
 * \retval NULL If size is `0` or no memory space.
 * \note Equivalent to [malloc(3)](https://linux.die.net/man/3/malloc).
 */
SG_EXTERN void *sg_malloc(size_t size) __SG_MALLOC;

/**
 * Allocates a new zero-initialized memory space.
 * \param[in] size Memory size to be allocated.
 * \return Pointer of the zero-initialized allocated memory.
 * \retval NULL If size is `0` or no memory space.
 */
SG_EXTERN void *sg_alloc(size_t size) __SG_MALLOC;

/**
 * Reallocates an existing memory block.
 * \param[in] ptr Pointer of the memory to be reallocated.
 * \param[in] size Memory size to be reallocated.
 * \return Pointer of the reallocated memory.
 * \note Equivalent to [realloc(3)](https://linux.die.net/man/3/realloc).
 */
SG_EXTERN void *sg_realloc(void *ptr, size_t size) __SG_MALLOC;

/**
 * Frees a memory space previously allocated by #sg_malloc(), #sg_alloc() or
 * #sg_realloc().
 * \param[in] ptr Pointer of the memory to be freed.
 * \note Equivalent to [free(3)](https://linux.die.net/man/3/free).
 */
SG_EXTERN void sg_free(void *ptr);

/**
 * Overrides the standard functions [pow(3)](https://linux.die.net/man/3/pow)
 * and [fmod(3)](https://linux.die.net/man/3/fmod) set by default in the math
 * manager.
 * \param[in] pow_func Reference to override the function `pow()`.
 * \param[in] fmod_func Reference to override the function `fmod()`.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \note It must be called before any other Sagui function or after all
 * resources have been freed.
 */
SG_EXTERN int sg_math_set(sg_pow_func pow_func, sg_fmod_func fmod_func);

/**
 * Returns string describing an error number.
 * \param[in] errnum Error number.
 * \param[in,out] errmsg Pointer of a string to store the error message.
 * \param[in] errlen Length of the error message.
 * \return Pointer to \pr{errmsg}.
 */
SG_EXTERN char *sg_strerror(int errnum, char *errmsg, size_t errlen);

/**
 * Checks if a string is a HTTP post method.
 * \param[in] method Null-terminated string.
 * \retval true If \pr{method} is `POST`, `PUT`, `DELETE` or `OPTIONS`.
 */
SG_EXTERN bool sg_is_post(const char *method);

/**
 * Extracts the entry-point of a path or resource. For example, given a path
 * `/api1/customer`, the part considered as entry-point is `/api1`.
 * \param path Path as a null-terminated string.
 * \return Entry-point as a null-terminated string.
 * \retval NULL If no memory space is available.
 * \warning The caller must free the returned value.
 */
SG_EXTERN char *sg_extract_entrypoint(const char *path) __SG_MALLOC;

/**
 * Returns the system temporary directory.
 * \return Temporary directory as a null-terminated string.
 * \retval NULL If no memory space is available.
 * \warning The caller must free the returned value.
 */
SG_EXTERN char *sg_tmpdir(void) __SG_MALLOC;

/**
 * Indicates the end-of-read processed in #sg_httpres_sendstream().
 * \param[in] err `true` to return a value indicating a stream reading error.
 * \return Value to end a stream reading.
 */
SG_EXTERN ssize_t sg_eor(bool err);

/**
 * Obtains the IP of a socket handle (e.g. the one returned by
 * #sg_httpreq_client()) into a null-terminated string.
 * \param[in] socket Socket handle.
 * \param[out] buf Pointer of the string to store the IP.
 * \param[in] size Size of the string to store the IP.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EAFNOSUPPORT Address family not supported by protocol.
 * \retval ENOSPC No space left on device.
 */
SG_EXTERN int sg_ip(const void *socket, char *buf, size_t size);

/** \} */

/**
 * \ingroup sg_api
 * \defgroup sg_str String
 * String handle and its related functions.
 * \{
 */

/**
 * Handle for the string structure used to represent a HTML body, POST payload
 * and more.
 * \struct sg_str
 */
struct sg_str;

/**
 * Creates a new zero-initialized string handle.
 * \return String handle.
 * \retval NULL If no memory space is available.
 */
SG_EXTERN struct sg_str *sg_str_new(void) __SG_MALLOC;

/**
 * Frees the string handle previously allocated by #sg_str_new().
 * \param[in] str Pointer of the string handle to be freed.
 */
SG_EXTERN void sg_str_free(struct sg_str *str);

/**
 * Writes a null-terminated string to the string handle \pr{str}. All strings
 * previously written are kept.
 * \param[in] str String handle.
 * \param[in] val String to be written.
 * \param[in] len Length of the string to be written.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_str_write(struct sg_str *str, const char *val, size_t len);

/**
 * Prints a null-terminated formatted string from the argument list to the
 * string handle \pr{str}.
 * \param[in] str String handle.
 * \param[in] fmt Formatted string (following the same
 * [`printf()`](https://linux.die.net/man/3/printf) format specification).
 * \param[in] ap Arguments list (handled by
 * [`va_start()`](https://linux.die.net/man/3/va_start)/
 * [`va_end()`](https://linux.die.net/man/3/va_end)).
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_str_printf_va(struct sg_str *str, const char *fmt, va_list ap);

/**
 * Prints a null-terminated formatted string to the string handle \pr{str}. All
 * strings previously written are kept.
 * \param[in] str String handle.
 * \param[in] fmt Formatted string (following the same
 * [`printf()`](https://linux.die.net/man/3/printf) format specification).
 * \param[in] ... Additional arguments (following the same
 * [`printf()`](https://linux.die.net/man/3/printf) arguments specification).
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_str_printf(struct sg_str *str, const char *fmt, ...)
  __SG_FORMAT(2, 3);

/**
 * Returns the null-terminated string content from the string handle \pr{str}.
 * \param[in] str String handle.
 * \return Content as a null-terminated string.
 * \retval NULL If the \pr{str} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_str_content(struct sg_str *str);

/**
 * Returns the total string length from the handle \pr{str}.
 * \param[in] str String handle.
 * \return Total string length.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN size_t sg_str_length(struct sg_str *str);

/**
 * Clears all existing content in the string handle \pr{str}.
 * \param[in] str String handle.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_str_clear(struct sg_str *str);

/** \} */

/**
 * \ingroup sg_api
 * \defgroup sg_strmap String map
 * String map handle and its related functions.
 * \{
 */

/**
 * Handle for hash table that maps name-value pairs. It is useful to represent
 * posting fields, query-string parameter, client cookies and more.
 * \struct sg_strmap
 */
struct sg_strmap;

/**
 * Callback signature used by #sg_strmap_iter() to iterate pairs of strings.
 * \param[out] cls User-defined closure.
 * \param[out] pair Current iterated pair.
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to stop pairs iteration.
 */
typedef int (*sg_strmap_iter_cb)(void *cls, struct sg_strmap *pair);

/**
 * Callback signature used by #sg_strmap_sort() to sort pairs of strings.
 * \param[out] cls User-defined closure.
 * \param[out] pair_a Current left pair (A).
 * \param[out] pair_b Current right pair (B).
 * \retval -1 A < B.
 * \retval 0 A == B.
 * \retval 1 A > B.
 */
typedef int (*sg_strmap_sort_cb)(void *cls, struct sg_strmap *pair_a,
                                 struct sg_strmap *pair_b);

/**
 * Returns a name from the \pr{pair}.
 * \param[in] pair Pair of name-value.
 * \return Name as a null-terminated string.
 * \retval NULL If the \pr{pair} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_strmap_name(struct sg_strmap *pair);

/**
 * Returns a value from the \pr{pair}.
 * \param[in] pair Pair of name-value.
 * \return Value as a null-terminated string.
 * \retval NULL If the \pr{pair} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_strmap_val(struct sg_strmap *pair);

/**
 * Adds a pair of name-value to the string \pr{map}.
 * \param[in,out] map Pairs map pointer to add a new pair.
 * \param[in] name Pair name.
 * \param[in] val Pair value.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 * \note It cannot check if a name already exists in a pair added to the
 * \pr{map}, then the uniqueness must be managed by the application.
 */
SG_EXTERN int sg_strmap_add(struct sg_strmap **map, const char *name,
                            const char *val);

/**
 * Sets a pair of name-value to the string \pr{map}.
 * \param[in,out] map Pairs map pointer to set a new pair.
 * \param[in] name Pair name.
 * \param[in] val Pair value.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 * \note If a name already exists in a pair previously added into the \pr{map},
 * then the function replaces its value, otherwise it is added as a new pair.
 */
SG_EXTERN int sg_strmap_set(struct sg_strmap **map, const char *name,
                            const char *val);

/**
 * Finds a pair by name.
 * \param[in] map Pairs map.
 * \param[in] name Name to find the pair.
 * \param[in,out] pair Pointer of the variable to store the found pair.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOENT Pair not found.
 * \retval ENOMEM Out of memory.
 */
SG_EXTERN int sg_strmap_find(struct sg_strmap *map, const char *name,
                             struct sg_strmap **pair);

/**
 * Gets a pair by name and returns the value.
 * \param[in] map Pairs map.
 * \param[in] name Name to get the pair.
 * \return Pair value as a null-terminated string.
 * \retval NULL If \pr{map} or \pr{name} is null or pair is not found.
 */
SG_EXTERN const char *sg_strmap_get(struct sg_strmap *map, const char *name);

/**
 * Removes a pair by name.
 * \param[in] map Pointer to the pairs map.
 * \param[in] name Name to find and then remove the pair.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOENT Pair already removed.
 * \retval ENOMEM Out of memory.
 */
SG_EXTERN int sg_strmap_rm(struct sg_strmap **map, const char *name);

/**
 * Iterates over pairs map.
 * \param[in] map Pairs map.
 * \param[in] cb Callback to iterate the pairs.
 * \param[in,out] cls User-specified value.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \return Callback result when it is different from `0`.
 */
SG_EXTERN int sg_strmap_iter(struct sg_strmap *map, sg_strmap_iter_cb cb,
                             void *cls);

/**
 * Sorts the pairs map.
 * \param[in,out] map Pointer to the pairs map.
 * \param[in] cb Callback to sort the pairs.
 * \param[in,out] cls User-specified value.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_strmap_sort(struct sg_strmap **map, sg_strmap_sort_cb cb,
                             void *cls);

/**
 * Counts the total pairs in the map.
 * \param[in] map Pairs map.
 * \return Total of pairs.
 * \retval 0 If the list is empty or null.
 */
SG_EXTERN unsigned int sg_strmap_count(struct sg_strmap *map);

/**
 * Returns the next pair in the map.
 * \param[in,out] next Pointer to the next pair.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_strmap_next(struct sg_strmap **next);

/**
 * Cleans the entire map.
 * \param[in,out] map Pointer to the pairs map.
 */
SG_EXTERN void sg_strmap_cleanup(struct sg_strmap **map);

/** \} */

/**
 * \ingroup sg_api
 * \defgroup sg_httpsrv HTTP server
 * Fast event-driven HTTP server.
 * \{
 */

/**
 * Handle for the HTTP basic authentication.
 * \struct sg_httpauth
 */
struct sg_httpauth;

/**
 * Handle for the upload handling. It is used to represent a single upload or a
 * list of uploads.
 * \struct sg_httpupld
 */
struct sg_httpupld;

/**
 * Handle for the request handling. It contains headers, cookies, query-string,
 * fields, payloads, uploads and other data sent by the client.
 * \struct sg_httpreq
 */
struct sg_httpreq;

/**
 * Handle for the response handling. It dispatches headers, contents, binaries,
 * files and other data to the client.
 * \struct sg_httpres
 */
struct sg_httpres;

/**
 * Handle for the fast event-driven HTTP server.
 * \struct sg_httpsrv
 */
struct sg_httpsrv;

/**
 * Callback signature used to handle client connection events.
 * \param[out] cls User-defined closure.
 * \param[out] client Socket handle of the client.
 * \param[in,out] closed Indicates if the client is connected allowing to
 * close it.
 */
typedef void (*sg_httpsrv_cli_cb)(void *cls, const void *client, bool *closed);

/**
 * Callback signature used to grant or deny the user access to the server
 * resources.
 * \param[out] cls User-defined closure.
 * \param[out] auth Authentication handle.
 * \param[out] req Request handle.
 * \param[out] res Response handle.
 * \retval true Grants the user access.
 * \retval false Denies the user access.
 */
typedef bool (*sg_httpauth_cb)(void *cls, struct sg_httpauth *auth,
                               struct sg_httpreq *req, struct sg_httpres *res);

/**
 * Callback signature used to handle uploaded files and/or fields.
 * \param[out] cls User-defined closure.
 * \param[in,out] handle Stream handle pointer.
 * \param[out] dir Directory to store the uploaded files.
 * \param[out] field Posted field.
 * \param[out] name Uploaded file name.
 * \param[out] mime Uploaded file content-type (e.g.: `text/plain`, `image/png`,
 * `application/json` etc.).
 * \param[out] encoding Uploaded file transfer-encoding (e.g.: `chunked`,
 * `deflate`, `gzip` etc.).
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to refuse the upload.
 */
typedef int (*sg_httpupld_cb)(void *cls, void **handle, const char *dir,
                              const char *field, const char *name,
                              const char *mime, const char *encoding);

/**
 * Callback signature used to iterate uploaded files.
 * \param[out] cls User-defined closure.
 * \param[out] upld Current upload item.
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to stop list iteration.
 */
typedef int (*sg_httpuplds_iter_cb)(void *cls, struct sg_httpupld *upld);

/**
 * Callback signature used to handle requests and responses.
 * \param[out] cls User-defined closure.
 * \param[out] req Request handle.
 * \param[out] res Response handle.
 */
typedef void (*sg_httpreq_cb)(void *cls, struct sg_httpreq *req,
                              struct sg_httpres *res);

/**
 * Sets the authentication protection space (realm).
 * \param[in] auth Authentication handle.
 * \param[in] realm Realm string.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Realm already set.
 * \retval ENOMEM Out of memory.
 */
SG_EXTERN int sg_httpauth_set_realm(struct sg_httpauth *auth,
                                    const char *realm);

/**
 * Gets the authentication protection space (realm).
 * \param[in] auth Authentication handle.
 * \return Realm as a null-terminated string.
 * \retval NULL If \pr{auth} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpauth_realm(struct sg_httpauth *auth);

/**
 * Deny the authentication sending the reason to the user.
 * \param[in] auth Authentication handle.
 * \param[in] reason Denial reason.
 * \param[in] content_type Content type.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Already denied.
 * \retval ENOMEM Out of memory.
 */
SG_EXTERN int sg_httpauth_deny2(struct sg_httpauth *auth, const char *reason,
                                const char *content_type, unsigned int status);

/**
 * Deny the authentication sending the reason to the user.
 * \param[in] auth Authentication handle.
 * \param[in] reason Denial reason.
 * \param[in] content_type Content type.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Already denied.
 * \retval ENOMEM Out of memory.
 */
SG_EXTERN int sg_httpauth_deny(struct sg_httpauth *auth, const char *reason,
                               const char *content_type);

/**
 * Cancels the authentication loop while the user is trying to access
 * the server.
 * \param[in] auth Authentication handle.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpauth_cancel(struct sg_httpauth *auth);

/**
 * Returns the authentication user.
 * \param[in] auth Authentication handle.
 * \return User as a null-terminated string.
 * \retval NULL If \pr{auth} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpauth_usr(struct sg_httpauth *auth);

/**
 * Returns the authentication password.
 * \param[in] auth Authentication handle.
 * \return Password as a null-terminated string.
 * \retval NULL If \pr{auth} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpauth_pwd(struct sg_httpauth *auth);

/**
 * Iterates over all the upload items in the \pr{uplds} list.
 * \param[in] uplds Uploads list handle.
 * \param[in] cb Callback to iterate over upload items.
 * \param[in] cls User-defined closure.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval E<ERROR> User-defined error to abort the list iteration.
 */
SG_EXTERN int sg_httpuplds_iter(struct sg_httpupld *uplds,
                                sg_httpuplds_iter_cb cb, void *cls);

/**
 * Gets the next upload item starting from the first item pointer \pr{upld}.
 * \param[in,out] upld Next upload item starting from the first item pointer.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpuplds_next(struct sg_httpupld **upld);

/**
 * Counts the total upload items in the list \pr{uplds}.
 * \param[in] uplds Uploads list.
 * \return Total of items.
 * \retval 0 If the list is empty or null.
 */
SG_EXTERN unsigned int sg_httpuplds_count(struct sg_httpupld *uplds);

/**
 * Returns the stream handle of the upload handle \pr{upld}.
 * \param[in] upld Upload handle.
 * \return Stream handle.
 * \retval NULL If \pr{upld} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN void *sg_httpupld_handle(struct sg_httpupld *upld);

/**
 * Returns the directory of the upload handle \pr{upld}.
 * \param[in] upld Upload handle.
 * \return Upload directory as a null-terminated string.
 * \retval NULL If \pr{upld} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpupld_dir(struct sg_httpupld *upld);

/**
 * Returns the field of the upload handle \pr{upld}.
 * \param[in] upld Upload handle.
 * \return Upload field as a null-terminated string.
 * \retval NULL If \pr{upld} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpupld_field(struct sg_httpupld *upld);

/**
 * Returns the name of the upload handle \pr{upld}.
 * \param[in] upld Upload handle.
 * \return Upload name as a null-terminated string.
 * \retval NULL If \pr{upld} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpupld_name(struct sg_httpupld *upld);

/**
 * Returns the MIME (content-type) of the upload.
 * \param[in] upld Upload handle.
 * \return Upload MIME as a null-terminated string.
 * \retval NULL If \pr{upld} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpupld_mime(struct sg_httpupld *upld);

/**
 * Returns the encoding (transfer-encoding) of the upload.
 * \param[in] upld Upload handle.
 * \return Upload encoding as a null-terminated string.
 * \retval NULL If \pr{upld} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpupld_encoding(struct sg_httpupld *upld);

/**
 * Returns the size of the upload.
 * \param[in] upld Upload handle.
 * \return Upload size into `uint64`. If \pr{upld} is null, set the `errno` to
 * `EINVAL`.
 */
SG_EXTERN uint64_t sg_httpupld_size(struct sg_httpupld *upld);

/**
 * Saves the uploaded file defining the destination path by upload name and
 * directory.
 * \param[in] upld Upload handle.
 * \param[in] overwritten Overwrite upload file if it exists.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EEXIST File already exists (if \pr{overwritten} is `false`).
 * \retval EISDIR Destination file is a directory.
 */
SG_EXTERN int sg_httpupld_save(struct sg_httpupld *upld, bool overwritten);

/**
 * Saves the uploaded file allowing to define the destination path.
 * \param[in] upld Upload handle.
 * \param[in] path Absolute destination path.
 * \param[in] overwritten Overwrite upload file if it exists.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EEXIST File already exists (if \pr{overwritten} is `true`).
 * \retval EISDIR Destination file is a directory.
 */
SG_EXTERN int sg_httpupld_save_as(struct sg_httpupld *upld, const char *path,
                                  bool overwritten);

/**
 * Returns the server instance.
 * \param[in] req Request handle.
 * \return Reference to the server instance.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`
 */
SG_EXTERN struct sg_httpsrv *sg_httpreq_srv(struct sg_httpreq *req);

/**
 * Returns the client headers into #sg_strmap map.
 * \param[in] req Request handle.
 * \return Reference to the client headers map.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`
 * \note The headers map is automatically freed by the library.
 */
SG_EXTERN struct sg_strmap **sg_httpreq_headers(struct sg_httpreq *req);

/**
 * Returns the client cookies into #sg_strmap map.
 * \param[in] req Request handle.
 * \return Reference to the client cookies map.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`
 * \note The cookies map is automatically freed by the library.
 */
SG_EXTERN struct sg_strmap **sg_httpreq_cookies(struct sg_httpreq *req);

/**
 * Returns the query-string into #sg_strmap map.
 * \param[in] req Request handle.
 * \return Reference to the query-string map.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`
 * \note The query-string map is automatically freed by the library.
 */
SG_EXTERN struct sg_strmap **sg_httpreq_params(struct sg_httpreq *req);

/**
 * Returns the fields of a HTML form into #sg_strmap map.
 * \param[in] req Request handle.
 * \return Reference to the form fields map.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`
 * \note The form fields map is automatically freed by the library.
 */
SG_EXTERN struct sg_strmap **sg_httpreq_fields(struct sg_httpreq *req);

/**
 * Returns the HTTP version.
 * \param[in] req Request handle.
 * \return HTTP version as a null-terminated string.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpreq_version(struct sg_httpreq *req);

/**
 * Returns the HTTP method.
 * \param[in] req Request handle.
 * \return HTTP method as a null-terminated string.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpreq_method(struct sg_httpreq *req);

/**
 * Returns the path component.
 * \param[in] req Request handle.
 * \return Path component as a null-terminated string.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpreq_path(struct sg_httpreq *req);

/**
 * Returns the posting payload into a #sg_str instance.
 * \param[in] req Request handle.
 * \return Instance of the payload.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`.
 * \note The form payload instance is automatically freed by the library.
 */
SG_EXTERN struct sg_str *sg_httpreq_payload(struct sg_httpreq *req);

/**
 * Checks if the client is uploading data.
 * \param[in] req Request handle.
 * \retval true If the client is uploading data, `false` otherwise. If \pr{req}
 * is null, set the `errno` to `EINVAL`.
 */
SG_EXTERN bool sg_httpreq_is_uploading(struct sg_httpreq *req);

/**
 * Returns the list of the uploaded files.
 * \param[in] req Request handle.
 * \return List of the uploaded files.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`.
 * \note The uploads list is automatically freed by the library.
 */
SG_EXTERN struct sg_httpupld *sg_httpreq_uploads(struct sg_httpreq *req);

/**
 * Gets the socket handle of the client.
 * \param[in] req Request handle.
 * \return Socket address of the client.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const void *sg_httpreq_client(struct sg_httpreq *req);

#ifdef SG_HTTPS_SUPPORT

/**
 * Returns the [GnuTLS](https://gnutls.org) session handle.
 * \param[in] req Request handle.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN void *sg_httpreq_tls_session(struct sg_httpreq *req);

#endif /* SG_HTTPS_SUPPORT */

/**
 * Isolates a request from the main event loop to an own dedicated thread,
 * bringing it back when the request finishes.
 * \param[in] req Request handle.
 * \param[in] cb Callback to handle requests and responses isolated from the
 * main event loop.
 * \param[in] cls User-defined closure.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 * \retval E<ERROR> Any returned error from the OS threading library.
 * \note Isolated requests will not time out.
 * \note While a request is isolated, the library will not detect disconnects
 * by the client.
 */
SG_EXTERN int sg_httpreq_isolate(struct sg_httpreq *req, sg_httpreq_cb cb,
                                 void *cls);

/**
 * Sets user data to the request handle.
 * \param[in] req Request handle.
 * \param[in] data User data pointer.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpreq_set_user_data(struct sg_httpreq *req, void *data);

/**
 * Gets user data from the request handle.
 * \param[in] req Request handle.
 * \return User data pointer.
 * \retval NULL If \pr{req} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN void *sg_httpreq_user_data(struct sg_httpreq *req);

/**
 * Returns the server headers into #sg_strmap map.
 * \param[in] res Response handle.
 * \return Reference to the server headers map.
 * \retval NULL If \pr{res} is null and set the `errno` to `EINVAL`
 * \note The headers map is automatically freed by the library.
 */
SG_EXTERN struct sg_strmap **sg_httpres_headers(struct sg_httpres *res);

/**
 * Sets server cookie to the response handle.
 * \param[in] res Response handle.
 * \param[in] name Cookie name.
 * \param[in] val Cookie value.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 */
SG_EXTERN int sg_httpres_set_cookie(struct sg_httpres *res, const char *name,
                                    const char *val);

/**
 * Sends a null-terminated string content to the client.
 * \param[in] res Response handle.
 * \param[in] val Null-terminated string.
 * \param[in] content_type Content type.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval ENOMEM Out of memory.
 */
#define sg_httpres_send(res, val, content_type, status)                        \
  sg_httpres_sendbinary((res), (void *) (val),                                 \
                        (((val) != NULL) ? strlen((val)) : 0), (content_type), \
                        (status))

/**
 * Sends a binary content to the client.
 * \param[in] res Response handle.
 * \param[in] buf Binary content.
 * \param[in] size Content size.
 * \param[in] content_type Content type.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval ENOMEM Out of memory.
 */
SG_EXTERN int sg_httpres_sendbinary(struct sg_httpres *res, void *buf,
                                    size_t size, const char *content_type,
                                    unsigned int status);

/**
 * Offers a file as download.
 * \param[in] res Response handle.
 * \param[in] filename Path of the file to be sent.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval EISDIR Is a directory.
 * \retval EBADF Bad file number.
 * \retval ENOMEM Out of memory.
 */
#define sg_httpres_download(res, filename, status)                             \
  sg_httpres_sendfile2((res), 0, 0, 0, (filename), "attachment", (status))

/**
 * Sends a file to be rendered.
 * \param[in] res Response handle.
 * \param[in] filename Path of the file to be sent.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval EISDIR Is a directory.
 * \retval EBADF Bad file number.
 * \retval ENOMEM Out of memory.
 */
#define sg_httpres_render(res, filename, status)                               \
  sg_httpres_sendfile2((res), 0, 0, 0, (filename), "inline", (status))

/**
 * Sends a file to the client.
 * \param[in] res Response handle.
 * \param[in] size Size of the file to be sent. Use zero to calculate
 * automatically.
 * \param[in] max_size Maximum allowed file size. Use zero for no limit.
 * \param[in] offset Offset to start reading from in the file to be sent.
 * \param[in] filename Path of the file to be sent.
 * \param[in] disposition Content disposition as a null-terminated string
 * (attachment or inline).
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval EISDIR Is a directory.
 * \retval EBADF Bad file number.
 * \retval EFBIG File too large.
 * \retval ENOMEM Out of memory.
 * \warning The parameter `disposition` is not checked internally, thus any
 * non-`NULL` value is passed directly to the header `Content-Disposition`.
 */
SG_EXTERN int sg_httpres_sendfile2(struct sg_httpres *res, uint64_t size,
                                   uint64_t max_size, uint64_t offset,
                                   const char *filename,
                                   const char *disposition,
                                   unsigned int status);

/**
 * Sends a file to the client.
 * \param[in] res Response handle.
 * \param[in] size Size of the file to be sent. Use zero to calculate
 * automatically.
 * \param[in] max_size Maximum allowed file size. Use zero for no limit.
 * \param[in] offset Offset to start reading from in the file to be sent.
 * \param[in] filename Path of the file to be sent.
 * \param[in] downloaded If `true` it offers the file as download.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval EISDIR Is a directory.
 * \retval EBADF Bad file number.
 * \retval EFBIG File too large.
 * \retval ENOMEM Out of memory.
 */
SG_EXTERN int sg_httpres_sendfile(struct sg_httpres *res, uint64_t size,
                                  uint64_t max_size, uint64_t offset,
                                  const char *filename, bool downloaded,
                                  unsigned int status);

/**
 * Sends a stream to the client.
 * \param[in] res Response handle.
 * \param[in] size Size of the stream.
 * \param[in] read_cb Callback to read data from stream handle.
 * \param[in] handle Stream handle.
 * \param[in] free_cb Callback to free the stream handle.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval ENOMEM Out of memory.
 * \note Use `size = 0` if the stream size is unknown.
 */
SG_EXTERN int sg_httpres_sendstream(struct sg_httpres *res, uint64_t size,
                                    sg_read_cb read_cb, void *handle,
                                    sg_free_cb free_cb, unsigned int status);

#ifdef SG_HTTP_COMPRESSION

/**
 * Compresses a null-terminated string content and sends it to the client. The
 * compression is done by zlib library using the DEFLATE compression algorithm.
 * \param[in] res Response handle.
 * \param[in] val Null-terminated string.
 * \param[in] content_type Content type.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 * \retval ENOBUFS No buffer space available.
 * \retval EALREADY Operation already in progress.
 * \retval Z_<ERROR> zlib error as negative number.
 * \note When compression succeeds, the header `Content-Encoding: deflate` is
 * automatically added to the response.
 */
#define sg_httpres_zsend(res, val, content_type, status)                       \
  sg_httpres_zsendbinary((res), (void *) (val),                                \
                         (((val) != NULL) ? strlen((val)) : 0),                \
                         (content_type), (status))

/**
 * Compresses a binary content and sends it to the client. The compression is
 * done by zlib library using the DEFLATE compression algorithm.
 * \param[in] res Response handle.
 * \param[in] level Compression level (1..9 or -1 for default).
 * \param[in] buf Binary content.
 * \param[in] size Content size.
 * \param[in] content_type Content type.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 * \retval ENOBUFS No buffer space available.
 * \retval EALREADY Operation already in progress.
 * \retval Z_<ERROR> zlib error as negative number.
 * \note When compression succeeds, the header `Content-Encoding: deflate` is
 * automatically added to the response.
 */
SG_EXTERN int sg_httpres_zsendbinary2(struct sg_httpres *res, int level,
                                      void *buf, size_t size,
                                      const char *content_type,
                                      unsigned int status);

/**
 * Compresses a binary content and sends it to the client. The compression is
 * done by zlib library using the DEFLATE compression algorithm.
 * \param[in] res Response handle.
 * \param[in] buf Binary content.
 * \param[in] size Content size.
 * \param[in] content_type Content type.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 * \retval ENOBUFS No buffer space available.
 * \retval EALREADY Operation already in progress.
 * \retval Z_<ERROR> zlib error as negative number.
 * \note When compression succeeds, the header `Content-Encoding: deflate` is
 * automatically added to the response.
 */
SG_EXTERN int sg_httpres_zsendbinary(struct sg_httpres *res, void *buf,
                                     size_t size, const char *content_type,
                                     unsigned int status);

/**
 * Compresses a stream and sends it to the client. The compression is done by
 * zlib library using the DEFLATE compression algorithm.
 * \param[in] res Response handle.
 * \param[in] level Compression level (1..9 or -1 for default).
 * \param[in] size Size of the stream.
 * \param[in] read_cb Callback to read data from stream handle.
 * \param[in] handle Stream handle.
 * \param[in] free_cb Callback to free the stream handle.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval ENOMEM Out of memory.
 * \retval Z_<ERROR> zlib error as negative number.
 * \note When compression succeeds, the header `Content-Encoding: deflate` is
 * automatically added to the response.
 */
SG_EXTERN int sg_httpres_zsendstream2(struct sg_httpres *res, int level,
                                      uint64_t size, sg_read_cb read_cb,
                                      void *handle, sg_free_cb free_cb,
                                      unsigned int status);

/**
 * Compresses a stream and sends it to the client. The compression is done by
 * zlib library using the DEFLATE compression algorithm.
 * \param[in] res Response handle.
 * \param[in] read_cb Callback to read data from stream handle.
 * \param[in] handle Stream handle.
 * \param[in] free_cb Callback to free the stream handle.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval ENOMEM Out of memory.
 * \retval Z_<ERROR> zlib error as negative number.
 * \note When compression succeeds, the header `Content-Encoding: deflate` is
 * automatically added to the response.
 */
SG_EXTERN int sg_httpres_zsendstream(struct sg_httpres *res, sg_read_cb read_cb,
                                     void *handle, sg_free_cb free_cb,
                                     unsigned int status);

/**
 * Compresses a file in Gzip format and offers it as download. The compression
 * is done by zlib library using the DEFLATE compression algorithm.
 * \param[in] res Response handle.
 * \param[in] filename Path of the file to be compressed and sent.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval EISDIR Is a directory.
 * \retval EBADF Bad file number.
 * \retval ENOMEM Out of memory.
 * \retval Z_<ERROR> zlib error as negative number.
 * \note When compression succeeds, the header `Content-Encoding: gzip` is
 * automatically added to the response.
 */
#define sg_httpres_zdownload(res, filename, status)                            \
  sg_httpres_zsendfile2((res), 1, 0, 0, 0, (filename), "attachment", (status))

/**
 * Compresses a file in Gzip format and sends it to be rendered. The
 * compression is done by zlib library using the DEFLATE compression algorithm.
 * \param[in] res Response handle.
 * \param[in] filename Path of the file to be sent.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval EISDIR Is a directory.
 * \retval EBADF Bad file number.
 * \retval ENOMEM Out of memory.
 * \retval Z_<ERROR> zlib error as negative number.
 * \note When compression succeeds, the header `Content-Encoding: gzip` is
 * automatically added to the response.
 */
#define sg_httpres_zrender(res, filename, status)                              \
  sg_httpres_zsendfile2((res), 1, 0, 0, 0, (filename), "inline", (status))

/**
 * Compresses a file in Gzip format and sends it to the client. The compression
 * is done by zlib library using the DEFLATE compression algorithm.
 * \param[in] res Response handle.
 * \param[in] level Compression level (1..9 or -1 for default).
 * \param[in] size Size of the file to be sent. Use zero to calculate
 * automatically.
 * \param[in] max_size Maximum allowed file size. Use zero for no limit.
 * \param[in] offset Offset to start reading from in the file to be sent.
 * \param[in] filename Path of the file to be sent.
 * \param[in] disposition Content disposition as a null-terminated string
 * (attachment or inline).
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval EISDIR Is a directory.
 * \retval EBADF Bad file number.
 * \retval EFBIG File too large.
 * \retval ENOMEM Out of memory.
 * \retval Z_<ERROR> zlib error as negative number.
 * \note When compression succeeds, the header `Content-Encoding: gzip` is
 * automatically added to the response.
 * \warning The parameter `disposition` is not checked internally, thus any
 * non-`NULL` value is passed directly to the header `Content-Disposition`.
 */
SG_EXTERN int sg_httpres_zsendfile2(struct sg_httpres *res, int level,
                                    uint64_t size, uint64_t max_size,
                                    uint64_t offset, const char *filename,
                                    const char *disposition,
                                    unsigned int status);

/**
 * Compresses a file in Gzip format and sends it to the client. The compression
 * is done by zlib library using the DEFLATE compression algorithm.
 * \param[in] res Response handle.
 * \param[in] size Size of the file to be sent. Use zero to calculate
 * automatically.
 * \param[in] max_size Maximum allowed file size. Use zero for no limit.
 * \param[in] offset Offset to start reading from in the file to be sent.
 * \param[in] filename Path of the file to be sent.
 * \param[in] downloaded If `true` it offers the file as download.
 * \param[in] status HTTP status code.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Operation already in progress.
 * \retval EISDIR Is a directory.
 * \retval EBADF Bad file number.
 * \retval EFBIG File too large.
 * \retval ENOMEM Out of memory.
 * \retval Z_<ERROR> zlib error as negative number.
 * \note When compression succeeds, the header `Content-Encoding: gzip` is
 * automatically added to the response.
 */
SG_EXTERN int sg_httpres_zsendfile(struct sg_httpres *res, uint64_t size,
                                   uint64_t max_size, uint64_t offset,
                                   const char *filename, bool downloaded,
                                   unsigned int status);

#endif /* SG_HTTP_COMPRESSION */

/**
 * Resets status and internal buffers of the response handle preserving all
 * headers and cookies.
 * \param[in] res Response handle.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpres_reset(struct sg_httpres *res);

/**
 * Clears all headers, cookies, status and internal buffers of the response
 * handle.
 * \param[in] res Response handle.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpres_clear(struct sg_httpres *res);

/**
 * Checks if the response is empty.
 * \param[in] res Response handle.
 * \retval true If the response is empty, `false` otherwise. If \pr{res}
 * is null, set the `errno` to `EINVAL`.
 */
SG_EXTERN bool sg_httpres_is_empty(struct sg_httpres *res);

/**
 * Creates a new HTTP server handle.
 * \param[in] auth_cb Callback to grant/deny user access to the server
 * resources.
 * \param[in] req_cb Callback to handle requests and responses.
 * \param[in] err_cb Callback to handle server errors.
 * \param[in] cls User-defined closure.
 * \return New HTTP server handle.
 * \retval NULL
 *  - If no memory space is available.
 *  - If the \pr{req_cb} or \pr{err_cb} is null and set the `errno` to
 *    `EINVAL`.
 *  - If a threading operation fails and set its error to `errno`.
 */
SG_EXTERN struct sg_httpsrv *sg_httpsrv_new2(sg_httpauth_cb auth_cb,
                                             sg_httpreq_cb req_cb,
                                             sg_err_cb err_cb,
                                             void *cls) __SG_MALLOC;

/**
 * Creates a new HTTP server handle.
 * \param[in] cb Callback to handle requests and responses.
 * \param[in] cls User-defined closure.
 * \return New HTTP server handle.
 * \retval NULL
 *  - If the \pr{cb} is null and set the `errno` to `EINVAL`.
 *  - If a threading operation fails and set its error to `errno`.
 */
SG_EXTERN struct sg_httpsrv *sg_httpsrv_new(sg_httpreq_cb cb,
                                            void *cls) __SG_MALLOC;

/**
 * Frees the server handle previously allocated by #sg_httpsrv_new() or
 * #sg_httpsrv_new2().
 * \param[in] srv Pointer of the server to be freed.
 * \note If the server is running it stops before being freed.
 */
SG_EXTERN void sg_httpsrv_free(struct sg_httpsrv *srv);

#ifdef SG_HTTPS_SUPPORT

/**
 * Starts the HTTPS server.
 * \param[in] srv Server handle.
 * \param[in] key Memory pointer for the private key (key.pem) to be used by
 * the HTTPS server.
 * \param[in] pwd Password for the private key.
 * \param[in] cert Memory pointer for the certificate (cert.pem) to be used by
 * the HTTPS server.
 * \param[in] trust Memory pointer for the certificate (ca.pem) to be used by
 * the HTTPS server for client authentication.
 * \param[in] dhparams Memory pointer for the Diffie Hellman parameters (dh.pem)
 * to be used by the HTTPS server for key exchange.
 * \param[in] priorities Memory pointer specifying the cipher algorithm.
 * Default: `"NORMAL"`.
 * \param[in] port Port for listening to connections.
 * \param[in] threaded Enables/disables the threaded mode. If `true`, the
 * server creates one thread per connection.
 * \retval true If the server is started, `false` otherwise. If \pr{srv} is
 * null, set the `errno` to `EINVAL`.
 * \note If port is `0`, the operating system will assign an unused port
 * randomly.
 */
SG_EXTERN bool sg_httpsrv_tls_listen3(struct sg_httpsrv *srv, const char *key,
                                      const char *pwd, const char *cert,
                                      const char *trust, const char *dhparams,
                                      const char *priorities, uint16_t port,
                                      bool threaded);

/**
 * Starts the HTTPS server.
 * \param[in] srv Server handle.
 * \param[in] key Memory pointer for the private key (key.pem) to be used by
 * the HTTPS server.
 * \param[in] pwd Password for the private key.
 * \param[in] cert Memory pointer for the certificate (cert.pem) to be used by
 * the HTTPS server.
 * \param[in] trust Memory pointer for the certificate (ca.pem) to be used by
 * the HTTPS server for client authentication.
 * \param[in] dhparams Memory pointer for the Diffie Hellman parameters (dh.pem)
 * to be used by the HTTPS server for key exchange.
 * \param[in] port Port for listening to connections.
 * \param[in] threaded Enables/disables the threaded mode. If `true`, the
 * server creates one thread per connection.
 * \retval true If the server is started, `false` otherwise. If \pr{srv} is
 * null, set the `errno` to `EINVAL`.
 * \note If port is `0`, the operating system will assign an unused port
 * randomly.
 */
SG_EXTERN bool sg_httpsrv_tls_listen2(struct sg_httpsrv *srv, const char *key,
                                      const char *pwd, const char *cert,
                                      const char *trust, const char *dhparams,
                                      uint16_t port, bool threaded);

/**
 * Starts the HTTPS server.
 * \param[in] srv Server handle.
 * \param[in] key Memory pointer for the private key (key.pem) to be used by
 * the HTTPS server.
 * \param[in] cert Memory pointer for the certificate (cert.pem) to be used by
 * the HTTPS server.
 * \param[in] port Port for listening to connections.
 * \param[in] threaded Enables/disables the threaded mode. If `true`, the
 * server creates one thread per connection.
 * \retval true If the server is started, `false` otherwise. If \pr{srv} is
 * null, set the `errno` to `EINVAL`.
 * \note If port is `0`, the operating system will assign an unused port
 * randomly.
 */
SG_EXTERN bool sg_httpsrv_tls_listen(struct sg_httpsrv *srv, const char *key,
                                     const char *cert, uint16_t port,
                                     bool threaded);

#endif /* SG_HTTPS_SUPPORT */

/**
 * Starts the HTTP server.
 * \param[in] srv Server handle.
 * \param[in] port Port for listening to connections.
 * \param[in] threaded Enables/disables the threaded mode. If `true`, the
 * server creates one thread per connection.
 * \retval true If the server is started, `false` otherwise. If \pr{srv} is
 * null, set the `errno` to `EINVAL`.
 * \note If port is `0`, the operating system will randomly assign an unused
 * port.
 */
SG_EXTERN bool sg_httpsrv_listen(struct sg_httpsrv *srv, uint16_t port,
                                 bool threaded);

/**
 * Stops the server not to accept new connections.
 * \param[in] srv Server handle.
 * \retval 0 If the server is stopped.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Already shut down.
 */
SG_EXTERN int sg_httpsrv_shutdown(struct sg_httpsrv *srv);

/**
 * Returns the server listening port.
 * \param[in] srv Server handle.
 * \return Server listening port, `0` otherwise. If \pr{srv} is null, set the
 * `errno` to `EINVAL`.
 */
SG_EXTERN uint16_t sg_httpsrv_port(struct sg_httpsrv *srv);

/**
 * Checks if the server was started in threaded mode.
 * \param[in] srv Server handle.
 * \retval true If the server is in threaded mode, `false` otherwise. If
 * \pr{srv} is null, set the `errno` to `EINVAL`.
 */
SG_EXTERN bool sg_httpsrv_is_threaded(struct sg_httpsrv *srv);

/**
 * Sets the server callback for client connection events.
 * \param[in] srv Server handle.
 * \param[in] cb Callback to handle client events.
 * \param[in] cls User-defined closure.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpsrv_set_cli_cb(struct sg_httpsrv *srv,
                                    sg_httpsrv_cli_cb cb, void *cls);

/**
 * Sets the server uploading callbacks.
 * \param[in] srv Server handle.
 * \param[in] cb Callback to handle uploaded files and/or fields.
 * \param[in] cls User-defined closure.
 * \param[in] write_cb Callback to write the stream of the uploaded files.
 * \param[in] free_cb Callback to free stream of the uploaded files.
 * \param[in] save_cb Callback to save the uploaded files.
 * \param[in] save_as_cb Callback to save the uploaded files defining their
 * path.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpsrv_set_upld_cbs(struct sg_httpsrv *srv, sg_httpupld_cb cb,
                                      void *cls, sg_write_cb write_cb,
                                      sg_free_cb free_cb, sg_save_cb save_cb,
                                      sg_save_as_cb save_as_cb);

/**
 * Sets the directory to save the uploaded files.
 * \param[in] srv Server handle.
 * \param[in] dir Directory as a null-terminated string.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpsrv_set_upld_dir(struct sg_httpsrv *srv, const char *dir);

/**
 * Gets the directory of the uploaded files.
 * \param[in] srv Server handle.
 * \return Directory as a null-terminated string.
 * \retval NULL If the \pr{srv} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_httpsrv_upld_dir(struct sg_httpsrv *srv);

/**
 * Sets a size to the post buffering.
 * \param[in] srv Server handle.
 * \param[in] size Post buffering size.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpsrv_set_post_buf_size(struct sg_httpsrv *srv, size_t size);

/**
 * Gets the size of the post buffering.
 * \param[in] srv Server handle.
 * \return Post buffering size.
 * \retval 0 If the \pr{srv} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN size_t sg_httpsrv_post_buf_size(struct sg_httpsrv *srv);

/**
 * Sets a limit to the total payload.
 * \param[in] srv Server handle.
 * \param[in] limit Payload total limit. Use zero for no limit.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpsrv_set_payld_limit(struct sg_httpsrv *srv, size_t limit);

/**
 * Gets the limit of the total payload.
 * \param[in] srv Server handle.
 * \return Payload total limit.
 * \retval 0 If the \pr{srv} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN size_t sg_httpsrv_payld_limit(struct sg_httpsrv *srv);

/**
 * Sets a limit to the total uploads.
 * \param[in] srv Server handle.
 * \param[in] limit Uploads total limit. Use zero for no limit.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpsrv_set_uplds_limit(struct sg_httpsrv *srv,
                                         uint64_t limit);

/**
 * Gets the limit of the total uploads.
 * \param[in] srv Server handle.
 * \return Uploads total limit.
 * \retval 0 If the \pr{srv} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN uint64_t sg_httpsrv_uplds_limit(struct sg_httpsrv *srv);

/**
 * Sets the size for the thread pool.
 * \param[in] srv Server handle.
 * \param[in] size Thread pool size. Size greater than 1 enables the thread
 * pooling.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpsrv_set_thr_pool_size(struct sg_httpsrv *srv,
                                           unsigned int size);

/**
 * Gets the size of the thread pool.
 * \param[in] srv Server handle.
 * \return Thread pool size.
 * \retval 0 If the \pr{srv} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN unsigned int sg_httpsrv_thr_pool_size(struct sg_httpsrv *srv);

/**
 * Sets the inactivity time to a client to get time out.
 * \param[in] srv Server handle.
 * \param[in] timeout Timeout (in seconds). Use zero for infinity timeout. Default: 15 seconds.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpsrv_set_con_timeout(struct sg_httpsrv *srv,
                                         unsigned int timeout);

/**
 * Gets the inactivity time to a client to get time out.
 * \param[in] srv Server handle.
 * \return Timeout (in seconds).
 * \retval 0 If the \pr{srv} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN unsigned int sg_httpsrv_con_timeout(struct sg_httpsrv *srv);

/**
 * Sets the limit of concurrent connections.
 * \param[in] srv Server handle.
 * \param[in] limit Concurrent connections limit. Use zero for no limit.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_httpsrv_set_con_limit(struct sg_httpsrv *srv,
                                       unsigned int limit);

/**
 * Gets the limit of concurrent connections.
 * \param[in] srv Server handle.
 * \return Concurrent connections limit.
 * \retval 0 If the \pr{srv} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN unsigned int sg_httpsrv_con_limit(struct sg_httpsrv *srv);

/**
 * Returns the MHD instance.
 * \param[in] srv Server handle.
 * \return MHD instance.
 * \return NULL If the server is shut down.
 * \retval NULL If \pr{srv} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN void *sg_httpsrv_handle(struct sg_httpsrv *srv);

/** \} */

#ifdef SG_PATH_ROUTING

/**
 * \ingroup sg_api
 * \defgroup sg_routing Path routing
 * High-performance path routing.
 * \{
 */

/**
 * Handle for the entry-point handling. It defines an entry-point to a path or
 * resource. For example, given a path `/api1/customer`, the part considered as
 * entry-point is `/api1`.
 * \struct sg_entrypoint
 */
struct sg_entrypoint;

/**
 * Returns the name of the entry-point handle \pr{entrypoint}.
 * \param[in] entrypoint Entry-point handle.
 * \return Entry-point name as a null-terminated string.
 * \retval NULL If the \pr{entrypoint} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_entrypoint_name(struct sg_entrypoint *entrypoint);

/**
 * Sets user data to the entry-point handle.
 * \param[in] entrypoint Entry-point handle.
 * \param[in] data User data pointer.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_entrypoint_set_user_data(struct sg_entrypoint *entrypoint,
                                          void *data);

/**
 * Gets user data from the entry-point handle.
 * \param[in] entrypoint Entry-point handle.
 * \return User data pointer.
 * \retval NULL If \pr{entrypoint} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN void *sg_entrypoint_user_data(struct sg_entrypoint *entrypoint);

/**
 * Handle for the entry-point list. It is used to create a list of entry-point
 * #sg_entrypoint.
 * \struct sg_entrypoints
 */
struct sg_entrypoints;

/**
 * Callback signature used by #sg_entrypoints_iter() to iterate entry-point
 * items.
 * \param[out] cls User-defined closure.
 * \param[out] pair Current iterated entry-point.
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to stop the items iteration.
 */
typedef int (*sg_entrypoints_iter_cb)(void *cls,
                                      struct sg_entrypoint *entrypoint);

/**
 * Creates a new entry-points handle.
 * \return Entry-points handle.
 * \retval NULL If no memory space is available.
 */
SG_EXTERN struct sg_entrypoints *sg_entrypoints_new(void) __SG_MALLOC;

/**
 * Frees the entry-points handle previously allocated by #sg_entrypoints_new().
 * \param[in] entrypoints Pointer of the entry-points to be freed.
 */
SG_EXTERN void sg_entrypoints_free(struct sg_entrypoints *entrypoints);

/**
 * Adds a new entry-point item to the entry-points \pr{entrypoints}.
 * \param[in] entrypoints Entry-points handle.
 * \param[in] path Entry-point path.
 * \param[in] user_data User data pointer.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 * \retval EALREADY Entry-point already added.
 */
SG_EXTERN int sg_entrypoints_add(struct sg_entrypoints *entrypoints,
                                 const char *path, void *user_data);

/**
 * Removes an entry-point item from the entry-points \pr{entrypoints}.
 * \param[in] entrypoints Entry-points handle.
 * \param[in] path Entry-point path to be removed.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 * \retval ENOENT Entry-point already removed.
 */
SG_EXTERN int sg_entrypoints_rm(struct sg_entrypoints *entrypoints,
                                const char *path);

/**
 * Iterates over entry-point items.
 * \param[in] entrypoints Entry-points handle.
 * \param[in] cb Callback to iterate the entry-point items.
 * \param[in,out] cls User-specified value.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \return Callback result when it is different from `0`.
 */
SG_EXTERN int sg_entrypoints_iter(struct sg_entrypoints *entrypoints,
                                  sg_entrypoints_iter_cb cb, void *cls);

/**
 * Clears all existing entry-point items in the entry-points \pr{entrypoints}.
 * \param[in] entrypoints Entry-points handle.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_entrypoints_clear(struct sg_entrypoints *entrypoints);

/**
 * Finds an entry-point item by path.
 * \param[in] entrypoints Entry-points handle.
 * \param[in,out] entrypoint Pointer of the variable to store the found
 * entry-point.
 * \param[in] path Entry-point path to be found.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 * \retval ENOENT Pair not found.
 */
SG_EXTERN int sg_entrypoints_find(struct sg_entrypoints *entrypoints,
                                  struct sg_entrypoint **entrypoint,
                                  const char *path);

/**
 * Handle for the route item. It holds a user data to be dispatched when a path
 * matches the user defined pattern (route pattern).
 * \struct sg_route
 */
struct sg_route;

/**
 * Callback signature used by #sg_route_segments_iter() to iterate the path
 * segments.
 * \param[out] cls User-defined closure.
 * \param[out] index Current iterated item index.
 * \param[out] segment Current iterated segment.
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to stop the segments iteration.
 */
typedef int (*sg_segments_iter_cb)(void *cls, unsigned int index,
                                   const char *segment);

/**
 * Callback signature used by #sg_route_vars_iter() to iterate the path
 * variables.
 * \param[out] cls User-defined closure.
 * \param[out] name Current iterated variable name.
 * \param[out] val Current iterated variable value.
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to stop the variables iteration.
 */
typedef int (*sg_vars_iter_cb)(void *cls, const char *name, const char *val);

/**
 * Callback signature used to handle the path routing.
 * \param[out] cls User-defined closure.
 * \param[out] route Route handle.
 */
typedef void (*sg_route_cb)(void *cls, struct sg_route *route);

/**
 * Callback signature used by #sg_routes_iter() to iterate route items.
 * \param[out] cls User-defined closure.
 * \param[out] route Current iterated route.
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to stop the route items iteration.
 */
typedef int (*sg_routes_iter_cb)(void *cls, struct sg_route *route);

/**
 * Returns the PCRE2 handle containing the compiled regex code.
 * \param[in] route Route handle.
 * \return PCRE2 handle containing the compiled regex code.
 * \retval NULL If \pr{route} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN void *sg_route_handle(struct sg_route *route);

/**
 * Returns the PCRE2 match data created from the route pattern.
 * \param[in] route Route handle.
 * \return PCRE2 match data.
 * \retval NULL If \pr{route} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN void *sg_route_match(struct sg_route *route);

/**
 * Returns the raw route pattern. For example, given a pattern `/foo`, the raw
 * pattern is `^/foo$`.
 * \param[in] route Route handle.
 * \return Raw pattern as a null-terminated string.
 * \retval NULL If \pr{route} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_route_rawpattern(struct sg_route *route);

/**
 * Returns the route pattern.
 * \param[in] route Route handle.
 * \return Pattern as a null-terminated string.
 * \retval NULL
 *  - If \pr{route} is null and set the `errno` to `EINVAL`.
 *  - If no memory space is available and set the `errno` to `ENOMEM`.
 * \warning The caller must free the returned value.
 */
SG_EXTERN char *sg_route_pattern(struct sg_route *route) __SG_MALLOC;

/**
 * Returns the route path.
 * \param[in] route Route handle.
 * \return Path component as a null-terminated string.
 * \retval NULL If \pr{route} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_route_path(struct sg_route *route);

/**
 * Iterates over path segments.
 * \param[in] route Route handle.
 * \param[in] cb Callback to iterate the path segments.
 * \param[in,out] cls User-specified value.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \return Callback result when it is different from `0`.
 */
SG_EXTERN int sg_route_segments_iter(struct sg_route *route,
                                     sg_segments_iter_cb cb, void *cls);

/**
 * Iterates over path variables.
 * \param[in] route Route handle.
 * \param[in] cb Callback to iterate the path variables.
 * \param[in,out] cls User-specified value.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 * \return Callback result when it is different from `0`.
 */
SG_EXTERN int sg_route_vars_iter(struct sg_route *route, sg_vars_iter_cb cb,
                                 void *cls);

/**
 * Gets user data from the route handle.
 * \param[in] route Route handle.
 * \return User data pointer.
 * \retval NULL If \pr{route} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN void *sg_route_user_data(struct sg_route *route);

/**
 * Adds a route item to the route list \pr{routes}.
 * \param[in,out] routes Route list pointer to add a new route item.
 * \param[in,out] route Pointer of the variable to store the added route
 * reference.
 * \param[in] pattern Pattern as a null-terminated string. It must be a valid
 * regular expression in PCRE2 syntax.
 * \param[in,out] errmsg Pointer of a string to store the error message.
 * \param[in] errlen Length of the error message.
 * \param[in] cb Callback to handle the path routing.
 * \param[in] cls User-defined closure.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Route already added.
 * \retval ENOMEM Out of memory.
 * \note The pattern is enclosed between `^` and `$` automatically if it does
 * not start with `(`.
 * \note The escape sequence \\K is not supported. It causes `EINVAL` if used.
 * \note The pattern is compiled using just-in-time optimization (JIT) when it
 * is supported.
 */
SG_EXTERN int sg_routes_add2(struct sg_route **routes, struct sg_route **route,
                             const char *pattern, char *errmsg, size_t errlen,
                             sg_route_cb cb, void *cls);

/**
 * Adds a route item to the route list \pr{routes}. It uses the `stderr` to
 * print the validation errors.
 * \param[in,out] routes Route list pointer to add a new route item.
 * \param[in] pattern Pattern as a null-terminated string. It must be a valid
 * regular expression in PCRE2 syntax.
 * \param[in] cb Callback to handle the path routing.
 * \param[in] cls User-defined closure.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval EALREADY Route already added.
 * \retval ENOMEM Out of memory.
 * \note The pattern is enclosed between `^` and `$` automatically if it does
 * not start with `(`.
 * \note The escape sequence \\K is not supported. It causes `EINVAL` if used.
 * \note The pattern is compiled using just-in-time optimization (JIT) when it
 * is supported.
 */
SG_EXTERN bool sg_routes_add(struct sg_route **routes, const char *pattern,
                             sg_route_cb cb, void *cls);

/**
 * Removes a route item from the route list \pr{routes}.
 * \param[in,out] routes Route list pointer to add a new route item.
 * \param[in] pattern Pattern as a null-terminated string of the route to be
 * removed.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOENT Route already removed.
 */
SG_EXTERN int sg_routes_rm(struct sg_route **routes, const char *pattern);

/**
 * Iterates over all the routes in the route list \pr{routes}.
 * \param[in] routes Route list handle.
 * \param[in] cb Callback to iterate over route items.
 * \param[in] cls User-defined closure.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval E<ERROR> User-defined error to abort the list iteration.
 */
SG_EXTERN int sg_routes_iter(struct sg_route *routes, sg_routes_iter_cb cb,
                             void *cls);

/**
 * Returns the next route into the route list.
 * \param[in,out] route Pointer to the next route item.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_routes_next(struct sg_route **route);

/**
 * Counts the total routes in the route list \pr{routes}.
 * \param[in] routes Route list handle.
 * \return Total of route items.
 * \retval 0 If the list is empty or null.
 */
SG_EXTERN unsigned int sg_routes_count(struct sg_route *routes);

/**
 * Cleans the entire route list.
 * \param[in,out] routes Pointer to the route list.
 */
SG_EXTERN int sg_routes_cleanup(struct sg_route **routes);

/**
 * Handle for the path router. It holds the reference of a route list to be
 * dispatched.
 * \struct sg_router
 */
struct sg_router;

/**
 * Callback signature used by #sg_router_dispatch2 in the route dispatching
 * loop.
 * \param[out] cls User-defined closure.
 * \param[out] path Route path as a null-terminated string.
 * \param[out] route Route handle.
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to stop the route dispatching loop.
 */
typedef int (*sg_router_dispatch_cb)(void *cls, const char *path,
                                     struct sg_route *route);

/**
 * Callback signature used by #sg_router_dispatch2 when the path matches the
 * pattern before the route dispatching.
 * \param[out] cls User-defined closure.
 * \param[out] route Route handle.
 * \retval 0 Success.
 * \retval E<ERROR> User-defined error to stop the route dispatching.
 */
typedef int (*sg_router_match_cb)(void *cls, struct sg_route *route);

/**
 * Creates a new path router handle. It requires a filled route list
 * \pr{routes}.
 * \param[in] routes Route list handle.
 * \return New router handle.
 * \retval NULL If the \pr{routes} is null and set the `errno` to `EINVAL` or
 * no memory space.
 */
SG_EXTERN struct sg_router *sg_router_new(struct sg_route *routes) __SG_MALLOC;

/**
 * Frees the router handle previously allocated by #sg_router_new().
 * \param[in] router Router handle.
 */
SG_EXTERN void sg_router_free(struct sg_router *router);

/**
 * Dispatches a route that its pattern matches the path passed in \pr{path}.
 * \param[in] router Router handle.
 * \param[in] path Path to dispatch a route.
 * \param[in] user_data User data pointer to be held by the route.
 * \param[in] dispatch_cb Callback triggered for each route item in the route
 * dispatching loop.
 * \param[in] cls User-defined closure passed to the \pr{dispatch_cb} and
 * \pr{match_cb} callbacks.
 * \param[in] match_cb Callback triggered when the path matches the route
 * pattern.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOENT Route not found or path not matched.
 * \retval E<ERROR> User-defined error in \pr{dispatch_cb} or \pr{match_cb}.
 * \note The route callback #sg_route_cb is triggered when the path matches the
 * route pattern.
 * \note The match logic uses just-in-time optimization (JIT) when it is
 * supported.
 */
SG_EXTERN int sg_router_dispatch2(struct sg_router *router, const char *path,
                                  void *user_data,
                                  sg_router_dispatch_cb dispatch_cb, void *cls,
                                  sg_router_match_cb match_cb);

/**
 * Dispatches a route that its pattern matches the path passed in \pr{path}.
 * \param[in] router Router handle.
 * \param[in] path Path to dispatch a route.
 * \param[in] user_data User data pointer to be held by the route.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOENT Route not found or path not matched.
 * \retval E<ERROR> User-defined error in \pr{dispatch_cb} or \pr{match_cb}.
 * \note The route callback #sg_route_cb is triggered when the path matches the
 * route pattern.
 * \note The match logic uses just-in-time optimization (JIT) when it is
 * supported.
 */
SG_EXTERN int sg_router_dispatch(struct sg_router *router, const char *path,
                                 void *user_data);

/** \} */

#endif /* SG_PATH_ROUTING */

#ifdef SG_MATH_EXPR_EVAL

/**
 * \ingroup sg_api
 * \defgroup sg_expr Math expression evaluator
 * Mathematical expression evaluator.
 * \{
 */

/**
 * Handle for the mathematical expression evaluator.
 * \struct sg_expr
 */
struct sg_expr;

/**
 * Possible error types returned by the mathematical expression evaluator.
 * \enum sg_expr_err_type
 */
enum sg_expr_err_type {
  /** Error not related to evaluation, e.g. `EINVAL`. */
  SG_EXPR_ERR_UNKNOWN,
  /** Unexpected number, e.g. `"(1+2)3"`. */
  SG_EXPR_ERR_UNEXPECTED_NUMBER,
  /** Unexpected word, e.g. `"(1+2)x"`. */
  SG_EXPR_ERR_UNEXPECTED_WORD,
  /** Unexpected parenthesis, e.g. `"1(2+3)"`. */
  SG_EXPR_ERR_UNEXPECTED_PARENS,
  /** Missing expected operand, e.g. `"0^+1"`. */
  SG_EXPR_ERR_MISSING_OPERAND,
  /** Unknown operator, e.g. `"(1+2)."`. */
  SG_EXPR_ERR_UNKNOWN_OPERATOR,
  /** Invalid function name, e.g. `"unknownfunc()"`. */
  SG_EXPR_ERR_INVALID_FUNC_NAME,
  /** Bad parenthesis, e.g. `"(1+2"`. */
  SG_EXPR_ERR_BAD_PARENS,
  /** Too few arguments passed to a macro, e.g. `"$()"`. */
  SG_EXPR_ERR_TOO_FEW_FUNC_ARGS,
  /** First macro argument is not variable, e.g. `"$(1)"`. */
  SG_EXPR_ERR_FIRST_ARG_IS_NOT_VAR,
  /** Bad variable name, e.g. `"2.3.4"`. */
  SG_EXPR_ERR_BAD_VARIABLE_NAME,
  /** Bad assignment, e.g. `"2=3"`. */
  SG_EXPR_ERR_BAD_ASSIGNMENT
};

/**
 * Handle to access a user-defined function registered in the mathematical
 * expression extension.
 * \struct sg_expr_argument
 */
struct sg_expr_argument;

/**
 * Callback signature to specify a function at build time to be executed at
 * run time in a mathematical expression.
 * \param[out] cls User-defined closure.
 * \param[out] args Floating-point arguments passed to the function.
 * \param[out] identifier Null-terminated string to identify the function.
 * \return Floating-point value calculated at build time to be used at run time
 * in a mathematical expression.
 */
typedef double (*sg_expr_func)(void *cls, struct sg_expr_argument *args,
                               const char *identifier);

/**
 * Handle for the mathematical expression evaluator extension.
 * \struct sg_expr_extension
 */
struct sg_expr_extension {
  /** User-defined function to be executed at run time in a mathematical
   * expression. */
  sg_expr_func func;
  /** Null-terminated to identify the function. */
  const char *identifier;
  /** User-defined closure. */
  void *cls;
};

/**
 * Creates a new mathematical expression evaluator handle.
 * \return New mathematical expression evaluator handle.
 * \retval NULL If no memory space is available and set the `errno` to
 * `ENOMEM`.
 */
SG_EXTERN struct sg_expr *sg_expr_new(void) __SG_MALLOC;

/**
 * Frees the mathematical expression evaluator handle previously allocated by
 * #sg_expr_new().
 * \param[in] expr Expression evaluator handle.
 */
SG_EXTERN void sg_expr_free(struct sg_expr *expr);

/**
 * Compiles a mathematical expression allowing to declare variables, macros and
 * extensions.
 * \param[in] expr Mathematical expression instance.
 * \param[in] str Null-terminated string with the mathematical expression to be
 * compiled.
 * \param[in] len Length of the mathematical expression to be compiled.
 * \param[in] extensions Array of extensions to extend the evaluator.
 * \note The extension array must be terminated by a zeroed item.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOENT Missing a needed extenssion.
 * \retval EALREADY Mathematical expression already compiled.
 * \retval ENOMEM Out of memory.
 */
SG_EXTERN int sg_expr_compile(struct sg_expr *expr, const char *str, size_t len,
                              struct sg_expr_extension *extensions);

/**
 * Clears a mathematical expression instance.
 * \param[in] expr Mathematical expression instance.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_expr_clear(struct sg_expr *expr);

/**
 * Evaluates a compiled mathematical expression.
 * \param[in] expr Compiled mathematical expression.
 * \return Floating-point value of the evaluated mathematical expression.
 * \retval NAN If the \pr{expr} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN double sg_expr_eval(struct sg_expr *expr);

/**
 * Gets the value of a declared variable.
 * \param[in] expr Mathematical expression instance.
 * \param[in] name Name of the declared variable.
 * \param[in] len Length of the variable name.
 * \retval NAN
 *  - If \pr{expr} or \pr{name} is null, or \pr{len} is less than one, and set
 *  the `errno` to `EINVAL`.
 *  - If no memory space is available and set the `errno` to `ENOMEM`.
 */
SG_EXTERN double sg_expr_var(struct sg_expr *expr, const char *name,
                             size_t len);

/**
 * Sets a variable to the mathematical expression.
 * \param[in] expr Mathematical expression instance.
 * \param[in] name Name for the variable.
 * \param[in] len Length of the variable name.
 * \param[in] val Value for the variable.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 * \retval ENOMEM Out of memory.
 */
SG_EXTERN int sg_expr_set_var(struct sg_expr *expr, const char *name,
                              size_t len, double val);

/**
 * Gets a function argument by its index.
 * \param[in] args Arguments list.
 * \param[in] index Argument index.
 * \retval NAN If the \pr{args} is null or \pr{index} is less than zero and
 * set the `errno` to `EINVAL`.
 */
SG_EXTERN double sg_expr_arg(struct sg_expr_argument *args, int index);

/**
 * Returns the nearby position of an error in the mathematical expression.
 * \param[in] expr Compiled mathematical expression.
 * \retval 0 Success.
 * \retval EINVAL Invalid argument.
 */
SG_EXTERN int sg_expr_near(struct sg_expr *expr);

/**
 * Returns the type of an error in the mathematical expression.
 * \param[in] expr Compiled mathematical expression.
 * \return Enumerator representing the error type.
 * \retval SG_EXPR_ERR_UNKNOWN If the \pr{expr} is null or an unknown error
 * occurred and set the `errno` to `EINVAL`.
 */
SG_EXTERN enum sg_expr_err_type sg_expr_err(struct sg_expr *expr);

/**
 * Returns the description of an error in the mathematical expression.
 * \param[in] expr Compiled mathematical expression.
 * \return Description of the error as a null-terminated string.
 * \retval NULL If the \pr{expr} is null and set the `errno` to `EINVAL`.
 */
SG_EXTERN const char *sg_expr_strerror(struct sg_expr *expr);

/**
 * Returns the evaluated value of a mathematical expression.
 * \param[in] str Null-terminated string with the mathematical expression to be
 * evaluated.
 * \param[in] len Length of the mathematical expression to be evaluated.
 * \retval NAN If \pr{str} is null, or \pr{len} is less than one, and set the
 * `errno` to `EINVAL`.
 */
SG_EXTERN double sg_expr_calc(const char *str, size_t len);

/** \} */

#endif /* SG_MATH_EXPR_EVAL */

#ifdef __cplusplus
}
#endif

#endif /* SAGUI_H */
