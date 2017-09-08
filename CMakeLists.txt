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


# Project Properties
##
cmake_minimum_required (VERSION 3.0)

# Please do a complete rebuild to change the compiler, i.e. empty
# the build dir and call
#
#   cmake .. -DUSE_CLANG=1
#
# or use the `.configure` script:
#
#   ./configure.sh -f -DUSE_CLANG=1
#
if (USE_CLANG)
    message ("-- Searching for Clang")
    find_program (clang_PROGRAM NAMES clang clang-3.8 clang-3.9)
    if (${clang_PROGRAM} STREQUAL clang_PROGRAM-NOTFOUND)
        message (WARNING "Clang compiler not found")
    else ()
        set (CMAKE_C_COMPILER ${clang_PROGRAM})
    endif ()
endif ()

project (domain_cloud VERSION 0.1.0 LANGUAGES C)
set (PROJECT_AUTHORS "A. Johannes RICHTER")
set (PROJECT_COPY_YEARS "2017")

include (CTest)

set (src_DIR ${PROJECT_SOURCE_DIR}/src)
set (tests_DIR ${PROJECT_SOURCE_DIR}/tests)
set (bin_DIR ${PROJECT_SOURCE_DIR}/bin)
set (lib_DIR ${PROJECT_SOURCE_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bin_DIR})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${lib_DIR})


# Compiler options
##

set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Debug, Release, RelWithDebInfo, MinSizeRel
set (CMAKE_BUILD_TYPE Debug)
list (APPEND
    domain_cloud_C_FLAGS
    ${CMAKE_C_FLAGS}
    -std=gnu11 -Wall -O2)
list (APPEND
    domain_cloud_C_FLAGS_DEBUG
    ${CMAKE_C_FLAGS_DEBUG}
    -Wextra -Wformat=2 -g -Og)
list (APPEND
    domain_cloud_C_DEFINITIONS -D_GNU_SOURCE)

include (CheckCCompilerFlag)
message ("-- Setting compiler options (build type ${CMAKE_BUILD_TYPE})")
function (check_c_compiler_flag_list FLAG_LIST_VAR GOOD_FLAGS_RES_VAR)
    set (CMAKE_REQUIRED_QUIET 1)
    list (REMOVE_DUPLICATES ${FLAG_LIST_VAR})
    set (good_FLAGS_LIST ${${FLAG_LIST_VAR}})
    foreach (flag IN LISTS ${FLAG_LIST_VAR})
        string (REGEX REPLACE "[=,-]+" "_" flag_var "COMPLILER-FLAG-${flag}")
        check_c_compiler_flag ("${flag}" ${flag_var})
        if (NOT ${flag_var})
            list (REMOVE_ITEM good_FLAGS_LIST "${flag}")
            message ("  Flag ${flag} not supported by ${CMAKE_C_COMPILER}")
        endif ()
    endforeach (flag)
    set (${GOOD_FLAGS_RES_VAR} "${good_FLAGS_LIST}" PARENT_SCOPE)
endfunction (check_c_compiler_flag_list)

function (join_list LIST_VAR RES_VAR)
    set (_RES_VAR "")
    foreach (i IN LISTS ${LIST_VAR})
        set (_RES_VAR "${_RES_VAR} ${i}")
    endforeach (i)
    string (STRIP "${_RES_VAR}" _RES_VAR)
    set (${RES_VAR} "${_RES_VAR}" PARENT_SCOPE)
endfunction (join_list)

check_c_compiler_flag_list (domain_cloud_C_FLAGS res)
join_list (res domain_cloud_C_FLAGS_used)
set (CMAKE_C_FLAGS "${domain_cloud_C_FLAGS_used}")
check_c_compiler_flag_list (domain_cloud_C_FLAGS_DEBUG res)
join_list (res domain_cloud_C_FLAGS_DEBUG_used)
set (CMAKE_C_FLAGS_DEBUG "${domain_cloud_C_FLAGS_DEBUG_used}")

add_definitions (${domain_cloud_C_DEFINITIONS})
join_list (domain_cloud_C_DEFINITIONS domain_cloud_C_DEFINITIONS_used)

message ("-- Setting compiler options - done")

add_subdirectory (${src_DIR})
add_subdirectory (${tests_DIR})


# Summary
##
message ("\n-- Summary:")
message ("Project ${PROJECT_NAME} (${PROJECT_VERSION}):")
message ("  src:   ${PROJECT_SOURCE_DIR}")
message ("  build: ${PROJECT_BINARY_DIR}")
message ("  build type: ${CMAKE_BUILD_TYPE}")
message ("C compiler: ${CMAKE_C_COMPILER}")
message ("  Flags:         ${CMAKE_C_FLAGS}")
message ("  Flags (Debug): ${CMAKE_C_FLAGS_DEBUG}")
message ("  -D:            ${domain_cloud_C_DEFINITIONS_used}")

