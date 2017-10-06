#!/bin/sh

## \file
#  Test cases with the shell.  Source this file to use the functions.
#
#  Sample usage:
#   \li call \ref tests_start
#   \li set the test name to \ref test_case and do some testing
#   \li store the result (interpretation as exit status) at \ref test_exit
#   \li call \ref evaluate_test
#   \li repeat for other test cases
#   \li call \ref tests_end

# Some terminal colors by name.
none="\033[0m"
blue="\033[0;34m"
green="\033[0;32m"
red="\033[0;31m"
brown="\033[0;33m"
cyan="\033[0;35m"

## Total number of run test cases.
tests_count=0
## Total number of failed tests.
tests_failed=0
## Name of the current test case
test_case=""
## Result (exit status) of the current test case.
test_exit=

## Print whether current \ref test_case was successful which is determined by
# \ref test_exit.  If the test was unsuccessful (`test_exit != 0`) print an
# optional message.
# If test_exit is not set (or null string) use $? for this.  The test_exit
# variable is unset after this call.
#
# \param $1 Optional error message.
evaluate_test ()
{
    test_exit=${test_exit:-$?}
    tests_count=`expr $tests_count + 1`

    echo -n "$test_case: " >&2
    if [ "$test_exit" -eq 0 ]; then
        echo "${green}passed$none." >&2
    else
        echo "${cyan}failed$none." >&2
        tests_failed=`expr $tests_failed + 1`
        if [ -n "$1" ]; then
            echo " >>> $brown$1$none" >&2
        fi
    fi
    test_exit=
}

## Call \ref evaluate_test with \a $1 and automatically set \ref test_case and
# \ref test_exit.
#
# \param $1 The function which represents a test case.  Should "return" 0 on
# success, i.e. the return state of the last command in the function is
# evaluated for \ref test_exit.
# \param $2 An optional name for \ref test_case.  Defaults to \a $1 if not set
# (or null string).
# \param $3 An optional error message that is passed on to \ref evaluate_test.
execute_test_case ()
{
    test_function=$1
    test_case=${2:-$test_function}
    "$test_function"
    evaluate_test "$3"
}

## Call before any test case.  Will set the initial values of the global
# variables.
tests_start ()
{
    tests_count=0
    tests_failed=0
    test_case=""
    test_exit=
    echo "${blue}------ RUNNING: `basename $0`$none" >&2
}

## Call after all test cases.  Exits with 1 if \ref tests_failed is not 0 or
# exits with 0 else.
tests_end ()
{
    echo "${blue}------ FINISHED: `basename $0`$none" >&2
    if [ "$tests_failed" -eq 0 ]; then
        echo "${green}------ ALL $tests_count tests passed.$none" >&2
        exit 0
    else
        echo "${cyan}------ $tests_failed of $tests_count" \
            "tests did NOT pass.$none" >&2
        exit 1
    fi
}

# Copyright 2017 A. Johannes RICHTER <albrechtjohannes.richter@gmail.com>

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

