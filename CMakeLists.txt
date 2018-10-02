#.rst:
# SgLibrary
# ---------
#
# Main library building.
#
# The main building of the Sagui library. It includes all necessary sub-bulding scripts to manage the library building.
#
# ::
#
#   SG_INCLUDE_DIR - Directory containing the library header.

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

cmake_minimum_required(VERSION 3.5)

project(sagui C)

set(CMAKE_C_STANDARD 99)

set(PROJECT_DESCRIPTION "–– cross-platform library which helps to develop web servers or frameworks.")
set(PROJECT_VENDOR "The Sagui Library Development Team")
set(PROJECT_URL "https://github.com/risoflora/libsagui")
set(PROJECT_ISSUES_URL "${PROJECT_URL}/issues")
set(SG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_COMPILER_IS_CLANG ON)
endif ()

if (NOT CMAKE_C_MACHINE)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpmachine
            OUTPUT_VARIABLE _machine
            RESULT_VARIABLE _result
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
    if (_result EQUAL 0)
        set(CMAKE_C_MACHINE "${_machine}") # the machine which building for
    endif ()
    unset(_machine)
    unset(_result)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(SG_HTTPS_SUPPORT "Enable HTTPS support" OFF)
option(SG_PATH_ROUTING "Enable path routing" ON)

include(GNUInstallDirs)
include(ExternalProject)
include(SgFlags)
include(SgGNUSource)
include(SgVersion)
include(SgABIComplianceChecker)
if (SG_HTTPS_SUPPORT)
    include(FindGnuTLS)
    if (GNUTLS_FOUND)
        add_definitions(-DSG_HTTPS_SUPPORT=1)
    endif ()
endif ()
include(SgMHD)
if (SG_PATH_ROUTING)
    include(SgPCRE2)
    add_definitions(-DSG_PATH_ROUTING=1)
endif ()
if (WIN32 AND BUILD_SHARED_LIBS)
    include(SgRC)
endif ()
include(SgPC)
include(SgUninstall)

if (CMAKE_BUILD_TYPE MATCHES "[Rr]elease|RELEASE")
    set(BUILD_TESTING OFF)
elseif (BUILD_TESTING)
    include(FindCURL)
    add_definitions(-DBUILD_TESTING=1)
    enable_testing()
endif ()

if (UNIX AND ((NOT APPLE) AND (NOT ANDROID)))
    include(CheckIncludeFiles)
    check_include_files(errno.h HAVE_ERRNO_H)
    if (NOT HAVE_ERRNO_H)
        include_directories(/usr/include/asm-generic)
    endif ()
endif ()

include_directories(${SG_INCLUDE_DIR})
include_directories(${MHD_INCLUDE_DIR})
if (SG_PATH_ROUTING)
    include_directories(${PCRE2_INCLUDE_DIR})
endif ()

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(test)

include(SgDoxygen)
include(SgSummary)
include(SgCPack)
include(SgPVSStudio)