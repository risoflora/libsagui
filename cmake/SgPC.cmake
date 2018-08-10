#.rst:
# SgPC
# ----
#
# `pkg-config` file generation.
#
# Generates and install the `libsagui.pc` file. When it is successfully installed, the `pkg-config` tool can be used to
# get info regarding the library, e.g: `pkg-config libsagui --modversion` returns the library version. More:
# [`pkg-config(1)`](https://linux.die.net/man/1/pkg-config).

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

if (__SG_PC_INCLUDED)
    return()
endif ()
set(__SG_PC_INCLUDED ON)

configure_file(${CMAKE_MODULE_PATH}/libsagui.pc.cmake.in
        ${CMAKE_BINARY_DIR}/libsagui.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/libsagui.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig COMPONENT dev)