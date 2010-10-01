// ccdregion.h

#ifndef CCDREGION_H

#define CCDREGION_H


class CCDRegion {
public:
  CCDRegion();
  CCDRegion(int smin, int smax, int pmin, int pmax);
  CCDRegion flipSerial(int serpix) const;
  CCDRegion flipParallel(int parpix) const;
public:
  int smin, smax;
  int pmin, pmax;
};


#endif
