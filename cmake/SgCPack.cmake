#.rst:
# SgCPack
# -------
#
# CPack variables for the Sagui library packaging distribution.
#
# Adds the CPack targets to generate the distribution package of the Sagui library. An extra added target `make dist`
# is just an intuitive alias to the `make package_source`.

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

if (__SG_CPACK_INCLUDED)
    return()
endif ()
set(__SG_CPACK_INCLUDED ON)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
set(CPACK_PACKAGE_VERSION_MAJOR ${SG_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SG_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${SG_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_NAME} ${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_FILE_NAME "lib${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
if (NOT CPACK_GENERATOR)
    set(CPACK_GENERATOR "TGZ;ZIP")
endif ()

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}")
if (NOT CPACK_SOURCE_GENERATOR)
    set(CPACK_SOURCE_GENERATOR ${CPACK_GENERATOR})
endif ()
if (NOT CPACK_SOURCE_IGNORE_FILES)
    set(CPACK_SOURCE_IGNORE_FILES
            "${CMAKE_BINARY_DIR}/"
            "/.idea/"
            "/.git/"
            "/.github/"
            "/build/"
            "/docs/"
            "/lib/"
            ".gitignore"
            ".travis.yml"
            "_config.yml"
            "cmake-build-*")
endif ()

add_custom_target(dist
        COMMAND ${CMAKE_BUILD_TOOL} package_source)

include(CPack)