// base/istype.h - This file is part of VScope.
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

// istype.h

#ifndef ISTYPE_H

#define ISTYPE_H

template <class DstTyp, class SrcTyp>
bool isType(const SrcTyp *ptr) {
  return dynamic_cast<const DstTyp *>(ptr) != 0;
}
/*:F isType
 *:D isType<Y>(p) returns true iff p is a pointer to type Y.
 *:N This only works if the stated type of p is a parent of Y with at least
     one virtual function (i.e., if dynamic_cast<> can do the job).
*/

#endif
