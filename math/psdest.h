// psdest.h

#ifndef PSDEST_H

#define PSDEST_H

#include <math/taperid.h>
#include <math/tapers.h>
#include <math/cvec.h>

class PSDEst {
public:
  PSDEst();
  ~PSDEst();
  void compute(rvec const &xx,
	       double dt, double df, Tapers const &tpr);
public:
  rvec freqbase; 
  rvec psd;
  QVector<rvec> psds; // individual tapered results
private:
  cvec xtap;
};

#endif
