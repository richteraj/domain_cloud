#!/bin/sh

prog_dir="${1?"Have to supply directory where program is located"}"
prog="$prog_dir/domaincloud"

. "`dirname \"$0\"`/shminitests.sh"

tests_start

test_case="Program with no arguments has nonzero exit"
! "$prog" >/dev/null 2>&1
test_exit=$?
evaluate_test

test_case="Program with no arguments prints an error message"
"$prog" 2>&1 | grep -q  "No input files!"
test_exit=$?
evaluate_test 'grep -q  "No input files!"'

input_file="`mktemp`"
output_file="`mktemp`"

test_case="Program doesn't fails if there are no words remaining"
echo "// Only comments...\n\"and strings...\"" >"$input_file"
"$prog" -S "$input_file" >/dev/null 2>&1
test_exit=$?
evaluate_test

test_case="Program skips non-readable input files"
echo "some words" >"$input_file"
bad_input_file="Not a file 1"
"$prog" -S "$bad_input_file" "$input_file" 2>&1 | \
    grep -q "Can't open '$bad_input_file'!: No such file or directory"
test_exit=$?
evaluate_test

test_case="Program skips non-readable input files and doesn't fail"
echo "some words" >"$input_file"
bad_input_file="Not a file 1"
"$prog" -S "$bad_input_file" "$input_file" >/dev/null 2>&1
test_exit=$?
evaluate_test

test_case="Program fails if it can't write to output file"
! "$prog" -S -o "/no_root_access" "$input_file" 2>/dev/null
test_exit=$?
evaluate_test

test_case="Program recognizes '-' as standard in/output"
echo "/* skip */some_words" | "$prog" -S -o - - | grep -q "some_words"
test_exit=$?
evaluate_test

test_case="Program writes output to given file"
echo "some_words" >"$input_file"
: > "$output_file"
"$prog" -S -o "$output_file" "$input_file"
res=$?
cat "$output_file" | grep -q "some_words"
test_exit=`expr $res + $?`
evaluate_test

# TODO Create a mock for word_cloud_cli.py: Tests without -S options require
# this program which also needs a lot of time

rm -f "$input_file" "$output_file"

tests_end
