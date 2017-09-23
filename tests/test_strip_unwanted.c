/** \file
 * Tests for stripping comments from sources etc.  */

#include "extractwords.h"
#include "cminitests.h"

/** Bundle the result of \ref remove_clutter.  */
typedef struct
{
    /** The return status of \e remove_clutter.  */
    int res;
    /** The output that \e remove_clutter generated as a string.  Has to be
     * freed.  */
    char *output;
} Rm_clutter_res;

/** Create a stream from the \a input string, send it to \ref remove_clutter,
 * capture the result as a string.
 *
 * \param input Input string.
 * \param input_len Length of \a input.
 * \return Bundled result.  User has to do the cleanup.  */
Rm_clutter_res
test_remove_clutter (char *input, size_t input_len)
{
    FILE *is = fmemopen (input, input_len, "r");
    char *output = NULL;
    size_t output_len = 0;
    FILE *os = open_memstream (&output, &output_len);

    int res = remove_clutter (is, os);

    fclose (os);
    fclose (is);

    return (Rm_clutter_res) {res, output};
}

char *
Line_comments_including_newline_are_stripped (void)
{
    char input[] = "line 1 //line 1.2\nline 2 //abc\n//3\nx / / y == z\n/";
    size_t input_len = sizeof (input) - 1;
    const char expected_output[] = "line 1 line 2 x / / y == z /";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Line_comments_terminated_by_eof_are_stripped (void)
{
    char input[] = "line 1 //line 1.2\nline 2 //abc\n//3\nx / / y == z\n//";
    size_t input_len = sizeof (input) - 1;
    const char expected_output[] = "line 1 line 2 x / / y == z ";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Line_comments_continue_with_excaped_newline (void)
{
    char input[] = "line 1 //comm 1.1\\\ncomm 1.2\nline 2";
    size_t input_len = sizeof (input) - 1;
    const char expected_output[] = "line 1 line 2";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Block_comments_terminated_by_eof_are_stripped (void)
{
    char input[] = "line 1 /*block-comment... line 1.2\nl 2 //\n* / * /\n//";
    size_t input_len = sizeof (input) - 1;
    const char expected_output[] = "line 1 ";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Block_comments_are_not_nested (void)
{
    char input[] = "line 1 /*bc1 /*bc2 */ bc3 */";
    size_t input_len = sizeof (input) - 1;
    const char expected_output[] = "line 1  bc3 */";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Inline_block_comments_are_stripped (void)
{
    char input[] = "line 1 /*block-comment... line 1.2\nl *//";
    size_t input_len = sizeof (input) - 1;
    const char expected_output[] = "line 1 /";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Line_comments_supersede_block_comments (void)
{
    char input[] = "line 1 // comm0 /*bc1 \nbc3 */";
    size_t input_len = sizeof (input) - 1;
    const char expected_output[] = "line 1 bc3 */";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Quoted_strings_are_removed (void)
{
    char input[] = "char *res = \"ABC\"; \'EFG\';";
    size_t input_len = sizeof (input) - 1;
    const char *expected_output = "char *res = ; ;";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Quoted_strings_with_escaped_quotes_are_removed (void)
{
    char input[] = "char *res = \"AB\\\"C\"; \'AB\\\'C\';";
    size_t input_len = sizeof (input) - 1;
    const char *expected_output = "char *res = ; ;";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Quoted_strings_are_not_terminated_by_newline (void)
{
    char input[] = "char *res = \"AB\\\"C\nD\" - 'AB\\\'C\nD';";
    size_t input_len = sizeof (input) - 1;
    const char *expected_output = "char *res =  - ;";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Quoted_strings_are_terminated_by_eof (void)
{
    char input[] = "char *res = \"AB\\\"C\n ...";
    size_t input_len = sizeof (input) - 1;
    const char *expected_output = "char *res = ";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    char input2[] = "char *res = \'AB\\\'C\n ...";
    input_len = sizeof (input2) - 1;
    expected_output = "char *res = ";

    res = test_remove_clutter (input2, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
Comments_are_ignored_inside_quoted_strings (void)
{
    char input[] = "char *res = \"AB //...\"; \'CD //...\'";
    size_t input_len = sizeof (input) - 1;
    const char *expected_output = "char *res = ; ";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    char input2[] = "char *res = \"AB /*...\"; \'CD //...\'";
    input_len = sizeof (input2) - 1;
    expected_output = "char *res = ; ";

    res = test_remove_clutter (input2, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

char *
An_even_number_of_preceding_escapes_does_not_escape_the_delimiter (void)
{
    char input[] =
        "even: '2:'\"\\\\\"text1\"text2\"\n"
        "odd:  '3:'\'\\\\\\\'text3\'text4\'\n\'\n"
        "even: '4:'//\\\\\\\\\nline 2\n"
        "odd:  '5:'//\\\\\\\\\\\nline 3\nline 4";
    size_t input_len = sizeof (input) - 1;
    const char *expected_output = "even: text1 odd: text4 even: line 2 odd: line 4";

    Rm_clutter_res res = test_remove_clutter (input, input_len);

    require (res.res == 0, caller,)
    require_streq (expected_output, res.output,)

    free (res.output);

    return NULL;
}

void
all_tests (void)
{
    CMT_TEST_CASE (Line_comments_including_newline_are_stripped,)
    CMT_TEST_CASE (Line_comments_terminated_by_eof_are_stripped,)
    CMT_TEST_CASE (Line_comments_continue_with_excaped_newline,)
    CMT_TEST_CASE (Inline_block_comments_are_stripped,)
    CMT_TEST_CASE (Block_comments_terminated_by_eof_are_stripped,)
    CMT_TEST_CASE (Block_comments_are_not_nested,)
    CMT_TEST_CASE (Line_comments_supersede_block_comments,)

    CMT_TEST_CASE (Quoted_strings_are_removed,)
    CMT_TEST_CASE (Quoted_strings_with_escaped_quotes_are_removed,)
    CMT_TEST_CASE (Quoted_strings_are_not_terminated_by_newline,)
    CMT_TEST_CASE (Quoted_strings_are_terminated_by_eof,)

    CMT_TEST_CASE (Comments_are_ignored_inside_quoted_strings,)
    CMT_TEST_CASE (An_even_number_of_preceding_escapes_does_not_escape_the_delimiter,)
}

CMT_RUN_TESTS (all_tests)

/* Copyright 2017 A. Johannes RICHTER <albrechtjohannes.richter@gmail.com>

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
