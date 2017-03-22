// base/minmax.h - This file is part of VScope.
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

// minmax.h

#ifndef MinMax_H
#define MinMax_H


#include <math.h>

inline double max(double a, double b) {
  return a>b ? a : b;
}

inline double min(double a, double b) {
  return a<b ? a : b;
}

inline int maxi(int a, int b) {
  return a>b ? a : b;
}

inline int mini(int a, int b) {
  return a<b ? a : b;
}

inline int ceili(double a) {
  return int(ceil(a));
}

inline int floori(double a) {
  return int(floor(a));
}

inline int roundi(double a) {
  return int(round(a));
}

#endif
