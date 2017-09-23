configure_file (
    "config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    ESCAPE_QUOTES @ONLY)

add_executable (domaincloud
    "domaincloud.c" "extractwords.c"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h")
target_include_directories (domaincloud
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions (domaincloud
    PRIVATE "-DHAVE_CONFIG_H=1" "-D_GNU_SOURCE")
# Feature c_std_11 only later available
target_compile_features (domaincloud PRIVATE c_static_assert)

add_library (domaincloudlib STATIC "extractwords.c")
target_compile_definitions (domaincloudlib
    PRIVATE "-D_GNU_SOURCE")
target_compile_features (domaincloudlib PRIVATE c_static_assert)

install(
    TARGETS domaincloud
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

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

