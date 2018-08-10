#.rst:
# SgTests
# -------
#
# Library testing.
#
# All library testings building.
#
# ::
#
#   SG_TESTS_DIR - Directory containing the library tests.
#   SG_ANDROID_TESTS_DEST_DIR - Directory to send the library tests (Android only).
#   SG_ANDROID_ADB_COMMAND - `adb` tool command  line (Android only).
#   SG_TESTS - All available tests.
#   SG_TESTS_SOURCE - All available testing source.
#   SG_BUILD_<TEST>_TESTING - Enable/disable a <TEST> listed by SG_TESTS, e.g: -DSG_BUILD_UTILS_TESTING=ON.

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

if (__SG_TESTS_INCLUDED)
    return()
endif ()
set(__SG_TESTS_INCLUDED ON)

if (BUILD_TESTING)
    if (WIN32)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    endif ()
    if (CURL_VERSION_STRING)
        string(COMPARE GREATER_EQUAL ${CURL_VERSION_STRING} "7.56.0" _curl_found)
    else ()
        unset(_curl_found)
    endif ()
    set(SG_TESTS_DIR ${CMAKE_SOURCE_DIR}/test)
    if (ANDROID)
        find_program(SG_ANDROID_ADB_COMMAND adb)
        if (NOT SG_ANDROID_ADB_COMMAND)
            message(FATAL_ERROR "adb not found")
        endif ()
        set(SG_ANDROID_TESTS_DEST_DIR "/data/local/tmp/sg_tests")
        add_definitions(-DSG_ANDROID_TESTS_DEST_DIR="${SG_ANDROID_TESTS_DEST_DIR}")
    endif ()
    list(APPEND SG_TESTS
            utils
            str
            strmap
            httputils
            httpauth
            httpuplds
            httpreq
            httpres
            httpsrv)
    if (_curl_found)
        list(APPEND SG_TESTS httpsrv_curl)
        if (SG_HTTPS_SUPPORT AND GNUTLS_FOUND)
            list(APPEND SG_TESTS httpsrv_tls_curl)
        endif ()
    endif ()
    set(SG_TESTS ${SG_TESTS} PARENT_SCOPE)
    list(APPEND _libs sagui)
    foreach (_test ${SG_TESTS})
        string(TOUPPER ${_test} _TEST)
        option(SG_BUILD_${_TEST}_TESTING "Build sg_${_test} testing" ON)
        if (SG_BUILD_${_TEST}_TESTING)
            list(APPEND SG_TESTS_SOURCE ${SG_TESTS_DIR}/test_${_test}.c)
            add_executable(test_${_test}
                    ${SG_TESTS_DIR}/sg_assert.h
                    ${SG_TESTS_DIR}/test_${_test}.c)
            target_link_libraries(test_${_test} ${_libs})
            if (_test MATCHES "curl")
                target_link_libraries(test_${_test} ${CURL_LIBRARIES} ${_libs})
            endif ()
            target_include_directories(test_${_test} PUBLIC ${SG_SOURCE_DIR})
            if (ANDROID)
                add_custom_command(TARGET test_${_test} POST_BUILD
                        COMMAND ${SG_ANDROID_ADB_COMMAND} shell mkdir -p ${SG_ANDROID_TESTS_DEST_DIR}
                        COMMAND ${SG_ANDROID_ADB_COMMAND} push test_${_test} ${SG_ANDROID_TESTS_DEST_DIR})
                add_test(NAME test_${_test}
                        COMMAND ${SG_ANDROID_ADB_COMMAND} shell ${SG_ANDROID_TESTS_DEST_DIR}/test_${_test})
            else ()
                add_test(NAME test_${_test}
                        WORKING_DIRECTORY ${SG_TESTS_DIR}
                        COMMAND test_${_test})
            endif ()
        endif ()
        unset(_TEST)
    endforeach ()
    unset(_curl_found)
    unset(_libs)
    if (ANDROID)
        add_custom_target(clean_pushed_tests
                COMMAND ${SG_ANDROID_ADB_COMMAND} shell rm -rf ${SG_ANDROID_TESTS_DEST_DIR}
                COMMENT "Cleaning all pushed tests"
                VERBATIM)
    endif ()
    set(SG_TESTS_SOURCE ${SG_TESTS_SOURCE} PARENT_SCOPE)
endif ()