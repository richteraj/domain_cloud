# TODO Make relative `Project-*` etc. to use as a sub-project
include_directories (${CMAKE_BINARY_DIR}/src ${CMAKE_SOURCE_DIR}/src)
# TODO Keep if tests library necessary
# add_library (testutils SHARED testutils.c)

file (GLOB srcs test_*.c)
foreach (test_source IN LISTS srcs)
    get_filename_component (test_target ${test_source} NAME_WE)
    add_executable (${test_target} ${test_source})
    # TODO Add testutils lib if present
    target_link_libraries (${test_target} domaincloudlib)
    add_test (NAME ${test_target} COMMAND ${test_target})
endforeach (test_source)

set_property (TEST test_domaincloud PROPERTY WILL_FAIL 1)

# CMakeLists.txt - CMake tests files
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

