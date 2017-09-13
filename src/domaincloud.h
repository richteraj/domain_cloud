/** \file
 * Generate a word cloud from source files and show the domain as
 * expressed by the code. */

#ifndef DOMAINCLOUD_H_
#define DOMAINCLOUD_H_

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>

void print_version (FILE *ostr);
void print_usage (FILE *ostr);

int remove_clutter (FILE *istr, FILE *ostr);

#endif /* not DOMAINCLOUD_H_ */

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

