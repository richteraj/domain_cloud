#!/bin/sh

# configure.sh -- CMake wrapper
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

force_build_delete=
if [ "$1" = "-f" ]; then
    force_build_delete=1
    shift
fi

echo "-- Detecting CMake"
cmake=`which cmake`
if [ -z "$cmake" ]; then
    echo "Error: 'cmake' is required for this project." >&2
    exit 1
fi

echo "-- Detecting CMake - done: $cmake"

echo "-- Creating build directory"
build_dir="build"
if [ -e "$build_dir" ]; then
    if [ -n $force_build_delete ]; then
        echo "-- Removing old build directory"
        rm -fr "$build_dir/"
    else
        echo "Error: build dir '$build_dir' already exists" >&2
        echo "  First delete the old directory or use the '-f' option." >&2
        exit 1
    fi
fi
mkdir "$build_dir"
echo "-- Creating build directory - done: $build_dir"

echo "-- Running CMake configuration: 'cmake .. $@'"
cd "$build_dir"
if ! cmake .. "$@"; then
    exit 1
fi
echo "-- Running CMake configuration - done"
echo
echo "To build the project run 'cmake --build $build_dir --target'."
