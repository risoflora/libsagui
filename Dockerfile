######################################################################
# Copyright (c) 2024 Silvio Clecio (silvioprog) <silvioprog@gmail.com>
#
# SPDX-License-Identifier: MIT
######################################################################

# podman build --platform linux/amd64 -t hello_sagui .
# podman run --platform linux/amd64 --rm -it -p 8080:8080 hello_sagui

FROM alpine:3.19.1 AS builder

RUN apk add --no-cache \
  make \
  autoconf \
  automake \
  clang \
  cmake
WORKDIR /app
COPY . /app/
RUN mkdir build && \
  cd build/ && \
  cmake -DBUILD_SHARED_LIBS=OFF .. && \
  make example_httpsrv
RUN strip /app/build/examples/example_httpsrv

FROM scratch
WORKDIR /app
COPY --from=builder /lib/ld-musl-x86_64.so.1 /lib/ld-musl-x86_64.so.1
COPY --from=builder /app/build/examples/example_httpsrv .
ENTRYPOINT ["./example_httpsrv", "8080"]
