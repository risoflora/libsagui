#.rst:
# SgExamples
# ----------
#
# Library examples.
#
# All library examples building.
#
# ::
#
# SG_EXAMPLES_SOURCE_DIR - Directory containing the library examples.
# SG_EXAMPLES_CERTS_DIR - Directory containing the example certificates
# if -DSG_HTTPS_SUPPORT=ON.
# SG_EXAMPLES - All available examples.
# SG_EXAMPLES_SOURCE - All available example source.
# SG_BUILD_<EXAMPLE>_EXAMPLE - Enable/disable a <SG_EXAMPLE> listed
# by SG_EXAMPLES, e.g: -DSG_BUILD_STR_EXAMPLE=ON.

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

if(__SG_EXAMPLES_INCLUDED)
  return()
endif()
set(__SG_EXAMPLES_INCLUDED ON)

option(SG_BUILD_EXAMPLES "Enable the library examples building" ON)

if(SG_BUILD_EXAMPLES)
  if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  endif()
  set(SG_EXAMPLES_SOURCE_DIR ${CMAKE_SOURCE_DIR}/examples)
  list(
    APPEND
    SG_EXAMPLES
    str
    strmap
    httpauth
    httpcookie
    httpsrv
    httpuplds
    httpsrv_benchmark
    httpsrv_sse
    httpreq_payload
    httpreq_isolate)
  if(SG_HTTPS_SUPPORT AND GNUTLS_FOUND)
    set(SG_EXAMPLES_CERTS_DIR "${SG_EXAMPLES_SOURCE_DIR}/certs")
    add_definitions(-DSG_EXAMPLES_CERTS_DIR="${SG_EXAMPLES_CERTS_DIR}")
    list(APPEND SG_EXAMPLES httpsrv_tls)
    list(APPEND SG_EXAMPLES httpsrv_tls_cert_auth)
  endif()
  if(SG_HTTP_COMPRESSION)
    list(APPEND SG_EXAMPLES httpcomp)
  endif()
  if(SG_PATH_ROUTING)
    list(
      APPEND
      SG_EXAMPLES
      entrypoint
      router_simple
      router_segments
      router_vars
      router_srv)
  endif()
  if(SG_MATH_EXPR_EVAL)
    list(APPEND SG_EXAMPLES expr_basic)
    list(APPEND SG_EXAMPLES expr_full)
  endif()
  set(SG_EXAMPLES
      ${SG_EXAMPLES}
      PARENT_SCOPE)
  list(APPEND _libs sagui)
  foreach(_example ${SG_EXAMPLES})
    string(TOUPPER ${_example} _EXAMPLE)
    option(SG_BUILD_${_EXAMPLE}_EXAMPLE "Build sg_${_example} example" ON)
    if(SG_BUILD_${_EXAMPLE}_EXAMPLE)
      list(APPEND SG_EXAMPLES_SOURCE
           ${SG_EXAMPLES_SOURCE_DIR}/example_${_example}.c)
      add_executable(example_${_example}
                     ${SG_EXAMPLES_SOURCE_DIR}/example_${_example}.c)
      target_link_libraries(example_${_example} ${_libs})
    endif()
    unset(_EXAMPLE)
  endforeach()
  unset(_libs)
  set(SG_EXAMPLES_SOURCE
      ${SG_EXAMPLES_SOURCE}
      PARENT_SCOPE)
endif()
