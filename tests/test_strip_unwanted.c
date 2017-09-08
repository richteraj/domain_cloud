/*
   test_strip_unwanted.c -- tests for stripping comments etc. from sources
   Copyright 2017 A. Johannes RICHTER <albrechtjohannes.richter@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>

#include "domaincloud.h"
#include "cminitests.h"

#undef cmt_set_up
void
cmt_set_up (void)
{
  /* TODO Set-up for every test case. */
}
#undef cmt_tear_down
void
cmt_tear_down (void)
{
  /* TODO Tear-down for every test case. */
}

typedef struct test_remove_clutter_res
{
  int res;
  char *output;
} rm_clutter_res;

rm_clutter_res
test_remove_clutter (char *input, size_t input_len)
{
  FILE *is = fmemopen (input, input_len, "r");
  char *output = NULL;
  size_t output_len = 0;
  FILE *os = open_memstream (&output, &output_len);

  int res = remove_clutter (is, os);

  fclose (os);
  fclose (is);

  return (rm_clutter_res) {res, output};
}

char *
line_comments_including_newline_are_stripped (void)
{
  char input[] = "line 1 //line 1.2\nline 2 //abc\n//3\nx / / y == z\n/";
  size_t input_len = sizeof (input) - 1;
  const char expected_output[] = "line 1 line 2 x / / y == z\n/";

  rm_clutter_res res = test_remove_clutter (input, input_len);

  require (res.res == 0, caller);
  require_streq (expected_output, res.output);

  free (res.output);

  return NULL;
}

char *
line_comments_terminated_by_eof_are_stripped (void)
{
  char input[] = "line 1 //line 1.2\nline 2 //abc\n//3\nx / / y == z\n//";
  size_t input_len = sizeof (input) - 1;
  const char expected_output[] = "line 1 line 2 x / / y == z\n";

  rm_clutter_res res = test_remove_clutter (input, input_len);

  require (res.res == 0, caller);
  require_streq (expected_output, res.output);

  free (res.output);

  return NULL;
}

void
all_tests (void)
{
  CMT_TEST_CASE (line_comments_including_newline_are_stripped)
  CMT_TEST_CASE (line_comments_terminated_by_eof_are_stripped)
}

CMT_RUN_TESTS (all_tests)
