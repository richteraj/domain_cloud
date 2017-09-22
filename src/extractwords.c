/** \file
 * Parse words from an input stream. */

#include <errno.h>
#include <obstack.h>
#include <search.h>
#include <stdlib.h>

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

#include "extractwords.h"

/** Advance \a istr past the next <tt>*</tt><tt>/</tt> or to \a EOF.
 *
 *  \param istr The file handle to the input source.  Has to be opened
 *      for reading.
 */
static void
skip_block_comments (FILE *istr)
{
    int cur = getc (istr);
    int next;
    while ((next = getc (istr)) != EOF)
    {
        if (cur == '*' && next == '/')
            break;
        cur = next;
    }
}

/** Advance \a istr past the first not escaped delimiter char \a delim
 *  or to \a EOF.
 *
 *  \param delim The delimiter char.
 *  \param istr The file handle to the input source.  Has to be opened
 *      for reading.
 */
static void
skip_delimiter_escape_aware (char const *delims, FILE *istr)
{
    int cur;
    bool ignore_next = false;
    while ((cur = getc (istr)) != EOF)
    {
        if (ignore_next)
            ignore_next = false;
        else if (cur == '\\')
            ignore_next = true;
        else if (strchr (delims, cur) != NULL)
            break;
    }
}

/** Write a space to \a ostr and skip following white space. The first
 *  char that is not white space is pushed back to \a istr.
 *
 *  \param istr The file handle to the input source.  Has to be opened
 *      for reading.
 *  \param ostr The file handle where non-skipped text will be put.  Has
 *      to be opened for writing.
 *  \pre The last char read was a white space.
 *  \post The position of \a ostr is at the first char that is not white space
 *      or at \a EOF.
 */
static void
skip_white_space (FILE *istr, FILE *ostr)
{
    putc (' ', ostr);
    int cur;
    while ((cur = getc (istr)) != EOF)
    {
        if (!isspace (cur))
        {
            ungetc (cur, istr);
            break;
        }
    }
}

/** Skip block or line comments if the next read char is \c / or <tt>*</tt>,
 *  respectively.  Otherwise push back this char with \a ungetc and put \c /
 *  to \a ostr.
 *
 *  \param istr The file handle to the input source.  Has to be opened
 *      for reading.
 *  \param ostr The file handle where non-skipped text will be put.  Has
 *      to be opened for writing.
 *  \pre The last char read was a \c /.
 *  \post The position of \a ostr is
 *    \li after the first not escaped newline (line comment \c //),
 *    \li after the first <tt>*</tt><tt>/</tt> (block comment),
 *    \li at the position of the start of the function call (not a comment) or
 *    \li at \a EOF (comment "terminated" by \a EOF).
 */
static int
try_skip_comments (FILE *istr)
{
    int next = getc (istr);

    if (next == '/')
        skip_delimiter_escape_aware ("\n", istr);
    else if (next == '*')
        skip_block_comments (istr);
    else
    {
        ungetc (next, istr);
        return '/';
    }

    return EOF;
}

/** Copy content of \a istr to \a ostr while skipping comments,
 *  string literals and replacing successive white space by a single
 *  space.
 *
 *  \param istr The file handle to the input source.  Has to be opened
 *      for reading.
 *  \param ostr The file handle where non-skipped text will be put.  Has
 *      to be opened for writing.
 *      Will be flushed after processing.
 *  \returns \a errno if some I/O error occurred else 0.
 *  \post \c feof(istr) is true.
 */
int
remove_clutter (FILE *istr, FILE *ostr)
{
    int cur;

    while ((cur = getc (istr)) != EOF && !ferror (ostr))
    {
        if (cur == '/')
        {
            int push_back = try_skip_comments (istr);
            if (push_back != EOF)
                putc (push_back, ostr);
        }
        else if (cur == '"')
            skip_delimiter_escape_aware ("\"", istr);
        else if (cur == '\'')
            skip_delimiter_escape_aware ("'", istr);
        else if (isspace (cur))
            skip_white_space (istr, ostr);
        else
            putc (cur, ostr);
    }

    fflush (ostr);

    if (ferror (istr) || ferror (ostr))
        return errno;
    else
        return 0;
}

struct Word_frequency_s
{
    void *tree_root;
    struct obstack word_stack;
};

struct Word_s
{
    int count;
    char *name;
};

static int
word_compare (struct Word_s *w1, struct Word_s *w2)
{
    return strcmp (w1->name, w2->name);
}

static comparison_fn_t word_cmp_void = (comparison_fn_t) word_compare;

int
wfreq_init (struct Word_frequency_s **words)
{
    if (*words)
        return EEXIST;

    *words = calloc (1, sizeof (struct Word_frequency_s));
    if (!*words)
        return ENOMEM;
    else if (!obstack_init (&(*words)->word_stack))
        return ENOMEM;
    else
        return 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void _tree_free_fn_nop (void *node) {}
#pragma GCC diagnostic pop

void
wfreq_destroy (struct Word_frequency_s *words)
{
    if (!words)
        return;

    tdestroy (words->tree_root, _tree_free_fn_nop);
    obstack_free (&words->word_stack, NULL);
    free (words);
}

enum Word_position {Word_begin, Word_end, Not_word};

static void
add_word_to_tree (struct Word_frequency_s *result_words)
{
    obstack_1grow (&result_words->word_stack, '\0');
    char *name = obstack_finish (&result_words->word_stack);

    struct Word_s *new_word =
        obstack_alloc (&result_words->word_stack, sizeof (struct Word_s));
    new_word->name = name;
    new_word->count = 0;

    struct Word_s **word_in_tree =
        tsearch (new_word, &result_words->tree_root, word_cmp_void);
    if (new_word != *word_in_tree)
        obstack_free (&result_words->word_stack, new_word);

    ++(*word_in_tree)->count;
}

int
count_words (FILE *istr, struct Word_frequency_s *result_words)
{
    enum Word_position in_word = Not_word;

    while (!feof (istr) && !ferror (istr))
    {
        int cur = getc (istr);

        if (cur == '/')
            try_skip_comments (istr);
        else if (cur == '"')
            skip_delimiter_escape_aware ("\"", istr);
        else if (cur == '\'')
            skip_delimiter_escape_aware ("'", istr);
        else if (isdigit (cur))
        {
            if (in_word == Word_begin)
                obstack_1grow (&result_words->word_stack, cur);
        }
        else if (is_identifier (cur))
        {
            in_word = Word_begin;
            obstack_1grow (&result_words->word_stack, cur);
        }

        if (!isdigit (cur) && !is_identifier (cur))
            if (in_word == Word_begin)
                in_word = Word_end;

        if (in_word == Word_end)
        {
            add_word_to_tree (result_words);
            in_word = Not_word;
        }
    }

    if (ferror (istr))
        return errno;
    else
        return 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static FILE *_current_ostr = NULL;

static void
print_action_function_alpha_sorted (
    struct Word_s const **word, VISIT const which, int const depth)
{
    if (which == leaf || which == postorder)
        fprintf (_current_ostr, "%s\n", (*word)->name);
}

static void
print_action_function_with_freq (
    struct Word_s const **word, VISIT const which, int const depth)
{
    if (which == leaf || which == postorder)
        fprintf (_current_ostr, "%s [%d]\n", (*word)->name, (*word)->count);
}

static void
print_action_function_raw (
    struct Word_s const **word, VISIT const which, int const depth)
{
    if (which == leaf || which == postorder)
        for (int i = 0; i < (*word)->count; ++i)
            fprintf (_current_ostr, "%s\n", (*word)->name);
}

#pragma GCC diagnostic pop

void
print_words_alpha_sorted (FILE *ostr, struct Word_frequency_s *words)
{
    _current_ostr = ostr;
    twalk (
        words->tree_root, (__action_fn_t) print_action_function_alpha_sorted);
    _current_ostr = NULL;
}

void
print_words_with_freq (FILE *ostr, struct Word_frequency_s *words)
{
    _current_ostr = ostr;
    twalk (words->tree_root, (__action_fn_t) print_action_function_with_freq);
    _current_ostr = NULL;
}

void
print_words_raw (FILE *ostr, struct Word_frequency_s *words)
{
    _current_ostr = ostr;
    twalk (words->tree_root, (__action_fn_t) print_action_function_raw);
    _current_ostr = NULL;
}

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
