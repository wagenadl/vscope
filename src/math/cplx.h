// math/cplx.h - This file is part of VScope.
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

class Cplx {
public:
  /* Cplx stores a complex number as the real part followed by the imaginary
     part. */
  Cplx(double re=0, double im=0): re(re), im(im) {  }
  operator double const *() const { return &re; }
  operator double *() { return &re; }
public:
  double re;
  double im;
};

inline double real(Cplx const &z) { return z.re; }
inline double imag(Cplx const &z) { return z.im; }
inline double abs2(Cplx const &z) { return z.re*z.re + z.im*z.im; }
inline double abs(Cplx const &z) { return sqrt(abs2(z)); }
inline double phase(Cplx const &z) { return atan2(z.im,z.re); }

#endif
