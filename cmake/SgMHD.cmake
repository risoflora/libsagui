#.rst:
# SgMHD
# -----
#
# Build libmicrohttpd.
#
# Build libmicrohttpd from Sagui building.
#
# ::
#
# MHD_INCLUDE_DIR - Directory of includes.
# MHD_ARCHIVE_LIB - AR archive library.

#                         _
#   ___  __ _  __ _ _   _(_)
#  / __|/ _` |/ _` | | | | |
#  \__ \ (_| | (_| | |_| | |
#  |___/\__,_|\__, |\__,_|_|
#             |___/
#
# Cross-platform library which helps to develop web servers or frameworks.
#
# Copyright (C) 2016-2020 Silvio Clecio <silvioprog@gmail.com>
#
# Sagui library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Sagui library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Sagui library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#

if(__SG_MHD_INCLUDED)
  return()
endif()
set(__SG_MHD_INCLUDED ON)

set(MHD_NAME "libmicrohttpd")
set(MHD_VER "0.9.70")
set(MHD_FULL_NAME "${MHD_NAME}-${MHD_VER}")
set(MHD_URL "https://ftp.gnu.org/gnu/libmicrohttpd/${MHD_FULL_NAME}.tar.gz")
set(MHD_URL_MIRROR
    "https://espejito.fder.edu.uy/gnu/libmicrohttpd/${MHD_FULL_NAME}.tar.gz")
set(MHD_SHA256
    "90d0a3d396f96f9bc41eb0f7e8187796049285fabef82604acd4879590977307")
set(_libdir ${CMAKE_BINARY_DIR}/${MHD_FULL_NAME}/lib)
if(${CMAKE_VERSION} VERSION_LESS "3.7")
  unset(MHD_URL_MIRROR)
endif()
if(SG_HTTPS_SUPPORT AND GNUTLS_FOUND)
  set(_enable_https "yes")
else()
  set(_enable_https "no")
endif()
set(MHD_OPTIONS
    --libdir=${_libdir}
    --enable-static=yes
    --enable-shared=no
    --enable-messages=yes
    --enable-https=${_enable_https}
    --enable-asserts=no
    --enable-coverage=no
    --disable-httpupgrade
    --disable-dauth
    --disable-doc
    --disable-examples
    --disable-curl)
unset(_enable_https)
if(MINGW)
  set(MHD_OPTIONS ${MHD_OPTIONS} --quiet)
  set(_manifest_tool MANIFEST_TOOL=:)
  set(_log_configure OFF)
elseif(UNIX)
  #if (CMAKE_POSITION_INDEPENDENT_CODE)
  set(MHD_OPTIONS ${MHD_OPTIONS} --with-pic)
  #endif ()
  set(_log_configure ON)
else()
  set(_log_configure ON)
endif()

ExternalProject_Add(
  ${MHD_FULL_NAME}
  URL ${MHD_URL} ${MHD_URL_MIRROR}
  URL_HASH SHA256=${MHD_SHA256}
  TIMEOUT 15
  DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/lib
  PREFIX ${CMAKE_BINARY_DIR}/${MHD_FULL_NAME}
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/${MHD_FULL_NAME}
  CONFIGURE_COMMAND
    <SOURCE_DIR>/configure ${CC} ${_manifest_tool} --host=${CMAKE_C_MACHINE}
    --prefix=<INSTALL_DIR> ${MHD_OPTIONS}
  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
  INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ${_log_configure}
  LOG_BUILD ON
  LOG_INSTALL ON)

ExternalProject_Get_Property(${MHD_FULL_NAME} INSTALL_DIR)
set(MHD_INCLUDE_DIR ${INSTALL_DIR}/include)
set(MHD_ARCHIVE_LIB ${INSTALL_DIR}/lib/${MHD_NAME}.a)
unset(_libdir)
unset(INSTALL_DIR)
