#.rst:
# SgVersion
# ---------
#
# Versioning variables.
#
# Variables containing the Sagui library version.
#
# ::
#
#   SG_VERSION_MAJOR - Major version.
#   SG_VERSION_MINOR - Minor version.
#   SG_VERSION_PATCH - Patch version.
#   SOVERSION - Composed by "SG_VERSION_MAJOR".
#   VERSION - Composed by "SG_VERSION_MAJOR.SG_VERSION_MINOR.SG_VERSION_PATCH".
#   VERSION_SUFFIX - Composed by "-SG_VERSION_MAJOR" (Windows only).

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

if (__SG_VERSION_INCLUDED)
    return()
endif ()
set(__SG_VERSION_INCLUDED ON)

if (EXISTS ${SG_INCLUDE_DIR}/sagui.h)
    set(_version_list MAJOR MINOR PATCH)
    foreach (v ${_version_list})
        set(_regex_version "^#define SG_VERSION_${v} ([0-9])")
        file(STRINGS "${SG_INCLUDE_DIR}/sagui.h" SG_VERSION_${v} REGEX "${_regex_version}")
        string(REGEX REPLACE "${_regex_version}" "\\1" SG_VERSION_${v} "${SG_VERSION_${v}}")
        unset(_regex_version)
    endforeach ()
    set(SOVERSION ${SG_VERSION_MAJOR})
    set(VERSION ${SG_VERSION_MAJOR}.${SG_VERSION_MINOR}.${SG_VERSION_PATCH})
    if (WIN32)
        set(VERSION_SUFFIX -${SG_VERSION_MAJOR})
    endif ()
    unset(_version_list)
endif ()