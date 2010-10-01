// cplx.h

#ifndef CPLX_H

#define CPLX_H

#include <math.h>

class cplx {
public:
  cplx(double re=0, double im=0): re(re), im(im) {
  }
  double re;
  double im;
};

extern bool operator==(cplx a, cplx b);
extern cplx operator+(cplx a, cplx b);
extern cplx operator-(cplx a, cplx b);
extern cplx operator-(cplx a);
extern cplx operator*(cplx a, cplx b);
extern cplx operator/(cplx a, cplx b);
extern double fabs(cplx a);
extern double arg(cplx a);
extern cplx sqrt(cplx a);
extern cplx curt(cplx a);
extern cplx cusq(cplx a);

#endif
