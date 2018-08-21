#.rst:
# SgABIComplianceChecker
# ----------------------
#
# Check backward API/ABI compatibility.
#
# A CMake script for checking backward API/ABI compatibility of the Sagui library.
#
# ::
#
#   SG_OLD_LIB_DIR - Absolute path of the old Sagui library.
#   SG_OLD_LIB_VERSION - Version of the old Sagui library.

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

#TODO: download latest Sagui from Github releases and delete SG_OLD_LIB_DIR

if (__SG_ABI_COMPLIANCE_CHECKER_INCLUDED)
    return()
endif ()
set(__SG_ABI_COMPLIANCE_CHECKER_INCLUDED ON)

option(SG_ABI_COMPLIANCE_CHECKER "Enable ABI compliance checker" OFF)

if (SG_ABI_COMPLIANCE_CHECKER)
    if (NOT SG_OLD_LIB_DIR)
        message(FATAL_ERROR "You must to specify SG_OLD_LIB_DIR for ABI compliance checking")
    endif ()
    if (NOT EXISTS ${SG_OLD_LIB_DIR})
        message(FATAL_ERROR "${SG_OLD_LIB_DIR} not found")
    endif ()
    if (NOT SG_OLD_LIB_VERSION)
        message(FATAL_ERROR "You must to specify SG_OLD_LIB_VERSION for ABI compliance checking")
    endif ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Og")
    find_program(_abi_comp_checker abi-compliance-checker)
    if (NOT _abi_comp_checker)
        message(FATAL_ERROR "abi-compliance-checker tool is required for ABI compliance checking")
    endif ()
    find_program(_abi_dumper abi-dumper)
    if (NOT _abi_dumper)
        message(FATAL_ERROR "abi-dumper tool is required for ABI compliance checking")
    endif ()
    file(TO_CMAKE_PATH "${SG_OLD_LIB_DIR}/build" _old_lib_build_dir)
    file(REMOVE_RECURSE "${_old_lib_build_dir}")
    file(MAKE_DIRECTORY "${_old_lib_build_dir}")
    add_custom_target(abi_compliance_checker
            COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS=-Og -DBUILD_SHARED_LIBS=ON -DSG_HTTPS_SUPPORT=ON ".."
            COMMAND ${CMAKE_COMMAND} --build "${_old_lib_build_dir}" --target ${PROJECT_NAME}
            WORKING_DIRECTORY ${_old_lib_build_dir}
            COMMENT "Compiling the old Sagui library for ABI compliance checking"
            DEPENDS ${PROJECT_NAME}
            VERBATIM)
    set(_old_lib_name "libsagui.so.${SG_OLD_LIB_VERSION}")
    set(_old_lib "${_old_lib_build_dir}/src/${_old_lib_name}")
    set(_new_lib_name libsagui.so.${VERSION})
    set(_new_lib "${CMAKE_BINARY_DIR}/src/${_new_lib_name}")
    file(REMOVE "${_old_lib}.dump")
    file(REMOVE "${_new_lib}.dump")
    add_custom_command(TARGET abi_compliance_checker POST_BUILD
            COMMAND ${_abi_dumper} "${_old_lib}" -o "${_old_lib}.dump" -lver "${SG_OLD_LIB_VERSION}"
            COMMAND ${_abi_dumper} "${_new_lib}" -o "${_new_lib}.dump" -lver "${VERSION}"
            COMMAND ${_abi_comp_checker} -l "${PROJECT_NAME}" -old "${_old_lib}.dump" -new "${_new_lib}.dump"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Checking ABI compliance"
            VERBATIM)
    unset(_abi_dumper)
    unset(_abi_comp_checker)
    unset(_old_lib_build_dir)
    unset(_old_lib_name)
    unset(_old_lib)
    unset(_new_lib_name)
    unset(_new_lib)
    unset(_old_lib)
endif ()