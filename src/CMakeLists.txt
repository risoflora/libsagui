#.rst:
# SgSource
# --------
#
# Library source.
#
# This script allows to build the library source and adds the targets to
# install it as static or shared library.
#
# ::
#
# SG_SOURCE_DIR - Directory containing the library source.
# SG_C_SOURCE - All pure C available source.
# SG_SOURCE - All available source.

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

if(__SG_SRC_INCLUDED)
  return()
endif()
set(__SG_SRC_INCLUDED ON)

if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

set(SG_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(SG_SOURCE_DIR
    ${SG_SOURCE_DIR}
    PARENT_SCOPE)

list(
  APPEND
  SG_C_SOURCE
  ${SG_SOURCE_DIR}/sg_utils.c
  ${SG_SOURCE_DIR}/sg_extra.c
  ${SG_SOURCE_DIR}/sg_str.c
  ${SG_SOURCE_DIR}/sg_strmap.c
  ${SG_SOURCE_DIR}/sg_httpauth.c
  ${SG_SOURCE_DIR}/sg_httpuplds.c
  ${SG_SOURCE_DIR}/sg_httpreq.c
  ${SG_SOURCE_DIR}/sg_httpres.c
  ${SG_SOURCE_DIR}/sg_httpsrv.c)
if(NOT HAVE_INET_NTOP)
  list(APPEND SG_C_SOURCE ${SG_SOURCE_DIR}/inet.c)
endif()
if(SG_PATH_ROUTING)
  list(APPEND SG_C_SOURCE ${SG_SOURCE_DIR}/sg_entrypoint.c
       ${SG_SOURCE_DIR}/sg_entrypoints.c ${SG_SOURCE_DIR}/sg_routes.c
       ${SG_SOURCE_DIR}/sg_router.c)
endif()
if(SG_MATH_EXPR_EVAL)
  list(APPEND SG_C_SOURCE ${SG_SOURCE_DIR}/sg_expr.c)
endif()
set(SG_C_SOURCE
    ${SG_C_SOURCE}
    PARENT_SCOPE)

list(APPEND SG_SOURCE ${SG_INCLUDE_DIR}/sagui.h ${SG_SOURCE_DIR}/sg_macros.h
     ${SG_C_SOURCE})
if(WIN32 AND BUILD_SHARED_LIBS)
  list(APPEND SG_SOURCE ${SG_LIBSAGUI_RC})
endif()
set(SG_SOURCE
    ${SG_SOURCE}
    PARENT_SCOPE)

add_library(sagui ${SG_SOURCE})
add_dependencies(sagui ${MHD_FULL_NAME})
list(APPEND _libs ${MHD_ARCHIVE_LIB})
if(UNIX)
  if(NOT ANDROID)
    list(APPEND _libs pthread)
  endif()
elseif(WIN32)
  list(APPEND _libs ws2_32)
endif()
if(SG_HTTPS_SUPPORT AND GNUTLS_FOUND)
  list(APPEND _libs ${GNUTLS_LIBRARIES})
endif()
if(SG_HTTP_COMPRESSION)
  add_dependencies(sagui ${ZLIB_FULL_NAME})
  list(APPEND _libs ${ZLIB_ARCHIVE_LIB})
endif()
if(SG_PATH_ROUTING)
  add_dependencies(sagui ${PCRE2_FULL_NAME})
  list(APPEND _libs ${PCRE2_ARCHIVE_LIB})
endif()
target_link_libraries(sagui ${_libs})
unset(_libs)
set_target_properties(sagui PROPERTIES PUBLIC_HEADER ${SG_INCLUDE_DIR}/sagui.h)
if(BUILD_SHARED_LIBS)
  set_target_properties(
    sagui PROPERTIES COMPILE_DEFINITIONS BUILDING_LIBSAGUI VERSION ${VERSION}
                     SOVERSION ${SOVERSION})
  if(WIN32)
    set_target_properties(sagui PROPERTIES SUFFIX ${VERSION_SUFFIX}.dll)
  endif()
endif()

install(
  TARGETS sagui
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT lib # Windows DLL
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT dev)
