// math/cohestci.h - This file is part of VScope.
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

// cohestci.h

#ifndef COHESTCI_H

#define COHESTCI_H

#include <math/taperid.h>
#include <math/tapers.h>
#include <math/cvec.h>

class CohEstCI {
public:
  CohEstCI();
  void compute(rvec const &xx, rvec const &yy, 
	       double dt, double df, Tapers const &tpr,
	       double ci_factor=1, double f_star=-1);
  /* set ci_factor=1 for 1xsigma error bars, or ci_factor=1.96 for 95% c.i. */
public: // results are stored in the following variables
  rvec freqbase;
  rvec magnitude;
  rvec magnitude_lo;
  rvec magnitude_hi;
  rvec phase;
  rvec phase_lo;
  rvec phase_hi;
  cvec coherence;
private: // these are internal, kept here to reduce reallocation
  cvec xtap;
  cvec ytap;
  QVector<cvec> pxy;
  QVector<rvec> pxx;
  QVector<rvec> pyy;
  rvec sumpxx;
  rvec sumpyy;
  cvec sumpxy;
  rvec sqrtsum;
  rvec mag_xf;
  cvec pxy_tao;
  rvec pxx_tao;
  rvec pyy_tao;
  rvec sqrtsum_tao;
  cvec coh_tao;
  rvec mag_tao;
  QVector<rvec> phase_tao;
  QVector<rvec> mag_tao_xf;
  cvec coh_tao_hat_mean;
  cvec coh_tao_hat;
  rvec mag_tao_xf_avg;
  rvec mag_xf_sig;
  rvec dmag_tao_xf;
  rvec phase_sigma;
};

#endif
