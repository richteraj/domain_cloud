#include <assert.h>

#include "extractwords.h"
#include "cminitests.h"

char *
A_newly_created_Word_frequency_s_is_safely_destroyed (void)
{
    struct Word_frequency_s *words = NULL;
    require (!wfreq_init (&words),)
    wfreq_destroy (words);
    return NULL;
}

char *
An_already_existing_Word_frequency_s_is_not_initialized_again (void)
{
    struct Word_frequency_s *words = NULL;
    require (!wfreq_init (&words),)
    require (wfreq_init (&words) == EEXIST,)
    wfreq_destroy (words);
    return NULL;
}

void
all_tests (void)
{
    CMT_TEST_CASE (A_newly_created_Word_frequency_s_is_safely_destroyed,)
    CMT_TEST_CASE (
        An_already_existing_Word_frequency_s_is_not_initialized_again,)
}

CMT_RUN_TESTS (all_tests)

/*
   test_domaincloud.c -- tests for domaincloud.c
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
