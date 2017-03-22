// base/xyabc.h - This file is part of VScope.
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

// xyabc.h

#ifndef XYABC_H
#define XYABC_H

#include <base/xyrra.h>

class XYABC {
  /*:C XYABC
   *:D Ellipse in the representation

         a*(x-x0)^2 + b*(y-y0)^2 + c*(x-x0)*(y-y0) = 1.
  */
public:
  XYABC(double x0=0, double y0=0, double a=1, double b=1, double c=0);
  XYABC(XYRRA const &xyrra);
  XYRRA toXYRRA() const;
public:
  double x0, y0;
  double a, b, c;
};

#endif
