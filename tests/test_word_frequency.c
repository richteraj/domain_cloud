/** \file
 * Tests around \ref count_words.  */

#include "extractwords.h"
#include "cminitests.h"

/** Bundle the results of \ref count_words.  */
typedef struct Tree_output_res_s
{
    /** The generated output as a string.  Has to be freed.  */
    char *output;
    /** The return status of \e count_words.  */
    int count_words_res;
} Tree_output_res;

/** Create a stream from the \a input string, send it to \ref count_words,
 * capture the \ref Word_frequency_s result and convert it to a string (format
 * as with \ref print_words_alpha_sorted).
 *
 * \param input Input string.
 * \return Bundled result.  User has to do the cleanup.  */
Tree_output_res
create_and_output_words (char *input)
{
    FILE *is = fmemopen (input, strlen (input), "r");
    char *output = NULL;
    size_t output_len = 0;
    FILE *os = open_memstream (&output, &output_len);

    struct Word_frequency_s *words = NULL;
    wfreq_init (&words);

    Tree_output_res tree_out;
    tree_out.count_words_res = count_words (is, words);
    print_words_alpha_sorted (os, words);
    fclose (os);
    tree_out.output = output;

    wfreq_destroy (words);
    fclose (is);
    return tree_out;
}

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

void *
Symbols_except_dot_do_not_count_as_words ()
{
    char *input = "{]}/()/%&$#+// no words";
    char *expected_output = "";

    Tree_output_res res = create_and_output_words (input);

    require (!res.count_words_res,)
    require_streq (res.output, expected_output,)
    free (res.output);

    input = "a{]}/()/%&$#+// no words";
    expected_output = "a\n";
    res = create_and_output_words (input);

    require (!res.count_words_res,)
    require_streq (res.output, expected_output,)
    free (res.output);

    input = "a{]}/()/*b*/aa%&$#+// two words";
    expected_output = "a\naa\n";
    res = create_and_output_words (input);

    require (!res.count_words_res,)
    require_streq (res.output, expected_output,)
    free (res.output);

    return NULL;
}

void *
Count_increases_for_the_same_strings ()
{
    char *input = "a a b a.a a_a";
    char *expected_output = "a\na.a\na_a\nb\n";

    Tree_output_res res = create_and_output_words (input);

    require (!res.count_words_res,)
    require_streq (res.output, expected_output,)
    free (res.output);
    return NULL;
}

void
all_tests (void)
{
    CMT_TEST_CASE (A_newly_created_Word_frequency_s_is_safely_destroyed,)
    CMT_TEST_CASE (
        An_already_existing_Word_frequency_s_is_not_initialized_again,)
    CMT_TEST_CASE (Symbols_except_dot_do_not_count_as_words,)
    CMT_TEST_CASE (Count_increases_for_the_same_strings,)
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
