#.rst:
# SgDoxygen
# ---------
#
# Doxygen variables for the Sagui library documentation generation.
#
# Adds the targets `make html` and `make pdf` to generate respectively the HTML and PDF API reference of
# the Sagui library.
#
# ::
#
#   SG_BUILD_HTML - Enable/disable the API reference generation as HTML.
#   SG_BUILD_PDF - Enable/disable the API reference generation as PDF. Note: it depends on SG_BUILD_HTML=ON.
#   SG_BUILD_MAN_PAGES - Enable/disable the API reference man pages generation. Note: it depends on SG_BUILD_HTML=ON.
#   SG_GENERATE_HTML - True when enabled the API reference generation as HTML.
#   SG_GENERATE_PDF - True when there is the possibility to generating the API reference as PDF.
#   SG_GENERATE_MAN_PAGES - True when there is the possibility to generating the API reference as man pages.
#
#   DOXYGEN_FOUND - True when Doxygen executable is found.
#   DOXYGEN_INPUT_FILE - Template file used to generate the DOXYGEN_OUTPUT_FILE.
#   DOXYGEN_OUTPUT_FILE - Production file to be used as Doxygen input file. It is generated from the
#                         template DOXYGEN_INPUT_FILE.
#   DOXYGEN_DOCS_DIR - Directory containing the documentation in HTML and PDF.
#   DOXYGEN_PDFLATEX_EXECUTABLE - The path of the `pdflatex` executable when it is found.
#   DOXYGEN_MAKEINDEX_EXECUTABLE - The path of the `makeindex` executable when it is found.
#   DOXYGEN_LATEX_DIR - Directory containing the generated PDF file.
#   DOXYGEN_MAN_DIR - Directory containing the generated man page files.

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

#TODO: Graphviz
#TODO: ePUB

if (__SG_DOXYGEN_INCLUDED OR (NOT CMAKE_BUILD_TYPE MATCHES "[Rr]elease|RELEASE"))
    return()
endif ()
set(__SG_DOXYGEN_INCLUDED ON)

option(SG_BUILD_HTML "Generate API reference [HTML]" OFF)
option(SG_BUILD_PDF "Generate API reference [PDF]" ${SG_BUILD_HTML})
#option(SG_BUILD_MAN_PAGES "Generate API reference [man pages]" OFF)

if (SG_BUILD_HTML)
    find_package(Doxygen QUIET)
    if (DOXYGEN_FOUND)
        set(SG_GENERATE_HTML ON)
        set(DOXYGEN_INPUT_FILE ${CMAKE_SOURCE_DIR}/doxygen/Doxyfile.in)
        set(DOXYGEN_OUTPUT_FILE ${CMAKE_BINARY_DIR}/Doxyfile)
        set(DOXYGEN_DOCS_DIR ${CMAKE_BINARY_DIR}/docs)
        set(SG_GENERATE_PDF NO)
        if (SG_BUILD_PDF)
            find_program(DOXYGEN_PDFLATEX_EXECUTABLE pdflatex)
            if (DOXYGEN_PDFLATEX_EXECUTABLE)
                find_program(DOXYGEN_MAKEINDEX_EXECUTABLE makeindex)
                if (DOXYGEN_MAKEINDEX_EXECUTABLE)
                    set(SG_GENERATE_PDF YES)
                else ()
                    message(WARNING "makeindex tool is required to generate the API reference PDF")
                endif ()
            else ()
                message(WARNING "pdflatex tool is required to generate the API reference PDF")
            endif ()
        endif ()
        #if (SG_BUILD_MAN_PAGES)
        #    set(SG_GENERATE_MAN_PAGES YES)
        #    set(DOXYGEN_MAN_DIR ${DOXYGEN_DOCS_DIR}/man)
        #endif ()
        if (SG_HTTPS_SUPPORT AND GNUTLS_FOUND)
            set(SG_HTTPS_SUPPORT_DOC "SG_HTTPS_SUPPORT")
        endif ()
        configure_file(${DOXYGEN_INPUT_FILE} ${DOXYGEN_OUTPUT_FILE} @ONLY)
        message(STATUS "Generating Doxygen file - done")
        add_custom_target(doc ALL
                COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_DOCS_DIR}
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUTPUT_FILE}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API reference with Doxygen [HTML]"
                VERBATIM)
        if (SG_GENERATE_PDF)
            set(DOXYGEN_LATEX_DIR ${DOXYGEN_DOCS_DIR}/latex)
            add_custom_target(pdf
                    COMMAND ${CMAKE_COMMAND} -E chdir ${DOXYGEN_LATEX_DIR} ${CMAKE_BUILD_TOOL}
                    COMMAND ${CMAKE_COMMAND} -E rename ${DOXYGEN_LATEX_DIR}/refman.pdf ${DOXYGEN_LATEX_DIR}/libsagui-v${VERSION}.pdf
                    WORKING_DIRECTORY ${DOXYGEN_LATEX_DIR}
                    COMMENT "Generating API reference with Doxygen [PDF]"
                    DEPENDS doc
                    VERBATIM)
        endif ()
        #if (SG_GENERATE_MAN_PAGES)
        #    find_program(_gzip gzip)
        #    if (_gzip)
        #        foreach (_src ${SG_C_SOURCE})
        #            get_filename_component(_filename ${_src} NAME_WE)
        #            set(_filename ${DOXYGEN_MAN_DIR}/man3/${_filename}.3)
        #            add_custom_command(TARGET doc POST_BUILD
        #                    COMMAND ${_gzip} -f ${_filename})
        #            install(FILES ${_filename}.gz
        #                    DESTINATION ${CMAKE_INSTALL_MANDIR}/man3)
        #            unset(_filename)
        #        endforeach ()
        #        unset(_gzip)
        #    endif ()
        #endif ()
    else ()
        message(WARNING "Doxygen tool is required to generate the API reference [HTML/PDF]")
    endif ()
endif ()