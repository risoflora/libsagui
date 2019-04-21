# Usage:
#
# For 32-bit:
#
# cmake -DCMAKE_TOOLCHAIN_FILE="../cmake/Toolchain-mingw32-Linux.cmake" ..
# make sagui install/strip
#
# For 64-bit:
#
# cmake -DMINGW64=ON -DCMAKE_TOOLCHAIN_FILE="../cmake/Toolchain-mingw32-Linux.cmake" ..
# make sagui install/strip

#                         _
#   ___  __ _  __ _ _   _(_)
#  / __|/ _` |/ _` | | | | |
#  \__ \ (_| | (_| | |_| | |
#  |___/\__,_|\__, |\__,_|_|
#             |___/
#
# Cross-platform library which helps to develop web servers or frameworks.
#
# Copyright (c) 2016-2019 Silvio Clecio <silvioprog@gmail.com>
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

if (__SG_TOOLCHAIN_MINGW32_LINUX_INCLUDED)
    return()
endif ()
set(__SG_TOOLCHAIN_MINGW32_LINUX_INCLUDED ON)

if (NOT DEFINED SG_USE_OUTPUT_PREFIX)
    set(SG_USE_OUTPUT_PREFIX ON)
endif ()

if (MINGW64)
    set(MINGW_PREFIX "x86_64-w64-mingw32")
elseif (NOT MINGW_PREFIX)
    set(MINGW_PREFIX "i686-w64-mingw32")
endif ()

set(CMAKE_SYSTEM_NAME Windows)

if (NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER ${MINGW_PREFIX}-gcc)
endif ()
if (NOT CMAKE_RC_COMPILER)
    set(CMAKE_RC_COMPILER ${MINGW_PREFIX}-windres)
endif ()

if (NOT CMAKE_FIND_ROOT_PATH)
    set(_tmp_dir /usr/${MINGW_PREFIX}/sys-root/mingw)
    if (EXISTS ${_tmp_dir})
        set(CMAKE_FIND_ROOT_PATH ${_tmp_dir})
        if ("${MINGW_PREFIX}" STREQUAL "x86_64-w64-mingw32")
            set(SG_LIB_SUFFIX "64")
        endif ()
    else ()
        set(CMAKE_FIND_ROOT_PATH /usr/${MINGW_PREFIX})
    endif ()
    unset(_tmp_dir)
endif ()
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)