#.rst:
# SgDoxygen
# ---------
#
# Doxygen variables for the Sagui library documentation generation.
#
# Adds the targets `make html` to generate the library API reference in
# HTML format.
#
# ::
#
# SG_BUILD_HTML - Enable/disable the API reference generation.
# SG_GENERATE_HTML - True when enabled the API reference generation.
#
# DOXYGEN_FOUND - True when Doxygen executable is found.
# DOXYGEN_INPUT_FILE - Template file used to generate the DOXYGEN_OUTPUT_FILE.
# DOXYGEN_OUTPUT_FILE - Production file to be used as Doxygen input file. It is
# generated from the template DOXYGEN_INPUT_FILE.
# DOXYGEN_DOCS_DIR - Directory containing the documentation.

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

#TODO: Graphviz

if(__SG_DOXYGEN_INCLUDED OR (NOT CMAKE_BUILD_TYPE MATCHES "[Rr]elease|RELEASE"))
  return()
endif()
set(__SG_DOXYGEN_INCLUDED ON)

option(SG_BUILD_HTML "Generate API reference [HTML]" OFF)

if(SG_BUILD_HTML)
  find_package(Doxygen QUIET)
  if(DOXYGEN_FOUND)
    set(SG_GENERATE_HTML ON)
    set(SG_EXTRA_CSS_INPUT_FILE ${CMAKE_SOURCE_DIR}/doxygen/sagui.css)
    set(DOXYGEN_INPUT_FILE ${CMAKE_SOURCE_DIR}/doxygen/Doxyfile.in)
    set(DOXYGEN_OUTPUT_FILE ${CMAKE_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_DOCS_DIR ${CMAKE_BINARY_DIR}/docs)
    if(SG_HTTPS_SUPPORT AND GNUTLS_FOUND)
      set(SG_HTTPS_SUPPORT_DOC "SG_HTTPS_SUPPORT")
    endif()
    if(SG_HTTP_COMPRESSION)
      set(SG_HTTP_COMPRESSION_DOC "SG_HTTP_COMPRESSION")
    endif()
    if(SG_PATH_ROUTING)
      set(SG_PATH_ROUTING_DOC "SG_PATH_ROUTING")
    endif()
    if(SG_MATH_EXPR_EVAL)
      set(SG_MATH_EXPR_EVAL_DOC "SG_MATH_EXPR_EVAL")
    endif()
    configure_file(${DOXYGEN_INPUT_FILE} ${DOXYGEN_OUTPUT_FILE} @ONLY)
    message(STATUS "Generating Doxygen file - done")
    add_custom_target(
      doc ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_DOCS_DIR}
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUTPUT_FILE}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating API reference with Doxygen [HTML]"
      VERBATIM)
  else()
    message(
      WARNING "Doxygen tool is required to generate the library API reference")
  endif()
endif()
