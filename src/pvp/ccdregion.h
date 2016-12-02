// ccdregion.h

#ifndef CCDREGION_H

#define CCDREGION_H

#include <QRect>

class CCDRegion {
public:
  CCDRegion();
  CCDRegion(int smin, int smax, int pmin, int pmax);
  CCDRegion(QRect const &);
  CCDRegion flipSerial(int serpix) const;
  CCDRegion flipParallel(int parpix) const;
public:
  int smin, smax; // inclusive, so for a 512-pixel camera, smin=0, smax=511.
  int pmin, pmax;
};


#endif
