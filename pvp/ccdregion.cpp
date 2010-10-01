// ccdregion.cpp

#include "ccdregion.h"

CCDRegion::CCDRegion() {
  smin=0;
  smax=511;

  pmin=0;
  pmax=511;
}

CCDRegion::CCDRegion(int smin_, int smax_, int pmin_, int pmax_) {
  smin = smin_;
  smax = smax_;

  pmin = pmin_;
  pmax = pmax_;
}

CCDRegion CCDRegion::flipSerial(int serpix) const {
  CCDRegion r = *this;
  r.smin = serpix-1-smax;
  r.smax = serpix-1-smin;
  return r;
}

CCDRegion CCDRegion::flipParallel(int parpix) const {
  CCDRegion r = *this;
  r.pmin = parpix-1-pmax;
  r.pmax = parpix-1-pmin;
  return r;
}
