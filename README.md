# `libsagui`

[![License: LGPL v2.1][license-badge]](LICENSE)
[![CII Best Practices][bestpractices-badge]][1]
[![Language grade: C/C++][lgtm-badge]][2]
[![GitHub releases][releases-badge]][3]
[![Build status][build-status-badge]][4]

## Overview

Sagui is a cross-platform C library which helps to develop web servers or
frameworks. Its core has been developed using the [GNU libmicrohttpd][5],
[uthash][6], [PCRE2][7], [ZLib][8] and [GnuTLS][9], that's why it is so fast,
compact and useful to run on embedded systems.

## Features

- **Requests processing through:**
  - Event-driven - single-thread + polling.
  - Threaded - one thread per request.
  - Polling - pre-allocated threads.
  - Isolated request - request processed outside main thread.
- **High-performance path routing that supports:**
  - Regular expressions using [PCRE2][10] [syntax][11].
  - Just-in-time optimization ([JIT][12]).
  - Binary search in path entry-points.
- **HTTP compression:**
  - [Deflate][13] for static contents and
    streams compression.
  - [Gzip][14] for files compression.
- **HTTPS support:**
  - TLS 1.3 through [GnuTLS][9] library.
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

All stable binaries are available for download at the [releases page][3] with
their respective checksums. For other systems, the packages
`Source code (tar.gz|zip)` contains the library source.

## Building/installing

The easiest way to build the library is using a Docker container as a builder.
Follow the instructions at [libsagui-docker/README.md][18] for more details.

Check the [docs/BUILD.md](docs/BUILD.md) for more instructions for how to build
the examples, tests, documentation and the library. Also, take a look at
[docs/INSTALL.md](docs/INSTALL.md) for how to install the library from sources
on your system.

## Documentation

The documentation has been written in [Doxygen][16] and is available in HTML
format at [libsagui-docs/index.html][17].

## Versioning

Starting from the version 1.0.0, Sagui follows the [SemVer][15] rules regarding
API changes with backwards compatibility and stable ABI across major releases.

## Compatibility

A typical upgrade of the Sagui library does not break the ABI at all. Take a
look at the [API/ABI compatibility report][19] to compare most recent library
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

[![Support this project via PayPal][paypal-gif]][20]

Check the list of [all donors](DONORS) that lovely supported this idea! :heart:

## Support

This project values being simple, direct and self-explanatory. However, if you
need some help to integrate Sagui to your application, we have the option of a
paid consulting service. [Contact us][21]!

## Projects using Sagui

- [Brook framework][22] - Pascal framework which helps to develop web
  applications. [[LGPL v2.1][23]]

Would you like to add your project to that list above? Feel free to open a
[new issue][24] requesting it! :-)

## Licensing

Sagui is released under GNU Lesser General Public License v2.1. Check the
[LICENSE file](LICENSE) for more details.

[license-badge]: https://img.shields.io/badge/license-LGPL%20v2.1-lemmon.svg
[bestpractices-badge]: https://bestpractices.coreinfrastructure.org/projects/2140/badge
[lgtm-badge]: https://img.shields.io/lgtm/grade/cpp/g/risoflora/libsagui.svg?logo=lgtm&logoWidth=18
[releases-badge]: https://img.shields.io/github/v/release/risoflora/libsagui?color=lemmon
[build-status-badge]: https://img.shields.io/github/workflow/status/risoflora/libsagui/CI "CI"
[paypal-gif]: https://www.paypalobjects.com/en_US/GB/i/btn/btn_donateCC_LG.gif
[1]: https://bestpractices.coreinfrastructure.org/projects/2140 "Best practices link"
[2]: https://lgtm.com/projects/g/risoflora/libsagui/context:cpp "LGTM link"
[3]: https://github.com/risoflora/libsagui/releases "Releases page"
[4]: https://github.com/risoflora/libsagui/actions/workflows/CI.yml "GitHub actions"
[5]: https://www.gnu.org/software/libmicrohttpd "libmicrohttpd page"
[6]: https://troydhanson.github.io/uthash "uthash page"
[7]: https://www.pcre.org "PCRE page"
[8]: https://www.zlib.net "ZLib page"
[9]: https://www.gnutls.org "GnuTLS page"
[10]: https://www.pcre.org/current/doc/html/pcre2pattern.html
[11]: https://www.pcre.org/current/doc/html/pcre2syntax.html
[12]: https://www.pcre.org/current/doc/html/pcre2jit.html
[13]: https://en.wikipedia.org/wiki/DEFLATE "DEFLATE wiki"
[14]: https://en.wikipedia.org/wiki/Gzip "Gzip wiki"
[15]: https://semver.org "Semantic Versioning page"
[16]: https://www.doxygen.nl/index.html "Doxygen page"
[17]: https://risoflora.github.io/libsagui-docs/index.html "Sagui documentation"
[18]: https://github.com/risoflora/libsagui-docker/blob/main/README.md "Sagui Docker"
[19]: https://abi-laboratory.pro/?view=timeline&l=libsagui "Sagui ABI status"
[20]: https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=silvioprog%40gmail%2ecom&lc=US&item_name=libsagui&item_number=libsagui&currency_code=USD&bn=PP%2dDonationsBF%3aproject%2dsupport%2ejpg%3aNonHosted "PayPal link"
[21]: mailto:silvioprog@gmail.com
[22]: https://github.com/risoflora/brookframework
[23]: https://github.com/risoflora/brookframework/blob/main/LICENSE
[24]: https://github.com/risoflora/libsagui/issues/new?labels=documentation&template=project_using_sagui.md
