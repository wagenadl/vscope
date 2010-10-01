// fft.h

#ifndef FFT_H

#define FFT_H

#include <math.h>
#include <math/cvec.h>
#include <math/fftinternals.h>

extern void FFT_insitu(cvec &x);
/*:F FFT
 *:D Computes the Fourier transform of the vector X.
 *:N Results are stored in-situ.
 *:N It is required that the length of X is a power of two. The current
     implementation further requires that that length is no greater than
     16,777,216. That number can easily be changed by adding "case"
     statements.
*/
  
#endif
