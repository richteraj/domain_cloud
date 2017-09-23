/** \file
 * Parse words from an input stream.  */

#ifndef EXTRACTWORDS_H_
#define EXTRACTWORDS_H_

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

int remove_clutter (FILE *istr, FILE *ostr);

struct Word_frequency_s;

int wfreq_init (struct Word_frequency_s **words);
void wfreq_destroy (struct Word_frequency_s *words);

int count_words (FILE *istr, struct Word_frequency_s *result_words);
void print_words_alpha_sorted (FILE *ostr, struct Word_frequency_s *words);
void print_words_with_freq (FILE *ostr, struct Word_frequency_s *words);
void print_words_raw (FILE *ostr, struct Word_frequency_s *words);

/** Defines the possible first letter of a word.
 * \param chr Test against this.
 * \return True if \a chr may start a word.  Else false.  */
static inline bool
is_identifier (int chr)
{
    return isalpha (chr) || chr == '.' || chr == '_';
}

#endif /* not EXTRACTWORDS_H_ */

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
