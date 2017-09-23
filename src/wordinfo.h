/** \file
 * Store information about words.  */

#ifndef WORDINFO_H_
#define WORDINFO_H_

#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

struct Word_s;

bool word_store_string (struct Word_s *word, char *str, int len);
char const *word_name (struct Word_s const *word);

/** Comparator for \ref Word_s to use with a tree.
 * \param w1 Left word.
 * \param w2 Right word.
 * \return \a *w1 > \a *w2 with the same semantics as `strcmp`.  */
static inline int
word_compare (struct Word_s const *w1, struct Word_s const *w2)
{
    return strcmp (word_name (w1), word_name (w2));
}

enum Word_s_short_string_sizes
{
    head_Word_s_string_size = alignof (void *) - alignof (short),
    tail_Word_s_string_size = alignof (void *),
    total_Word_s_string_size = head_Word_s_string_size + alignof (void *)
};

static inline int
word_max_short_string_len ()
{
    return total_Word_s_string_size;
}

/** One parsed word with the count of occurrences.  */
struct Word_s
{
    /** The number of occurrences of \p name.  */
    short count;
    /** Head of internal string. Doubles as union tag if `short_name[0] == '\0'`
     * then we have a longer string.  */
    char short_name[head_Word_s_string_size];
    union
    {
        /** The parsed word.  */
        char *name;
        /** Tail of the short string.  */
        char short_name_tail[tail_Word_s_string_size];
    };
};

_Static_assert (
    offsetof (struct Word_s, short_name) == alignof (short),
    "Word_s.short_name starts at sizeof(short)");
_Static_assert (
    offsetof (struct Word_s, name) == alignof (void *),
    "Word_s.short_name starts at sizeof(void *)");
_Static_assert (
    2 * tail_Word_s_string_size == sizeof (struct Word_s),
    "Use all the space of Word_s");

#endif /* not WORDINFO_H_ */

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

