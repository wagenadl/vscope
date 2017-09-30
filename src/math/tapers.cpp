// math/tapers.cpp - This file is part of VScope.
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

// tapers.cpp

#include "tapers.h"

Tapers::Tapers() {
  length = 0;
  nTapers = 0;
}

Tapers::~Tapers() {
  rebuild(0,0);
}

void Tapers::rebuild(int len, int nTap) {
  if (nTap!=nTapers)
    tapers.resize(nTap);

  for (int k=0; k<nTap; k++) 
    if (tapers[k].size() != len)
      tapers[k].resize(len);
  
  length=len;
  nTapers=nTap;
}
