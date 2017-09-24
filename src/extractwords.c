/** \file
 * Parse words from an input stream.  */

#include <errno.h>
#include <obstack.h>
#include <search.h>
#include <stdlib.h>

/** Use `malloc` for obstack allocation.  */
#define obstack_chunk_alloc malloc
/** Use `free` for obstack de-allocation.  */
#define obstack_chunk_free free

#include "extractwords.h"

/** Advance \a istr past the next `*``/` or to `EOF`.
 * \param istr The file handle to the input source.  Has to be opened for
 * reading.  */
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

/** Advance \a istr past the first not escaped delimiter char matching one in \a
 * delims or to `EOF`.
 *
 * \param delims The delimiter chars.
 * \param istr The file handle to the input source.  Has to be opened for
 * reading.  */
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

/** Write a space to \a ostr and skip following white space. The first char that
 * is not white space is pushed back to \a istr.
 *
 * \param istr The file handle to the input source.  Has to be opened for
 * reading.
 * \param ostr The file handle where non-skipped text will be put.  Has to be
 * opened for writing.
 *
 * \pre The last char read was a white space.
 * \post The position of \a ostr is at the first char that is not white space or
 * at `EOF`.  */
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

/** Skip block or line comments if the next read char is `/` or `*`,
 * respectively.  Otherwise push back this char and return `'/'`.
 *
 * \param istr The file handle to the input source.  Has to be opened for
 * reading.
 *
 * \return `EOF` if there was indeed a comment, `'/'` else.
 * \pre The last char read was a `'/'`.  */
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

/** Copy content of \a istr to \a ostr while skipping comments, string literals
 * and replacing successive white space by a single space.
 *
 * \param istr The file handle to the input source.  Has to be opened for
 * reading.
 * \param ostr The file handle where non-skipped text will be put.  Has to be
 * opened for writing.  Will be flushed after processing.
 *
 * \returns \e errno if some I/O error occurred else 0.
 * \post `feof(istr)` is true.  */
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

/** Store the parsed words of type \ref Word_s.  */
struct Word_frequency_s
{
    /** Root of the tree which sorts the \ref Word_s instances.  This is
     * manipulated by a POSIX binary tree from `<search.h>` (`twalk` and such).
     */
    void *tree_root;
    /** All strings and \e Word_s instances are put there.  */
    struct obstack word_stack;
};

/** One parsed word with the count of occurrences.  */
struct Word_s
{
    /** The number of occurrences of \p name.  */
    int count;
    /** The parsed word.  */
    char *name;
};

/** Comparator for \ref Word_s to use with a tree.
 * \param w1 Left word.
 * \param w2 Right word.
 * \return \a *w1 > \a *w2 with the same semantics as `strcmp`.  */
static int
word_compare (struct Word_s *w1, struct Word_s *w2)
{
    return strcmp (w1->name, w2->name);
}

/** Type erased pointer to \ref word_compare.  */
static comparison_fn_t word_cmp_void = (comparison_fn_t) word_compare;

/** Initialize a \ref Word_frequency_s instance.
 * \param words Location which will be set to a \e Word_frequency_s instance.
 * \return
 *   \li 0 on success,
 *   \li `EEXIST` if \a *words was not `NULL`,
 *   \li `ENOMEM` for allocation errors.  */
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

    obstack_alignment_mask (&(*words)->word_stack) = (sizeof (void *) - 0x1);

    return 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/** Function of type `__free_fn_t` for `tdestroy`.  Does nothing, since the
 * memory is managed via an `obstack`.
 * \param node \em Unused.*/
static void _tree_free_fn_nop (void *node) {}
#pragma GCC diagnostic pop

/** Free the resources of a \ref Word_frequency_s instance.  Usage of this
 * instance after cleanup yields undefined behavior.
 * \param words Free this.  */
void
wfreq_destroy (struct Word_frequency_s *words)
{
    if (!words)
        return;

    tdestroy (words->tree_root, _tree_free_fn_nop);
    obstack_free (&words->word_stack, NULL);
    free (words);
}

/** Simple parse state for a word.  */
enum Word_position {Word_begin, Word_end, Not_word};

/** Add the growing string on \a result_words->word_stack as a \ref Word_s to
 * the tree if the word was not yet encountered.  Increment the counter of a
 * word which was already there.
 * \param result_words Operate on this.
 * \pre A growing sequence of zero or more chars is on the obstack.
 * \post [The word was not yet present in the tree]
 *    \li The char sequence is terminated and finished (`obstack_finish`);
 *    \li a new corresponding \e Word_s instance is at the top of the obstack
 *    with count 1;
 *    \li this instance is inserted into the tree.
 * \post [The word was already present in the tree]
 *    \li the growing sequence is removed from the obstack;
 *    \li the count of the word in the tree is incremented.
 * */
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
        obstack_free (&result_words->word_stack, name);

    ++(*word_in_tree)->count;
}

/** Parse words from the stream \a istr, count their occurrence and add the
 * results to \a result_words.  Comments and string literals are ignored.
 * \sa remove_clutter
 *
 * \param istr The file handle to the input source.  Has to be opened for
 * reading.
 * \param result_words Words are put there.
 * \return \e errno if a stream error occured, 0 else.  */
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

/** Where the print_* functions print to.  Global since `__action_fn_t` (\c
 * twalk) does not allow to pass a state. */
static FILE *_current_ostr = NULL;

/** Action handler for \ref print_words_alpha_sorted.
 *
 * \warning Writes to the global \ref _current_ostr stream.
 *
 * \param word The tree node pointing to the current word.
 * \param which The `VISIT` order.
 * \param depth Current tree depth.  Not used.  */
static void
print_action_function_alpha_sorted (
    struct Word_s const **word, VISIT const which, int const depth)
{
    if (which == leaf || which == postorder)
        fprintf (_current_ostr, "%s\n", (*word)->name);
}

/** Action handler for \ref print_words_with_freq.
 * \copydetails print_action_function_alpha_sorted */
static void
print_action_function_with_freq (
    struct Word_s const **word, VISIT const which, int const depth)
{
    if (which == leaf || which == postorder)
        fprintf (_current_ostr, "%s [%d]\n", (*word)->name, (*word)->count);
}

/** Action handler for \ref print_words_raw.
 * \copydetails print_action_function_alpha_sorted */
static void
print_action_function_raw (
    struct Word_s const **word, VISIT const which, int const depth)
{
    if (which == leaf || which == postorder)
        for (int i = 0; i < (*word)->count; ++i)
            fprintf (_current_ostr, "%s\n", (*word)->name);
}

#pragma GCC diagnostic pop

/** Print all words of \a words to \a ostr in alphabetical order without the
 * number of occurrences.  Each word is on a separate line.
 *
 * \warning The global \ref _current_ostr stream should not be modified during
 * this call.
 *
 * \param ostr The file handle where to output to.  Has to be opened for
 * writing.
 * \param words Where the words and counts are.  */
void
print_words_alpha_sorted (FILE *ostr, struct Word_frequency_s *words)
{
    _current_ostr = ostr;
    twalk (
        words->tree_root, (__action_fn_t) print_action_function_alpha_sorted);
    _current_ostr = NULL;
}

/** Print all words of \a words to \a ostr in alphabetical order.
 * Each word is on a separate line followed by the count, e.g.
 * \code
 *      options.x [5]
 *      param [2]
 * \endcode
 * \copydetails print_words_alpha_sorted
 */
void
print_words_with_freq (FILE *ostr, struct Word_frequency_s *words)
{
    _current_ostr = ostr;
    twalk (words->tree_root, (__action_fn_t) print_action_function_with_freq);
    _current_ostr = NULL;
}

/** Print all words of \a words to \a ostr in alphabetical order and repeat each
 * word by the number of occurrences.
 * \copydetails print_words_alpha_sorted
 */
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
