#.rst:
# SgLibrary
# ---------
#
# Main library building.
#
# The main building of the Sagui library. It includes all necessary sub-building
# scripts to manage the library building.
#
# ::
#
# SG_INCLUDE_DIR - Directory containing the library header.
# SG_USE_OUTPUT_PREFIX - Install all files into "./Output" directory.

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

cmake_minimum_required(VERSION 3.0.2)

project(sagui C)

set(CMAKE_C_STANDARD 99)

set(PROJECT_DESCRIPTION
    "Cross-platform library which helps to develop web servers or frameworks.")
set(PROJECT_VENDOR "The Sagui Library Development Team")
set(PROJECT_URL "https://github.com/risoflora/libsagui")
set(PROJECT_ISSUES_URL "${PROJECT_URL}/issues")
set(SG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
endif()
if((SG_USE_OUTPUT_PREFIX OR MINGW)
   AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
      "${CMAKE_BINARY_DIR}/Output"
      CACHE PATH "..." FORCE)
endif()

# Based on: https://gitlab.kitware.com/cmake/cmake/issues/19460
if(DEFINED CMAKE_OSX_SYSROOT)
  set(CC "CC=${CMAKE_C_COMPILER} ${CMAKE_C_SYSROOT_FLAG} ${CMAKE_OSX_SYSROOT}")
else()
  set(CC "CC=${CMAKE_C_COMPILER}")
endif()

if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_COMPILER_IS_CLANG ON)
endif()

if(NOT CMAKE_C_MACHINE)
  execute_process(
    COMMAND ${CMAKE_C_COMPILER} -dumpmachine
    OUTPUT_VARIABLE _machine
    RESULT_VARIABLE _result
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
  if(_result EQUAL 0)
    set(CMAKE_C_MACHINE "${_machine}") # the machine which building for
  endif()
  unset(_machine)
  unset(_result)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(SG_HTTPS_SUPPORT "Enable HTTPS support" OFF)
option(SG_HTTP_COMPRESSION "Enable HTTP compression" ON)
option(SG_PATH_ROUTING "Enable path routing" ON)
option(SG_MATH_EXPR_EVAL "Enable mathematical expression evaluator" ON)

include(GNUInstallDirs)
include(ExternalProject)
include(SgFlags)
include(SgGNUSource)
include(SgVersion)
include(SgABIComplianceChecker)
if(SG_HTTPS_SUPPORT)
  include(FindGnuTLS)
  if(GNUTLS_FOUND)
    add_definitions(-DSG_HTTPS_SUPPORT=1)
    if(MINGW)
      get_filename_component(_ext ${GNUTLS_LIBRARIES} EXT)
      if(_ext MATCHES ".dll.a")
        unset(_ext)
        get_filename_component(_ori_path ${GNUTLS_LIBRARIES} PATH)
        string(REGEX REPLACE "lib$" "bin" _path "${_ori_path}")
        unset(_ori_path)
        get_filename_component(_name ${GNUTLS_LIBRARIES} NAME_WE)
        string(CONCAT _basename ${_path} "/" ${_name} "-")
        unset(_path)
        unset(_name)
        string(CONCAT _fullname ${_basename} "20.dll")
        if(NOT ${_fullname})
          string(CONCAT _fullname ${_basename} "30.dll")
        endif()
        set(GNUTLS_LIBRARIES
            "${_fullname}"
            CACHE STRING "The libraries needed to use gnutls" FORCE)
        unset(_fullname)
      endif()
    endif()
  endif()
endif()
include(SgMHD)
if(SG_HTTP_COMPRESSION)
  include(SgZLib)
  add_definitions(-DSG_HTTP_COMPRESSION=1)
endif()
if(SG_PATH_ROUTING)
  include(SgPCRE2)
  add_definitions(-DSG_PATH_ROUTING=1)
endif()
if(SG_MATH_EXPR_EVAL)
  add_definitions(-DSG_MATH_EXPR_EVAL=1)
endif()
if(WIN32 AND BUILD_SHARED_LIBS)
  include(SgRC)
endif()
include(SgPC)
include(SgUninstall)

if(BUILD_TESTING)
  if(CMAKE_BUILD_TYPE MATCHES "[Dd]ebug|DEBUG")
    include(FindCURL)
    add_definitions(-DBUILD_TESTING=1)
    enable_testing()
  else()
    set(BUILD_TESTING OFF)
    message(STATUS "BUILD_TESTING disabled in build type ${CMAKE_BUILD_TYPE}")
  endif()
endif()

if(UNIX AND ((NOT APPLE) AND (NOT ANDROID)))
  include(CheckIncludeFiles)
  check_include_files(errno.h HAVE_ERRNO_H)
  if(NOT HAVE_ERRNO_H)
    include_directories(/usr/include/asm-generic)
  endif()
endif()

if(MINGW)
  message(STATUS "Looking for inet_ntop")
  set(_chk_inet_ntop_src "${CMAKE_BINARY_DIR}/check_inet_ntop.c")
  file(
    WRITE ${_chk_inet_ntop_src}
    "#include <stdio.h>\n#include <ws2tcpip.h>\nint main(void){inet_ntop(0,NULL,NULL,0);return 0;}"
  )
  try_compile(HAVE_INET_NTOP ${CMAKE_BINARY_DIR} ${_chk_inet_ntop_src}
              LINK_LIBRARIES ws2_32)
  file(REMOVE ${_chk_inet_ntop_src})
  unset(_chk_inet_ntop_src)
  if(${HAVE_INET_NTOP})
    message(STATUS "Looking for inet_ntop - found")
  else()
    message(STATUS "Looking for inet_ntop - not found")
  endif()
else()
  include(CheckIncludeFile)
  check_include_file(arpa/inet.h HAVE_ARPA_INET_H)
  if(HAVE_ARPA_INET_H)
    add_definitions(-DHAVE_ARPA_INET_H=1)
  endif()
  include(CheckFunctionExists)
  check_function_exists(inet_ntop HAVE_INET_NTOP)
  if(NOT HAVE_INET_NTOP)
    include(CheckSymbolExists)
    check_symbol_exists(inet_ntop "arpa/inet.h" HAVE_INET_NTOP)
  endif()
endif()
if(${HAVE_INET_NTOP})
  add_definitions(-DHAVE_INET_NTOP=1)
endif()

include_directories(${SG_INCLUDE_DIR})
include_directories(${MHD_INCLUDE_DIR})
if(SG_HTTP_COMPRESSION)
  include_directories(${ZLIB_INCLUDE_DIR})
endif()
if(SG_PATH_ROUTING)
  include_directories(${PCRE2_INCLUDE_DIR})
endif()

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(test)

include(SgDoxygen)
include(SgSummary)
include(SgCPack)
include(SgPVSStudio)
