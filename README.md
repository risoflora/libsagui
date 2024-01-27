# `libsagui`

[![License: LGPL v2.1][license-badge]](LICENSE)
[![CII Best Practices][bestpractices-badge]][1]
[![GitHub releases][releases-badge]][2]
[![Build status][build-status-badge]][3]

## Overview

Sagui is a cross-platform C library which helps to develop web servers or
frameworks. Its core has been developed using the [GNU libmicrohttpd][4],
[uthash][5], [PCRE2][6], [ZLib][7] and [GnuTLS][8], that's why it is so fast,
compact and useful to run on embedded systems.

## Features

- **Requests processing through:**
  - Event-driven - single-thread + polling.
  - Threaded - one thread per request.
  - Polling - pre-allocated threads.
  - Isolated request - request processed outside main thread.
- **High-performance path routing that supports:**
  - Regular expressions using [PCRE2][9] [syntax][10].
  - Just-in-time optimization ([JIT][11]).
  - Binary search in path entry-points.
- **HTTP compression:**
  - [Deflate][12] for static contents and
    streams compression.
  - [Gzip][13] for files compression.
- **HTTPS support:**
  - TLS 1.3 through [GnuTLS][8] library.
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

There are other examples available in the [`examples`](examples) directory.

## Downloading

All stable binaries are available for download at the [releases page][2] with
their respective checksums. For other systems, the packages
`Source code (tar.gz|zip)` contains the library source.

## Building/installing

The easiest way to build the library is using a Docker container as a builder.
Follow the instructions at [libsagui-docker/README.md][17] for more details.

Check the [docs/BUILD.md](docs/BUILD.md) for more instructions for how to build
the examples, tests, documentation and the library. Also, take a look at
[docs/INSTALL.md](docs/INSTALL.md) for how to install the library from sources
on your system.

## Documentation

The documentation has been written in [Doxygen][15] and is available in HTML
format at [libsagui-docs/index.html][16].

## Versioning

Starting from the version 1.0.0, Sagui follows the [SemVer][14] rules regarding
API changes with backwards compatibility and stable ABI across major releases.

## Compatibility

A typical upgrade of the Sagui library does not break the ABI at all. Take a
look at the [API/ABI compatibility report][18] to compare most recent library
versions.

See also [Checking backward API/ABI compatibility of Sagui library versions](docs/ABIComplianceChecker.md).

## Contributing

Sagui is totally open source and would not be possible without our
[contributors](THANKS). If you want to submit contributions, please fork the
project on GitHub and send a pull request. You retain the copyright on your
contributions.

## Donations

Many open source projects, large and small, receive donations to encourage their
authors, therefore, it would be not different in Sagui.

All money collected from donations are invested to the purchase of study
materials. This way, directly or indirectly, all knowledge acquired in the
studies influence the spread of this project.

If you want to support this project, please click the button below:

[![Support this project via PayPal][paypal-gif]][19]

Check the list of [all donors](DONORS) that lovely supported this idea! :heart:

## Support

This project values being simple, direct and self-explanatory. However, if you
need some help to integrate Sagui to your application, we have the option of a
paid consulting service. [Contact us][20]!

## Projects using Sagui

- [Brook framework][21] - Pascal framework which helps to develop web
  applications. [LGPL v2.1][22]

Would you like to add your project to that list above? Feel free to open a
[new issue][23] requesting it! 🙂

## Licensing

Sagui is released under GNU Lesser General Public License v2.1. Check the
[LICENSE file](LICENSE) for more details.

[license-badge]: https://img.shields.io/badge/license-LGPL%20v2.1-lemmon.svg
[bestpractices-badge]: https://bestpractices.coreinfrastructure.org/projects/2140/badge
[releases-badge]: https://img.shields.io/github/v/release/risoflora/libsagui?color=lemmon
[build-status-badge]: https://github.com/risoflora/libsagui/actions/workflows/CI.yml/badge.svg?style=flat-square "CI/CD"
[paypal-gif]: https://www.paypalobjects.com/en_US/GB/i/btn/btn_donateCC_LG.gif
[1]: https://bestpractices.coreinfrastructure.org/projects/2140 "Best practices link"
[2]: https://github.com/risoflora/libsagui/releases "Releases page"
[3]: https://github.com/risoflora/libsagui/actions/workflows/CI.yml "GitHub actions"
[4]: https://www.gnu.org/software/libmicrohttpd "libmicrohttpd page"
[5]: https://troydhanson.github.io/uthash "uthash page"
[6]: https://www.pcre.org "PCRE page"
[7]: https://www.zlib.net "ZLib page"
[8]: https://www.gnutls.org "GnuTLS page"
[9]: https://www.pcre.org/current/doc/html/pcre2pattern.html
[10]: https://www.pcre.org/current/doc/html/pcre2syntax.html
[11]: https://www.pcre.org/current/doc/html/pcre2jit.html
[12]: https://en.wikipedia.org/wiki/DEFLATE "DEFLATE wiki"
[13]: https://en.wikipedia.org/wiki/Gzip "Gzip wiki"
[14]: https://semver.org "Semantic Versioning page"
[15]: https://www.doxygen.nl/index.html "Doxygen page"
[16]: https://risoflora.github.io/libsagui-docs/index.html "Sagui documentation"
[17]: https://github.com/risoflora/libsagui-docker/blob/master/README.md "Sagui Docker"
[18]: https://abi-laboratory.pro/?view=timeline&l=libsagui "Sagui ABI status"
[19]: https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=silvioprog%40gmail%2ecom&lc=US&item_name=libsagui&item_number=libsagui&currency_code=USD&bn=PP%2dDonationsBF%3aproject%2dsupport%2ejpg%3aNonHosted "PayPal link"
[20]: mailto:silvioprog@gmail.com
[21]: https://github.com/risoflora/brookframework
[22]: https://github.com/risoflora/brookframework/blob/master/LICENSE
[23]: https://github.com/risoflora/libsagui/issues/new?labels=documentation&template=project_using_sagui.md
