#undef NDEBUG

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif /* not _GNU_SOURCE */

#ifndef CMINITESTS_H_
#define CMINITESTS_H_

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <string.h>

#define cmt_set_up()
#define cmt_tear_down()

/** Total number of tests run with \ref CMT_TEST_CASE. */
extern int tests_count;
/** Total number of tests run with \ref CMT_TEST_CASE which failed. */
extern int tests_failed;

#define cmt_error(msg, ...) \
    fprintf (stderr, \
        "%s:%d: %s failed: \n >>>\t" #msg "\n", \
        __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define cmt_require_status(msg, ...) \
    fprintf (stderr, \
        " >>>\t" #msg "\n", ##__VA_ARGS__)

#define CMT_TEST_CASE(test, args...) { \
    ++tests_count; \
    cmt_set_up (); \
    char *test##_result = test (args); \
    cmt_tear_down (); \
    if (test##_result) { \
        cmt_error ("%s", test##_result); \
        ++tests_failed; } \
    else { \
        printf (#test " passed.\n"); } \
}

#define CMT_RUN_TESTS(tests_wrapper) \
    int main (int argc, char *argv[]) { \
        /* Suppress parameter unused warning */ \
        assert (argc > 0); \
        tests_failed = 0; \
        tests_count = 0; \
        printf ("------ RUNNING: %s\n", argv[0]); \
        tests_wrapper (); \
        if (tests_count != 0 && tests_failed != 0) { \
            error (EXIT_FAILURE, 0, \
                "%d of %d tests did NOT pass.\n", \
                tests_failed, tests_count); } \
        else { \
            printf ("------ All %d tests passed.\n", tests_count); \
            return EXIT_SUCCESS; } \
    }

#define require(cond, message...) \
    if (!(cond)) { \
        cmt_error ("Assertion require '%s' failed", #cond); \
        return #cond " NOT true: " #message; }

#define require_not(cond, message...) \
    if ((cond)) { \
        cmt_error ("Assertion require_not '%s' failed", #cond); \
        return #cond " NOT false: " #message; }

#define require_streq(s1, s2, message...) { \
    size_t s1_len = strlen (s1); \
    size_t s2_len = strlen (s2); \
    if (s1_len != s2_len) { \
        cmt_error ("Length '%s' != '%s' (%zd != %zd)", s1, s2, s1_len, s2_len); \
        return #s1 " != " #s2 ": " #message; } \
    else if (strcmp (s1, s2)) { \
        cmt_error ("'%s' != '%s'", s1, s2); \
        return #s1 " != " #s2 ": " #message; } \
}

#define require_strneq(s1, s2, message...) { \
    if (!strcmp (s1, s2)) { \
        cmt_error ("'%s' == '%s'", s1, s2); \
        return #s1 " == " #s2 ": " #message; } \
}

bool
streqneq_array (char **sa1, char **sa2, bool cmp_equal)
{
    char **sa1_pos = sa1;
    char **sa2_pos = sa2;
    int pos = 0;
    while (*sa1_pos && *sa2_pos)
    {
        if (strcmp (*sa1_pos, *sa2_pos))
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

#define require_streq_array(sa1, sa2, message...) { \
    if (!streqneq_array ((sa1), (sa2), true)) { \
        cmt_error ("Arrays %s and %s are not equal", #sa1, #sa2); \
        return "Arrays mismatch: " #message; } \
}


#define require_strneq_array(sa1, sa2, message...) { \
    if (!streqneq_array ((sa1), (sa2), false)) { \
        cmt_error ("Arrays %s and %s are equal", #sa1, #sa2); \
        return "Arrays match: " #message; } \
}

int tests_count;
int tests_failed;

#endif /* not CMINITESTS_H_ */

/* cminitests.h -- Minimal C testing "framework"
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

