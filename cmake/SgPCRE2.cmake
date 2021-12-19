#.rst:
# SgPCRE2
# -------
#
# Build PCRE2.
#
# Build PCRE2 from Sagui building.
#
# ::
#
# PCRE2_INCLUDE_DIR - Directory of includes.
# PCRE2_ARCHIVE_LIB - AR archive library.
# PCRE2_JIT_SUPPORT - Enable/disable JIT support.

#                         _
#   ___  __ _  __ _ _   _(_)
#  / __|/ _` |/ _` | | | | |
#  \__ \ (_| | (_| | |_| | |
#  |___/\__,_|\__, |\__,_|_|
#             |___/
#
# Cross-platform library which helps to develop web servers or frameworks.
#
# Copyright (C) 2016-2021 Silvio Clecio <silvioprog@gmail.com>
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

if(__SG_PCRE2_INCLUDED)
  return()
endif()
set(__SG_PCRE2_INCLUDED ON)

option(PCRE2_JIT_SUPPORT "Enable JIT support" ON)

set(PCRE2_NAME "pcre2")
set(PCRE2_VER "10.39")
set(PCRE2_FULL_NAME "${PCRE2_NAME}-${PCRE2_VER}")
set(PCRE2_URL
    "https://github.com/PhilipHazel/pcre2/releases/download/${PCRE2_FULL_NAME}/${PCRE2_FULL_NAME}.tar.gz"
)
set(PCRE2_URL_MIRROR
    "https://github.com/PhilipHazel/pcre2/releases/download/${PCRE2_FULL_NAME}/${PCRE2_FULL_NAME}.tar.gz"
)
set(PCRE2_SHA256
    "0781bd2536ef5279b1943471fdcdbd9961a2845e1d2c9ad849b9bd98ba1a9bd4")
if(${CMAKE_VERSION} VERSION_LESS "3.7")
  unset(PCRE2_URL_MIRROR)
endif()
if(CMAKE_C_COMPILER)
  set(PCRE2_OPTIONS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER})
endif()
if(CMAKE_RC_COMPILER)
  set(PCRE2_OPTIONS ${PCRE2_OPTIONS} -DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER})
endif()
if(CMAKE_SYSTEM_NAME)
  set(PCRE2_OPTIONS ${PCRE2_OPTIONS} -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
endif()
if(UNIX)
  set(PCRE2_OPTIONS ${PCRE2_OPTIONS} -DCMAKE_POSITION_INDEPENDENT_CODE=ON)
endif()
if(ANDROID)
  set(PCRE2_OPTIONS
      ${PCRE2_OPTIONS}
      -DCMAKE_ANDROID_ARM_MODE=${CMAKE_ANDROID_ARM_MODE}
      -DCMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION}
      -DCMAKE_ANDROID_ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}
      -DCMAKE_ANDROID_STANDALONE_TOOLCHAIN=${CMAKE_ANDROID_STANDALONE_TOOLCHAIN}
  )
endif()
set(PCRE2_OPTIONS
    ${PCRE2_OPTIONS}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/${PCRE2_FULL_NAME}
    -DPCRE2_BUILD_PCRE2GREP=OFF
    -DPCRE2_BUILD_TESTS=OFF
    -DPCRE2_SUPPORT_JIT=${PCRE2_JIT_SUPPORT}
    -DPCRE2_SUPPORT_UNICODE=OFF
    -DPCRE2_SUPPORT_VALGRIND=OFF
    -DPCRE2_SUPPORT_LIBBZ2=OFF
    -DPCRE2_SUPPORT_LIBZ=OFF
    -DPCRE2_SUPPORT_LIBEDIT=OFF
    -DPCRE2_SUPPORT_LIBREADLINE=OFF)

ExternalProject_Add(
  ${PCRE2_FULL_NAME}
  URL ${PCRE2_URL} ${PCRE2_URL_MIRROR}
  URL_HASH SHA256=${PCRE2_SHA256}
  TIMEOUT 15
  DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/lib
  PREFIX ${CMAKE_BINARY_DIR}/${PCRE2_FULL_NAME}
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/${PCRE2_FULL_NAME}
  CMAKE_ARGS ${PCRE2_OPTIONS}
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON
  LOG_INSTALL ON)

ExternalProject_Get_Property(${PCRE2_FULL_NAME} INSTALL_DIR)
set(PCRE2_INCLUDE_DIR ${INSTALL_DIR}/include)
if(WIN32 AND (CMAKE_BUILD_TYPE MATCHES "[Dd]ebug|DEBUG"))
  set(PCRE2_SUFFIX "d")
endif()
set(PCRE2_ARCHIVE_LIB ${INSTALL_DIR}/lib/lib${PCRE2_NAME}-8${PCRE2_SUFFIX}.a)
add_definitions(-DPCRE2_STATIC=1)
add_definitions(-DPCRE2_CODE_UNIT_WIDTH=8)
if(PCRE2_JIT_SUPPORT)
  add_definitions(-DPCRE2_JIT_SUPPORT=1)
endif()
unset(INSTALL_DIR)
