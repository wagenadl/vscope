// base/types.cpp - This file is part of VScope.
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

// types.cpp

#include <base/types.h>
#include <base/exception.h>
#include <stdio.h>
#include <base/dbg.h>

void checkTypes() {
  if (sizeof(int)<4)
    throw Exception("checkTypes","int has fewer than 32 bits.");
  if (sizeof(int8_t)!=1)
    throw Exception("checkTypes","int8 has incorrect size.");
  if (sizeof(int16_t)!=2)
    throw Exception("checkTypes","int16 has incorrect size.");
  if (sizeof(int32_t)!=4)
    throw Exception("checkTypes","int32 has incorrect size.");
  if (sizeof(int64_t)!=8)
    throw Exception("checkTypes","int64 has incorrect size.");
  
  if (sizeof(uint8_t)!=1)
    throw Exception("checkTypes","uint8 has incorrect size.");
  if (sizeof(uint16_t)!=2)
    throw Exception("checkTypes","uint16 has incorrect size.");
  if (sizeof(uint32_t)!=4)
    throw Exception("checkTypes","uint32 has incorrect size.");
  if (sizeof(uint64_t)!=8)
    throw Exception("checkTypes","uint64 has incorrect size.");

  if (sizeof(float)!=4)
    throw Exception("checkTypes","float has incorrect size.");
  if (sizeof(double)!=8)
    throw Exception("checkTypes","double has incorrect size.");
}
