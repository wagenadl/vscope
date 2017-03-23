// base/numbers.h - This file is part of VScope.
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

// numbers.h

#ifndef Numbers_H
#define Numbers_H

#include <limits>
#include <cmath>

class Numbers {
public:
  static inline double pi() { static double p=2*std::acos(0); return p; }
  static inline double nan() { static double n = std::nan(""); return n; }
  static inline double inf() { return std::numeric_limits<double>::infinity(); }
  static inline bool isNaN(double x) { return std::isnan(x); }
};

#endif
