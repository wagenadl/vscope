// base/cplx.h - This file is part of VScope.
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

// cplx.h

#ifndef CPLX_H

#define CPLX_H

#include <math.h>

class cplx {
public:
  cplx(double re=0, double im=0): re(re), im(im) {
  }
  double re;
  double im;
};

extern bool operator==(cplx a, cplx b);
extern cplx operator+(cplx a, cplx b);
extern cplx operator-(cplx a, cplx b);
extern cplx operator-(cplx a);
extern cplx operator*(cplx a, cplx b);
extern cplx operator/(cplx a, cplx b);
extern double fabs(cplx a);
extern double arg(cplx a);
extern cplx sqrt(cplx a);
extern cplx curt(cplx a);
extern cplx cusq(cplx a);

#endif
