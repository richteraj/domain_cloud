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
test_exit=0

## Print whether current \ref test_case was successful which is determined by
#  \ref test_exit.  If the test was unsuccessful (<tt>test_exit != 0</tt>)
#  print an optional message.
#
#  \param $1 Optional error message.
evaluate_test ()
{
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
}

## Call before any test case.
tests_start ()
{
    echo "$blue------ RUNNING: `basename $0`$none" >&2
}

## Call after all test cases.  Exits with 1 if \ref tests_failed is not 0 or
#  exits with 0 else.
tests_end ()
{
    echo "$blue------ FINISHED: `basename $0`$none" >&2
    if [ "$tests_failed" -eq 0 ]; then
        echo "${green}------ ALL $tests_count tests passed.$none" >&2
        exit 0
    else
        echo "${cyan}------ $tests_failed of $tests_count" \
            "tests did NOT pass.$none" >&2
        exit 1
    fi
}
