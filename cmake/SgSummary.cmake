#.rst:
# SgSummary
# ---------
#
# Building summary.
#
# Summarizes the building options and prints it on screen.

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

if(__SG_SUMMARY_INCLUDED)
  return()
endif()
set(__SG_SUMMARY_INCLUDED ON)

set(_system_name "${CMAKE_SYSTEM} ${CMAKE_SYSTEM_PROCESSOR}")
if(NOT ${_system_name} MATCHES "${CMAKE_C_PLATFORM_ID}")
  string(CONCAT _system_name ${_system_name} " (${CMAKE_C_PLATFORM_ID})")
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" _BUILD_TYPE)
string(CONCAT _cflags "${CMAKE_C_FLAGS_${_BUILD_TYPE}}" " ${CMAKE_C_FLAGS}")
string(REPLACE "  " " " _cflags "${_cflags}")
unset(_BUILD_TYPE)

if(CMAKE_SIZEOF_VOID_P MATCHES "8")
  set(_build_arch "x86_64")
else()
  set(_build_arch "i686")
endif()

if(CMAKE_BUILD_TYPE)
  set(_build_type ${CMAKE_BUILD_TYPE})
else()
  set(_build_type "None")
endif()

if(BUILD_SHARED_LIBS)
  set(_lib_type "shared")
else()
  set(_lib_type "static")
endif()

if(${_build_type} MATCHES "[Dd]ebug|DEBUG")
  set(_is_debug ON)
endif()

if(${_build_type} MATCHES "[Rr]elease|RELEASE")
  set(_is_release ON)
endif()

if(SG_HTTPS_SUPPORT)
  if(GNUTLS_FOUND)
    set(_https_support "Yes")
  else()
    set(_https_support "No (missing library: GnuTLS)")
  endif()
else()
  set(_https_support "No")
endif()

if(SG_HTTP_COMPRESSION)
  set(_http_compression "Yes")
else()
  set(_http_compression "No")
endif()

if(SG_PATH_ROUTING)
  set(_routing "Yes")
else()
  set(_routing "No")
endif()

if(SG_MATH_EXPR_EVAL)
  set(_expr "Yes")
else()
  set(_expr "No")
endif()

if(SG_BUILD_EXAMPLES)
  set(_build_examples "Yes")
  if(SG_EXAMPLES)
    string(CONCAT _build_examples ${_build_examples} " (${SG_EXAMPLES})")
    string(REPLACE ";" ", " _build_examples "${_build_examples}")
  endif()
else()
  set(_build_examples "No")
endif()

if(SG_BUILD_HTML AND SG_GENERATE_HTML)
  if(_is_release)
    set(_build_html "Yes")
  else()
    set(_build_html "No (unavailable in build type: ${_build_type})")
  endif()
else()
  set(_build_html "No")
endif()

if(BUILD_TESTING)
  if(_is_debug)
    set(_build_testing "Yes")
    if(SG_TESTS)
      string(CONCAT _build_testing ${_build_testing} " (${SG_TESTS})")
      string(REPLACE ";" ", " _build_testing "${_build_testing}")
    endif()
  else()
    set(_build_testing
        "${_build_testing} (unavailable in build type: ${_build_type})")
  endif()
else()
  set(_build_testing "No")
endif()

if(CURL_VERSION_STRING AND CURL_LIBRARY)
  string(COMPARE GREATER_EQUAL ${CURL_VERSION_STRING} "7.56" _curl)
  if(_curl)
    set(_curl "Yes")
  else()
    set(_curl "No (requires 7.56 or higher)")
  endif()
else()
  set(_curl "No")
endif()

message(
  "
Sagui library ${VERSION} - building summary:

  Generator: ${CMAKE_GENERATOR}
  Install: ${CMAKE_INSTALL_PREFIX}
  System: ${_system_name}
  Compiler details:
    Executable: ${CMAKE_C_COMPILER}
    Version: ${CMAKE_C_COMPILER_VERSION}
    Machine: ${CMAKE_C_MACHINE}
    CFLAGS: ${_cflags}
  Build: ${_build_type}-${_build_arch} (${_lib_type})
  Additional features:
    HTTPS support: ${_https_support}
    HTTP compression: ${_http_compression}
    Path routing: ${_routing}
    Math expression evaluator: ${_expr}
  Examples: ${_build_examples}
  Docs: ${_build_html}
  Run tests: ${_build_testing}
  cURL tests: ${_curl}
")

unset(_system_name)
unset(_cflags)
unset(_build_type)
unset(_build_arch)
unset(_lib_type)
unset(_https_support)
unset(_http_compression)
unset(_routing)
unset(_expr)
unset(_build_examples)
unset(_build_html)
unset(_build_testing)
unset(_curl)
