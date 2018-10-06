#.rst:
# SgFFI
# -----
#
# Build libffi.
#
# Build libffi from Sagui building.
#
# ::
#
#   FFI_INCLUDE_DIR - Directory of includes.
#   FFI_ARCHIVE_LIB - AR archive library.
#   FFI_LEGACY - Use FFI version 3.2.1.
#

#                         _
#   ___  __ _  __ _ _   _(_)
#  / __|/ _` |/ _` | | | | |
#  \__ \ (_| | (_| | |_| | |
#  |___/\__,_|\__, |\__,_|_|
#             |___/
#
#   –– cross-platform library which helps to develop web servers or frameworks.
#
# Copyright (c) 2016-2018 Silvio Clecio <silvioprog@gmail.com>
#
# This file is part of Sagui library.
#
# Sagui library is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Sagui library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Sagui library.  If not, see <http://www.gnu.org/licenses/>.
#

if (__SG_FFI_INCLUDED)
    return()
endif ()
set(__SG_FFI_INCLUDED ON)

if (WIN32 AND (CMAKE_SIZEOF_VOID_P EQUAL 4))
    set(_ffi_legacy ON)
else ()
    set(_ffi_legacy OFF)
endif ()

option(SG_FFI_LEGACY "Use legacy FFI version 3.2.1" ${_ffi_legacy})

unset(_ffi_legacy)

set(FFI_NAME "libffi")
if (FFI_LEGACY)
    set(FFI_VER "3.2.1")
else ()
    set(FFI_VER "3.3-rc0")
endif ()
set(FFI_FULL_NAME "${FFI_NAME}-${FFI_VER}")
if (FFI_LEGACY)
    set(FFI_URL "https://sourceware.org/ftp/libffi/${FFI_FULL_NAME}.tar.gz")
    set(FFI_URL_MIRROR "ftp://sourceware.org/pub/libffi/${FFI_FULL_NAME}.tar.gz")
    set(FFI_SHA256 "d06ebb8e1d9a22d19e38d63fdb83954253f39bedc5d46232a05645685722ca37")
else ()
    set(FFI_URL "https://github.com/libffi/libffi/releases/download/v3.3-rc0/${FFI_FULL_NAME}.tar.gz")
    set(FFI_URL_MIRROR "https://github.com/libffi/libffi/releases/download/v3.3-rc0/${FFI_FULL_NAME}.tar.gz")
    set(FFI_SHA256 "403d67aabf1c05157855ea2b1d9950263fb6316536c8c333f5b9ab1eb2f20ecf")
endif ()

set(FFI_OPTIONS
        --enable-static=yes
        --enable-shared=no
        --disable-multi-os-directory)

if (MINGW)
    set(FFI_OPTIONS ${FFI_OPTIONS} --quiet)
    set(_log_configure OFF)
else ()
    set(_log_configure ON)
endif ()

ExternalProject_Add(${FFI_FULL_NAME}
        URL ${FFI_URL} ${FFI_URL_MIRROR}
        URL_HASH SHA256=${FFI_SHA256}
        TIMEOUT 15
        DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/lib
        PREFIX ${CMAKE_BINARY_DIR}/${FFI_FULL_NAME}
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/${FFI_FULL_NAME}
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --host=${CMAKE_C_MACHINE} --prefix=<INSTALL_DIR> ${FFI_OPTIONS}
        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ${_log_configure}
        LOG_BUILD ON
        LOG_INSTALL ON)

ExternalProject_Get_Property(${FFI_FULL_NAME} INSTALL_DIR)
set(FFI_INCLUDE_DIR ${INSTALL_DIR}/include)
set(FFI_ARCHIVE_LIB ${INSTALL_DIR}/lib/${FFI_NAME}.a)
unset(INSTALL_DIR)