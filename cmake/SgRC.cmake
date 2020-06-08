#.rst:
# SgRC
# ----
#
# RC file generation.
#
# Generates the RC file.
#
# ::
#
# SG_LIBSAGUI_RC - RC file path.

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

if(__SG_RC_INCLUDED)
  return()
endif()
set(__SG_RC_INCLUDED ON)

set(SG_LIBSAGUI_RC ${CMAKE_BINARY_DIR}/libsagui.rc)

if(${CMAKE_BUILD_TYPE} MATCHES "[Dd]ebug|DEBUG")
  set(FILEFLAGS VS_FF_DEBUG)
else()
  set(FILEFLAGS 0)
endif()

set(RC_FILE_DESC "Sagui library for")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(RC_FILE_DESC "${RC_FILE_DESC} Win64")
else()
  set(RC_FILE_DESC "${RC_FILE_DESC} Win32")
endif()

unset(RC_FILE_DESC_MODS)
if(SG_HTTPS_SUPPORT)
  list(APPEND RC_FILE_DESC_MODS "TLS")
endif()
if(SG_HTTP_COMPRESSION)
  list(APPEND RC_FILE_DESC_MODS "ZLIB")
endif()
if(SG_PATH_ROUTING)
  list(APPEND RC_FILE_DESC_MODS "PCRE2")
endif()
if(SG_MATH_EXPR_EVAL)
  list(APPEND RC_FILE_DESC_MODS "EXPR")
endif()
if(RC_FILE_DESC_MODS)
  string(REPLACE ";" "," RC_FILE_DESC_MODS "${RC_FILE_DESC_MODS}")
  set(RC_FILE_DESC "${RC_FILE_DESC} (${RC_FILE_DESC_MODS})")
endif()

configure_file(${CMAKE_MODULE_PATH}/libsagui.rc.cmake.in
               ${CMAKE_BINARY_DIR}/libsagui.rc @ONLY)

unset(FILEFLAGS)
unset(RC_FILE_DESC)
unset(RC_FILE_DESC_MODS)
