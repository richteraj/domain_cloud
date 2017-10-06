/** \file
 * Minimal C testing "framework".  */

#ifndef CMINITESTS_H_
#define CMINITESTS_H_

#undef NDEBUG

/** \def CMT_COLOR_MODE
 * Enables terminal color escape sequences if evaluating to true.  Default is 1.
 */
#ifndef CMT_COLOR_MODE
#define CMT_COLOR_MODE 1
#endif /* not CMT_COLOR_MODE */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Setup function is called before \ref CMT_TEST_CASE calls the test function.
 * By default this does nothing.  To define a setup function redefine the
 * macro to call this function.  */
#define cmt_set_up()
/** Cleanup function is called after the by \ref CMT_TEST_CASE called test
 * function finishes.  By default this does nothing.  To define a tear-down
 * function redefine the macro to call this function.  */
#define cmt_tear_down()

/** Total number of tests run with \ref CMT_TEST_CASE.  */
extern int tests_count;
/** Total number of tests run with \ref CMT_TEST_CASE which failed.  */
extern int tests_failed;

/** Escape sequences for terminal coloring.
 * Print the color string `_colorful.`<em>color</em> to a terminal output to
 * switch the text color and print the `_colorful.none` value to reset the
 * default color.  */
struct _colorful_s
{
    /** Color reset.  */
    const char *none;
    /** Text after this is blue.  */
    const char *blue;
    /** Text after this is green.  */
    const char *green;
    /** Text after this is red.  */
    const char *red;
    /** Text after this is brown/orange.  */
    const char *brown;
    /** Text after this is cyan.  */
    const char *cyan;
};

/** Global object with the escape sequences for terminal coloring.  If \ref
 * CMT_COLOR_MODE is not true all colors will be an empty string.  */
static const struct _colorful_s _colorful = {
#if CMT_COLOR_MODE
    .none = "\033[0m",
    .blue = "\033[0;34m",
    .green = "\033[0;32m",
    .red = "\033[0;31m",
    .brown = "\033[0;33m",
    .cyan = "\033[0;35m"
#else
    .none = "",
    .blue = "",
    .green = "",
    .red = "",
    .brown = "",
    .cyan = ""
#endif
};

#ifdef _GNU_SOURCE
#define set_short_name(long_name) /* NULL */
#else /* not _GNU_SOURCE */

/** A string of a short name with which the program was called.  */
static char const *program_invocation_short_name = NULL;

/** Let \ref program_invocation_short_name point to the position after the last
 * '/' of \a long_name or to \a long_name directly if there are no slashes.
 *
 * \param long_name The file name from which to get the base name.  May not
 * change as long as \ref program_invocation_short_name is in use.  */
void
set_short_name (char const *long_name)
{
    char const *last_slash = strrchr (long_name, '/');
    program_invocation_short_name = last_slash ? last_slash + 1 : long_name;
}

#endif /* not _GNU_SOURCE */

/** Print a colored (depending on \ref _colorful_s) error message to `stderr`.
 * The output is of the form:
 * \code
 *   file:line-number: function: failed:
 *    >>>   formated-msg
 * \endcode
 *
 * \param msg A `printf` format string.
 * \param ... Arguments to the format string.  */
#define cmt_error(msg, ...)                                             \
    {                                                                   \
        fprintf (                                                       \
            stderr, "%s:%d: %s: %sfailed%s: \n >>>\t%s" #msg, __FILE__, \
            __LINE__, __func__, _colorful.cyan, _colorful.none,         \
            _colorful.brown, __VA_ARGS__);                              \
        fprintf (stderr, "%s\n", _colorful.none);                       \
    }

/** Register a test case function.  Call \ref cmt_set_up and \ref cmt_tear_down
 * before the \a test function and after the function finished, respectively.
 * Update \ref tests_count and \ref tests_failed accordingly.  Print to `stderr`
 * whether the test failed or succedded and if it failed print a message which
 * contains the returned string.
 *
 * \param test A test function which returns `NULL` on success or an error
 * string if failed.  Must have the parameter types that fit the `__VA_ARGS__`
 * argument (or none).
 * \param ... The parameters to pass on to \a test.  May be none.  */
#define CMT_TEST_CASE(test, ...)                                  \
    {                                                             \
        ++tests_count;                                            \
        cmt_set_up ();                                            \
        char *test##_result = test (__VA_ARGS__);                 \
        cmt_tear_down ();                                         \
        if (test##_result)                                        \
        {                                                         \
            cmt_error ("%s", test##_result);                      \
            ++tests_failed;                                       \
        }                                                         \
        else                                                      \
        {                                                         \
            fprintf (                                             \
                stderr, #test ": %spassed%s.\n", _colorful.green, \
                _colorful.none);                                  \
        }                                                         \
    }

/** Execute the \a tests_wrapper function which should contain all the tests
 * registered with \ref CMT_TEST_CASE.  Evaluate the resulting \ref tests_count
 * and \ref tests_failed values and print the status.
 * This macro generates `main`.
 *
 * \param tests_wrapper A function with no parameters and no return value (i.e.
 * it is ignored) which contains the test cases.
 * \return 0 if \ref tests_failed was 0 after calling \a tests_wrapper.
 * Otherwise non-zero.  */
#define CMT_RUN_TESTS(tests_wrapper)                                          \
    int main (int argc, char **argv)                                          \
    {                                                                         \
        /* Suppress compiler warning: unused arguments */                     \
        assert (argc > 0 && argv);                                            \
        set_short_name (argv[0]);                                             \
        tests_failed = 0;                                                     \
        tests_count = 0;                                                      \
        fprintf (                                                             \
            stderr, "%s------ RUNNING: %s%s\n", _colorful.blue,               \
            program_invocation_short_name, _colorful.none);                   \
        tests_wrapper ();                                                     \
        fprintf (                                                             \
            stderr, "%s------ FINISHED: %s%s\n", _colorful.blue,              \
            program_invocation_short_name, _colorful.none);                   \
        if (tests_count != 0 && tests_failed != 0)                            \
        {                                                                     \
            fprintf (                                                         \
                stderr, "%s------ %d of %d tests did NOT pass.%s\n",          \
                _colorful.cyan, tests_failed, tests_count, _colorful.none);   \
            exit (EXIT_FAILURE);                                              \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            fprintf (                                                         \
                stderr, "%s------ All %d tests passed.%s\n", _colorful.green, \
                tests_count, _colorful.none);                                 \
        }                                                                     \
    }

/** Assert that \a cond is true.  Otherwise return from the enclosing function
 * with a message that contains `__VA_ARGS__`.
 *
 * \param cond The condition expression to evaluate.
 * \param ... Strings which describe the failed condition.  They will be
 * directly concatenated.  */
#define require(cond, ...)                                  \
    if (!(cond))                                            \
    {                                                       \
        cmt_error ("Assertion require '%s' failed", #cond); \
        return #cond " NOT true: " #__VA_ARGS__;            \
    }

/** Assert that \a cond is false.
 * \copydetails require  */
#define require_not(cond, ...)                                  \
    if ((cond))                                                 \
    {                                                           \
        cmt_error ("Assertion require_not '%s' failed", #cond); \
        return #cond " NOT false: " #__VA_ARGS__;               \
    }

/** Assert that strings \a s1 and \a s2 are equal according to `strcmp`.
 * Otherwise return from the enclosing function with a message that contains
 * `__VA_ARGS__`.
 *
 * \param s1 The first string.
 * \param s2 The second string.
 * \param ... Strings which describe the failed condition.  They will be
 * directly concatenated.  */
#define require_streq(s1, s2, ...)                                           \
    {                                                                        \
        size_t s1_len = strlen (s1);                                         \
        size_t s2_len = strlen (s2);                                         \
        if (s1_len != s2_len)                                                \
        {                                                                    \
            cmt_error (                                                      \
                "Length '%s' != '%s' (%zd != %zd)", s1, s2, s1_len, s2_len); \
            return #s1 " != " #s2 ": " #__VA_ARGS__;                         \
        }                                                                    \
        else if (strcmp (s1, s2))                                            \
        {                                                                    \
            cmt_error ("'%s' != '%s'", s1, s2);                              \
            return #s1 " != " #s2 ": " #__VA_ARGS__;                         \
        }                                                                    \
    }

/** Assert that strings \a s1 and \a s2 are not equal according to `strcmp`.
 * \copydetails require_streq  */
#define require_strneq(s1, s2, ...)                  \
    {                                                \
        if (!strcmp (s1, s2))                        \
        {                                            \
            cmt_error ("'%s' == '%s'", s1, s2);      \
            return #s1 " == " #s2 ": " #__VA_ARGS__; \
        }                                            \
    }

/** Helper function for \ref require_streq_array and \ref require_strneq_array.
 * Does the actual comparison.
 *
 * \param sa1 The first string vector.
 * \param sa2 The second string vector.
 * \param cmp_equal If true compare for equality.  If false compare for
 * inequality.
 * \return
 *   \li \a cmp_equal is true: true if \a sa1 and \a sa2 are equal else false.
 *   \li \a cmp_equal is false: true if \a sa1 and \a sa2 are not equal else
 *   false.  */
bool
streqneq_array (char **sa1, char **sa2, bool cmp_equal)
{
    char **sa1_pos = sa1;
    char **sa2_pos = sa2;
    int pos = 0;
    while (*sa1_pos && *sa2_pos)
    {
        if (strcmp (*sa1_pos, *sa2_pos) != 0)
        {
            if (cmp_equal)
                cmt_error (
                    "Array mismatch at position %d: '%s' != '%s'",
                    pos, *sa1_pos, *sa2_pos);
            return !cmp_equal;
        }
        ++sa1_pos;
        ++sa2_pos;
        ++pos;
    }

    if (*sa1_pos != *sa2_pos)
    {
        if (cmp_equal)
            cmt_error ("Array sizes don't match (stopped at position %d)", pos);
        return !cmp_equal;
    }
    else
        return cmp_equal;
}

/** Assert that the string vectors \a as1 and \a sa2 are equal.  Otherwise
 * return from the enclosing function with a message that contains
 * `__VA_ARGS__`.
 *
 * A string vector has the same form as `argv` of `main`, i.e. it is an array of
 * `char *` with a terminating `NULL` entry.
 *
 * \param sa1 The first `NULL`-terminated `char **` string vector.
 * \param sa2 The second string vector.
 * \param ... Strings which describe the failed condition.  They will be
 * directly concatenated.  */
#define require_streq_array(sa1, sa2, ...)                            \
    {                                                                 \
        if (!streqneq_array ((sa1), (sa2), true))                     \
        {                                                             \
            cmt_error ("Arrays %s and %s are not equal", #sa1, #sa2); \
            return "Arrays mismatch: " #__VA_ARGS__;                  \
        }                                                             \
    }

/** Assert that the string vectors \a as1 and \a sa2 are not equal.
 * \copydetails require_streq_array  */
#define require_strneq_array(sa1, sa2, ...)                       \
    {                                                             \
        if (!streqneq_array ((sa1), (sa2), false))                \
        {                                                         \
            cmt_error ("Arrays %s and %s are equal", #sa1, #sa2); \
            return "Arrays match: " #__VA_ARGS__;                 \
        }                                                         \
    }

int tests_count;
int tests_failed;

#endif /* not CMINITESTS_H_ */

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
