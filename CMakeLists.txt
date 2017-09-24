cmake_minimum_required (VERSION 3.0)

set (CMAKE_BUILD_TYPE Debug CACHE STRING
    "Debug, Release, RelWithDebInfo or MinSizeRel")

project (domain_cloud VERSION 0.3.0 LANGUAGES C)
set (PROJECT_AUTHORS "A. Johannes RICHTER")
set (PROJECT_COPY_YEARS "2017")

set (src_DIR ${PROJECT_SOURCE_DIR}/src)
set (tests_DIR ${PROJECT_SOURCE_DIR}/tests)
set (doc_DIR ${PROJECT_SOURCE_DIR}/doc)
set (bin_DIR ${PROJECT_SOURCE_DIR}/bin)
set (lib_DIR ${PROJECT_SOURCE_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bin_DIR})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${lib_DIR})

include (CTest)

set (CMAKE_C_STANDARD 99)
set (CMAKE_C_STANDARD_REQUIRED ON)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -O3")
if (CMAKE_C_COMPILER_ID STREQUAL Clang)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
        -Wno-gnu-zero-variadic-macro-arguments -Wno-missing-prototypes \
        -Wno-variadic-macros")
    set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O2 -Weverything -Wno-padded")
elseif (CMAKE_C_COMPILER_ID STREQUAL GNU)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-variadic-macros")
    set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wformat=2 -Og")
endif ()
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)


add_subdirectory (${src_DIR})
add_subdirectory (${tests_DIR})
add_subdirectory (${doc_DIR})


# Summary
message ("\n-- Summary:")
message ("Project ${PROJECT_NAME} (${PROJECT_VERSION}):")
message ("  src:   ${PROJECT_SOURCE_DIR}")
message ("  build: ${PROJECT_BINARY_DIR}")
message ("  build type: ${CMAKE_BUILD_TYPE}")
message ("C compiler: ${CMAKE_C_COMPILER}")
message ("  Flags:         ${CMAKE_C_FLAGS}")
message ("  Flags (Debug): ${CMAKE_C_FLAGS_DEBUG}")
message ("  -D:            ${COMPILE_DEFINITIONS}")

# CMakeLists.txt -- global project configuration
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

