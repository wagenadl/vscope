// pvp/ccdtrigmode.h - This file is part of VScope.
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

// ccdtrigmode.h

#ifndef CCDTRIGMODE_H

#define CCDTRIGMODE_H

class CCDTrigMode {
public:
  enum V {
    Immediate,
    FirstFrame,
    EachFrame,
    Bulb,
  };
  CCDTrigMode(V x=Immediate);
  operator int() const { return x; }
  char const *toString() const;
private:
  V x;
};


#endif
