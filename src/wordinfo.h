/** \file
 * Store information about words.  */

#ifndef WORDINFO_H_
#define WORDINFO_H_

#include <string.h>

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
static inline int
word_compare (struct Word_s *w1, struct Word_s *w2)
{
    return strcmp (w1->name, w2->name);
}

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

