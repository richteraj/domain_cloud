/** \file
 * Store information about words.  */

#include "wordinfo.h"

bool
word_store_string (struct Word_s *word, char *str, int len)
{
    if (len < total_Word_s_string_size)
    {
        memcpy (word->short_name, str, len);
        word->short_name[len] = '\0';
        return true;
    }
    else
    {
        word->short_name[0] = '\0';
        word->name = str;
        return false;
    }
}

char const *
word_name (struct Word_s const *word)
{
    if (!word->short_name[0])
        return word->name;
    else
        return word->short_name;
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

