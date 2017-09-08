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


#include <assert.h>

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

char *
line_comments_including_newline_are_stripped (void)
{
  require (0 > 1, "TODO Fill in the integration tests for domaincloud.");
  return NULL;
}

void
all_tests (void)
{
  CMT_TEST_CASE (line_comments_including_newline_are_stripped)
}

CMT_RUN_TESTS (all_tests)
