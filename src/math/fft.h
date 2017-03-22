// math/fft.h - This file is part of VScope.
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

// fft.h

#ifndef FFT_H

#define FFT_H

#include <math.h>
#include <math/cvec.h>
#include <math/fftinternals.h>

extern void FFT_insitu(cvec &x);
/*:F FFT
 *:D Computes the Fourier transform of the vector X.
 *:N Results are stored in-situ.
 *:N It is required that the length of X is a power of two. The current
     implementation further requires that that length is no greater than
     16,777,216. That number can easily be changed by adding "case"
     statements.
*/
  
#endif
