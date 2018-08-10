#.rst:
# SgFlags
# -------
#
# Compiler/linker warnings.
#
# Configures the compiler/linker flags.
#
# ::
#
#   SG_PICKY_COMPILER - Enable/disable the compiler warnings.

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

#TODO: -fsanitize=address/leak

if (__SG_FLAGS_INCLUDED)
    return()
endif ()
set(__SG_FLAGS_INCLUDED ON)

option(SG_PICKY_COMPILER "Enable picky compiler options" ON)

if (MINGW)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static")
endif ()

if (BUILD_TESTING)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-multiple-definition")
endif ()

if (SG_PICKY_COMPILER)
    if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
        #-Wsign-conversion - needs to fix (un)signed bugs in utstring.h
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -Wpedantic -Wdeclaration-after-statement -Wstrict-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline")
        if (ANDROID)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-command-line-argument")
        endif ()
    else ()
        message(FATAL_ERROR "Unknown C compiler: ${CMAKE_C_COMPILER}")
    endif ()
endif ()