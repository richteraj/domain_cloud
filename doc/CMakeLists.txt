set (doxygen_common_file ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-common)
set (doxygen_dev_file ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-dev)

configure_file (
    ${doxygen_common_file}.in ${doxygen_common_file}
    ESCAPE_QUOTES @ONLY)
configure_file (
    ${doxygen_dev_file}.in ${doxygen_dev_file}
    ESCAPE_QUOTES @ONLY)

if ("${CMAKE_BUILD_TYPE}" STREQUAL Debug
    OR "${CMAKE_BUILD_TYPE}" STREQUAL RelWithDebInfo)
    set (doxyfile "${doxygen_dev_file}")
else ()
    set (doxyfile "${doxygen_common_file}")
endif ()

find_program (doxygen_BIN doxygen)
if (NOT ${doxygen_BIN} STREQUAL doxygen_BIN-NOTFOUND)

    add_custom_target (doc
        COMMENT "Generating documentation with Doxygen"
        DEPENDS ${doc_DIR}/doxygen)

    file(GLOB_RECURSE documented_files
        "${src_DIR}/[^.]*" "${tests_DIR}/[^.]*"
        "${PROJECT_SOURCE_DIR}/README.md")

    add_custom_command (OUTPUT ${doc_DIR}/doxygen
        COMMAND ${doxygen_BIN} ${doxyfile}
        COMMENT "${doxygen_BIN} ${doxyfile}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        DEPENDS ${doxygen_common_file}.in ${doxygen_dev_file}.in
                ${documented_files})
else ()
    message (WARNING
        "Couldn't find Doxygen executable. "
        "No documentation will be generated")
endif ()

# CMakeLists.txt - CMake source files
# Copyright 2017 A. Johannes RICHTER <albrechtjohannes.richter@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

