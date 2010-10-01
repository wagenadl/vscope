// tapers.h

#ifndef TAPERS_H

#define TAPERS_H

#include <math/cvec.h>

class Tapers {
public:
  Tapers();
  ~Tapers();
  void rebuild(int length, int nTapers);
  rvec const &taper(int k) const { return tapers[k]; }
  rvec const &tapr(int k) const { return *((rvec*)&tapers[k]); }
public:
  QVector<rvec> tapers;
  int length;
  int nTapers;
};


#endif
