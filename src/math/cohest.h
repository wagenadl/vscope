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
