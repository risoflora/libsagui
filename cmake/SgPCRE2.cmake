#.rst:
# SgPCRE2
# -------
#
# Build PCRE2.
#
# Build PCRE2 from Cordel building.
#
# ::
#
#   PCRE2_INCLUDE_DIR - Directory of includes.
#   PCRE2_ARCHIVE_LIB - AR archive library.
#   PCRE2_JIT_SUPPORT - Enable/disable JIT support.
#

#                         _
#   ___  __ _  __ _ _   _(_)
#  / __|/ _` |/ _` | | | | |
#  \__ \ (_| | (_| | |_| | |
#  |___/\__,_|\__, |\__,_|_|
#             |___/
#
#   –– an ideal C library to develop cross-platform HTTP servers.
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

if (__SG_PCRE2_INCLUDED)
    return()
endif ()
set(__SG_PCRE2_INCLUDED ON)

option(PCRE2_JIT_SUPPORT "Enable JIT support" ON)

set(PCRE2_NAME "pcre2")
set(PCRE2_VER "10.31")
set(PCRE2_FULL_NAME "${PCRE2_NAME}-${PCRE2_VER}")
set(PCRE2_URL "https://ftp.pcre.org/pub/pcre/${PCRE2_FULL_NAME}.tar.gz")
set(PCRE2_URL_MIRROR "ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/${PCRE2_FULL_NAME}.tar.gz")
set(PCRE2_SHA256 "e11ebd99dd23a7bccc9127d95d9978101b5f3cf0a6e7d25a1b1ca165a97166c4")
if (CMAKE_C_COMPILER)
    set(PCRE2_OPTIONS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER})
endif ()
if (CMAKE_RC_COMPILER)
    set(PCRE2_OPTIONS ${PCRE2_OPTIONS} -DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER})
endif ()
if (CMAKE_SYSTEM_NAME)
    set(PCRE2_OPTIONS ${PCRE2_OPTIONS} -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME})
endif ()
if (UNIX)
    set(PCRE2_OPTIONS ${PCRE2_OPTIONS} -DCMAKE_POSITION_INDEPENDENT_CODE=ON)
endif ()
if (ANDROID)
    set(PCRE2_OPTIONS ${PCRE2_OPTIONS}
            -DCMAKE_ANDROID_ARM_MODE=${CMAKE_ANDROID_ARM_MODE}
            -DCMAKE_SYSTEM_VERSION=${CMAKE_SYSTEM_VERSION}
            -DCMAKE_ANDROID_ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}
            -DCMAKE_ANDROID_STANDALONE_TOOLCHAIN=${CMAKE_ANDROID_STANDALONE_TOOLCHAIN})
endif ()
set(PCRE2_OPTIONS ${PCRE2_OPTIONS}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/${PCRE2_FULL_NAME}
        -DPCRE2_BUILD_PCRE2GREP=OFF
        -DPCRE2_BUILD_TESTS=OFF
        -DPCRE2_SUPPORT_JIT=ON
        -DPCRE2_SUPPORT_PCRE2GREP_JIT=OFF
        -DPCRE2_SUPPORT_PCRE2GREP_CALLOUT=OFF
        -DPCRE2_SUPPORT_UNICODE=OFF
        -DPCRE2_SUPPORT_VALGRIND=OFF
        -DPCRE2_SUPPORT_LIBBZ2=OFF
        -DPCRE2_SUPPORT_LIBZ=OFF
        -DPCRE2_SUPPORT_LIBEDIT=OFF
        -DPCRE2_SUPPORT_LIBREADLINE=OFF)

ExternalProject_Add(${PCRE2_FULL_NAME}
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
if (WIN32 AND (CMAKE_BUILD_TYPE MATCHES "[Dd]ebug|DEBUG"))
    set(PCRE2_SUFFIX "d")
endif ()
set(PCRE2_ARCHIVE_LIB ${INSTALL_DIR}/lib/lib${PCRE2_NAME}-8${PCRE2_SUFFIX}.a)
add_definitions(-DPCRE2_STATIC=1)
add_definitions(-DPCRE2_CODE_UNIT_WIDTH=8)
if (PCRE2_JIT_SUPPORT)
    add_definitions(-DPCRE2_JIT_SUPPORT=1)
endif ()
unset(INSTALL_DIR)