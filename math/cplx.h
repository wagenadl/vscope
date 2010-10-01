// cplx.h

#ifndef CPLX_H

#define CPLX_H

#include <math.h>

class Cplx {
public:
  /* Cplx stores a complex number as the real part followed by the imaginary
     part. */
  Cplx(double re=0, double im=0): re(re), im(im) {  }
  operator double const *() const { return &re; }
  operator double *() { return &re; }
public:
  double re;
  double im;
};

inline double real(Cplx const &z) { return z.re; }
inline double imag(Cplx const &z) { return z.im; }
inline double abs2(Cplx const &z) { return z.re*z.re + z.im*z.im; }
inline double abs(Cplx const &z) { return sqrt(abs2(z)); }
inline double phase(Cplx const &z) { return atan2(z.im,z.re); }

#endif
