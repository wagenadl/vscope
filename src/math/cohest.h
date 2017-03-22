// math/cohest.h - This file is part of VScope.
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

// cohest.h

#ifndef COHEST_H

#define COHEST_H

#include <math/taperid.h>
#include <math/tapers.h>
#include <math/cvec.h>

class CohEst {
public:
  CohEst();
  void compute(rvec const &xx, rvec const &yy,
	       double dt, double df, Tapers const &tpr,
	       double fstar=-1);
public:
  rvec freqbase;
  rvec magnitude;
  rvec phase;
  cvec coherence;
private:
  cvec xtap;
  cvec ytap;
  cvec pxy;
  rvec pxx;
  rvec pyy;
  rvec sumpxx;
  rvec sumpyy;
  cvec sumpxy;
};

#endif
