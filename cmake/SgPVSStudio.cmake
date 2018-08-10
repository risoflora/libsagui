#.rst:
# SgPVSStudio
# -----------
#
# PVS-Studio analysis.
#
# This script allows to check the library sources using the PVS-Studio, offering more security for Sagui library users.

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

if (__SG_PVS_STUDIO_INCLUDED)
    return()
endif ()
set(__SG_PVS_STUDIO_INCLUDED ON)

option(SG_PVS_STUDIO "Enable PVS-Studio analysis" OFF)

if (SG_PVS_STUDIO)
    include(PVS-Studio)
    if (NOT CMAKE_EXPORT_COMPILE_COMMANDS)
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    endif ()
    include_directories(${SG_SOURCE_DIR})
    pvs_studio_add_target(TARGET pvs_studio_analysis ALL
            FORMAT fullhtml
            ANALYZE ${PROJECT_NAME}
            SOURCES ${SG_SOURCE} ${SG_TESTS_SOURCE}
            LOG pvs_studio_fullhtml)
endif ()