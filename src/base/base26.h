// base/base26.h - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// base26.h

#ifndef Base26_H
#define Base26_H

#include <QString>

extern QString num2az(int n);
/*:F num2az
 *:D Maps positive numbers to strings of letters:
       1 -> a
       2 -> b
       26 -> z
       27 -> aa
       28 -> ab
       etc.
 *:N Negative numbers or zero return "@".
 */
extern int az2num(QString s);
/*:F az2num
 *:D The inverse of num2az.
 *:N Bad numbers return -1.
 */

#endif
