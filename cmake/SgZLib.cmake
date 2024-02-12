#.rst:
# SgZLib
# ------
#
# Build ZLib.
#
# Build ZLib from Sagui building.
#
# ::
#
# ZLIB_INCLUDE_DIR - Directory of includes.
# ZLIB_ARCHIVE_LIB - AR archive library.

#                         _
#   ___  __ _  __ _ _   _(_)
#  / __|/ _` |/ _` | | | | |
#  \__ \ (_| | (_| | |_| | |
#  |___/\__,_|\__, |\__,_|_|
#             |___/
#
# Cross-platform library which helps to develop web servers or frameworks.
#
# Copyright (C) 2016-2024 Silvio Clecio <silvioprog@gmail.com>
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

if(__SG_ZLIB_INCLUDED)
  return()
endif()
set(__SG_ZLIB_INCLUDED ON)

if(CMAKE_VERSION VERSION_GREATER "3.23")
  cmake_policy(SET CMP0135 NEW)
endif()

set(ZLIB_NAME "zlib")
set(ZLIB_VER "1.3.1")
set(ZLIB_FULL_NAME "${ZLIB_NAME}-${ZLIB_VER}")
set(ZLIB_URL "https://zlib.net/${ZLIB_FULL_NAME}.tar.gz")
set(ZLIB_URL_MIRROR
    "https://github.com/madler/zlib/releases/download/v${ZLIB_VER}/${ZLIB_FULL_NAME}.tar.gz"
)
set(ZLIB_SHA256
    "9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23")
if(${CMAKE_VERSION} VERSION_LESS "3.7")
  unset(ZLIB_URL_MIRROR)
endif()
if(CMAKE_C_COMPILER)
  set(ZLIB_OPTIONS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER})
endif()
if(CMAKE_RC_COMPILER)
  set(ZLIB_OPTIONS ${ZLIB_OPTIONS} -DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER})
endif()
if(CMAKE_SYSTEM_NAME)
  set(ZLIB_OPTIONS ${ZLIB_OPTIONS} -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
endif()
if(UNIX)
  set(ZLIB_OPTIONS ${ZLIB_OPTIONS} -DCMAKE_POSITION_INDEPENDENT_CODE=ON)
endif()
if(ANDROID)
  set(ZLIB_OPTIONS
      ${ZLIB_OPTIONS}
      -DCMAKE_ANDROID_ARM_MODE=${CMAKE_ANDROID_ARM_MODE}
      -DCMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION}
      -DCMAKE_ANDROID_ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}
      -DCMAKE_ANDROID_STANDALONE_TOOLCHAIN=${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}
  )
endif()
set(ZLIB_OPTIONS ${ZLIB_OPTIONS} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                 -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/${ZLIB_FULL_NAME})

ExternalProject_Add(
  ${ZLIB_FULL_NAME}
  URL ${ZLIB_URL} ${ZLIB_URL_MIRROR}
  URL_HASH SHA256=${ZLIB_SHA256}
  TIMEOUT 15
  DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/lib
  PREFIX ${CMAKE_BINARY_DIR}/${ZLIB_FULL_NAME}
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/${ZLIB_FULL_NAME}
  CMAKE_ARGS ${ZLIB_OPTIONS}
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON
  LOG_INSTALL ON)

ExternalProject_Get_Property(${ZLIB_FULL_NAME} INSTALL_DIR)
set(ZLIB_INCLUDE_DIR ${INSTALL_DIR}/include)
if(WIN32)
  set(ZLIB_NAME "zlibstatic")
else()
  set(ZLIB_NAME "z")
endif()
set(ZLIB_ARCHIVE_LIB ${INSTALL_DIR}/lib/lib${ZLIB_NAME}.a)
unset(INSTALL_DIR)
