#!/bin/sh

none="\033[0m"
blue="\033[0;34m"
green="\033[0;32m"
red="\033[0;31m"
brown="\033[0;33m"
cyan="\033[0;35m"

prog="${1?"Have to supply directory where program is located"}/domaincloud"
tests_failed=0
tests_count=0

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

tests_start ()
{
    echo "$blue------ RUNNING: `basename $0`$none" >&2
}

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

tests_start

test_case="Program with no arguments has nonzero exit"
! "$prog" >/dev/null 2>&1
test_exit=$?
evaluate_test

test_case="Program with no arguments prints an error message"
"$prog" 2>&1 | grep -q  "No input files!"
test_exit=$?
evaluate_test 'grep -q  "No input files!"'

tests_end
