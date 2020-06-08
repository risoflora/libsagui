# `libsagui`

[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-lemmon.svg)](https://github.com/risoflora/libsagui/blob/master/LICENSE)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/2140/badge)](https://bestpractices.coreinfrastructure.org/projects/2140)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/risoflora/libsagui.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/risoflora/libsagui/context:cpp)
[![GitHub releases](https://img.shields.io/github/v/release/risoflora/libsagui?color=lemmon)](https://github.com/risoflora/libsagui/releases)
[![Build status](https://travis-ci.org/risoflora/libsagui.svg?branch=master)](https://travis-ci.org/risoflora/libsagui)

## Overview

Sagui is a cross-platform C library which helps to develop web servers or
frameworks. Its core has been developed using the [GNU libmicrohttpd](https://www.gnu.org/software/libmicrohttpd),
[uthash](https://troydhanson.github.io/uthash), [PCRE2](https://www.pcre.org),
[ZLib](https://www.zlib.net) and [GnuTLS](https://www.gnutls.org), that's why it
is so fast, compact and useful to run on embedded systems.

## Features

- **Requests processing through:**
  - Event-driven - single-thread + polling.
  - Threaded - one thread per request.
  - Polling - pre-allocated threads.
  - Isolated request - request processed outside main thread.
- **High-performance path routing that supports:**
  - Regular expressions using [PCRE2](https://www.pcre.org/current/doc/html/pcre2pattern.html)
    [syntax](https://www.pcre.org/current/doc/html/pcre2syntax.html).
  - Just-in-time optimization ([JIT](https://www.pcre.org/current/doc/html/pcre2jit.html)).
  - Binary search in path entry-points.
- **HTTP compression:**
  - [Deflate](https://en.wikipedia.org/wiki/DEFLATE) for static contents and
    streams compression.
  - [Gzip](https://en.wikipedia.org/wiki/Gzip) for files compression.
- **HTTPS support:**
  - TLS 1.3 through [GnuTLS](https://www.gnutls.org) library.
- **Dual stack:**
  - Single socket for IPv4 and IPv6 support.
- **Basic authentication:**
  - For standard login using _user name/password_.
- **Upload/download streaming by:**
  - Payload - for raw data transferring as JSON, XML and other.
  - File - for large data transferring as videos, images, binaries and so on.
- **Mathematical expression evaluator:**
  - Arithmetic, bitwise and logical operators.
  - Variables allocation at build and/or run time.
  - Macro support to define functions at run time.
  - Extendable with custom functions.
  - Error handling with error kind and position.
- **Dynamic strings:**
  - Makes it easy strings operations in C.
- **String map:**
  - Fast key-value mapping.
- **And more:**
  - Fields, parameters, cookies, headers under hash table structure.
  - Several callbacks for total library customization.

## Examples

A minimal `hello world` HTTP server:

```c
void req_cb(void *cls, struct sg_httpreq *req, struct sg_httpres *res) {
    sg_httpres_send(res, "Hello world", "text/plain", 200);
}

int main(void) {
    struct sg_httpsrv *srv = sg_httpsrv_new(req_cb, NULL);
    sg_httpsrv_listen(srv, 8080, false);
    printf("Server running at http://localhost:%d\n", sg_httpsrv_port(srv));
    getchar();
    sg_httpsrv_free(srv);
    return 0;
}
```

The router support is isolated from the HTTP feature, so it can be used to route
any path structure, for example:

```c
void home_cb(void *cls, struct sg_route *route) {
    printf("Home\n");
}

void download_cb(void *cls, struct sg_route *route) {
    printf("Download\n");
}

int main(void) {
    struct sg_router *router;
    struct sg_route *routes = NULL;
    sg_routes_add(&routes, "/home", home_cb, NULL);
    sg_routes_add(&routes, "/download", download_cb, NULL);
    router = sg_router_new(routes);
    sg_router_dispatch(router, "/home", NULL);
    sg_routes_cleanup(&routes);
    sg_router_free(router);
    return 0;
}
```

lastly, putting everything together:

```c
struct Holder {
    struct sg_httpreq *req;
    struct sg_httpres *res;
};

void route_home_cb(void *cls, struct sg_route *route) {
    struct Holder *holder = sg_route_user_data(route);
    sg_httpres_send(holder->res, "Home", "text/plain", 200);
}

void route_download_cb(void *cls, struct sg_route *route) {
    struct Holder *holder = sg_route_user_data(route);
    sg_httpres_send(holder->res, "Download", "text/plain", 200);
}

void req_cb(void *cls, struct sg_httpreq *req, struct sg_httpres *res) {
    struct sg_router *router = cls;
    struct Holder holder = {req, res};
    if (sg_router_dispatch(router, sg_httpreq_path(req), &holder) != 0)
        sg_httpres_send(res, "404", "text/plain", 404);
}

int main(void) {
    struct sg_route *routes = NULL;
    struct sg_router *router;
    struct sg_httpsrv *srv;
    sg_routes_add(&routes, "/home", route_home_cb, NULL);
    sg_routes_add(&routes, "/download", route_download_cb, NULL);
    router = sg_router_new(routes);
    srv = sg_httpsrv_new(req_cb, router);
    sg_httpsrv_listen(srv, 8080, false);
    printf("Server running at http://localhost:%d\n", sg_httpsrv_port(srv));
    getchar();
    sg_httpsrv_free(srv);
    sg_routes_cleanup(&routes);
    sg_router_free(router);
    return 0;
}
```

There are other examples available in the
[`examples/`](https://github.com/risoflora/libsagui/tree/master/examples)
directory.

## Versioning

Starting from the version 1.0.0, Sagui follows the [SemVer](https://semver.org)
rules regarding API changes with backwards compatibility and stable ABI across
major releases.

## Licensing

Sagui is released under GNU Lesser General Public License v2.1. Check the
[LICENSE file](https://github.com/risoflora/libsagui/blob/master/LICENSE) for
more details.

## Documentation

The documentation has been written in [Doxygen](https://www.stack.nl/~dimitri/doxygen)
and is available in HTML format at [libsagui-docs/index.html](https://risoflora.github.io/libsagui-docs/index.html).

## Downloading

All stable binaries are available for download at the
[releases page](https://github.com/risoflora/libsagui/releases) with their
respective checksums. For other systems, the packages `Source code (tar.gz|zip)`
contains the library source.

## Building/installing

The easiest way to build the library is using a Docker container as builder.
Follow the instructions at [libsagui-docker/README.md](https://github.com/risoflora/libsagui-docker/blob/master/README.md)
for more details.

Check the [docs/BUILD.md](https://github.com/risoflora/libsagui/blob/master/docs/BUILD.md)
for more instructions for how to build the examples, tests, documentation and
the library. Also, take a look at [docs/INSTALL.md](https://github.com/risoflora/libsagui/blob/master/docs/INSTALL.md)
for how to install the library from sources on your system.

## Compatibility

A typical upgrade of the Sagui library does not break the ABI at all. Take a
look at the [API/ABI compatibility report](https://abi-laboratory.pro/?view=timeline&l=libsagui)
to compare most recent library versions.

See also [Checking backward API/ABI compatibility of Sagui library versions](https://github.com/risoflora/libsagui/blob/master/docs/ABIComplianceChecker.md).

## Projects using Sagui

- [Brook framework](https://github.com/risoflora/brookframework) - Pascal
  framework which helps to develop web applications.
  [[LGPL v2.1](https://github.com/risoflora/brookframework/blob/master/LICENSE)]

Would you like to add your project to that list above? Feel free to open a
[new issue](https://github.com/risoflora/libsagui/issues/new?labels=documentation&template=project_using_sagui.md)
requesting it! :-)

## Contributing

Sagui is totally open source and would not be possible without our
[contributors](https://github.com/risoflora/libsagui/blob/master/THANKS). If you
want to submit contributions, please fork the project on GitHub and send a pull
request. You retain the copyright on your contributions. If you have questions,
open a new issue at the [issues page](https://github.com/risoflora/libsagui/issues).
For donations to support this project, please click the button below.

[![Support this project](https://www.paypalobjects.com/en_US/GB/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=silvioprog%40gmail%2ecom&lc=US&item_name=libsagui&item_number=libsagui&currency_code=USD&bn=PP%2dDonationsBF%3aproject%2dsupport%2ejpg%3aNonHosted)

## Support

This project is completely self-explanatory, but, if you need a consulting
service to integrate it on your project, [contact us](mailto:silvioprog@gmail.com).
