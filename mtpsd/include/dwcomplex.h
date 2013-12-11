// dwcomplex.h

#ifndef DWCOMPLEX_H

#define DWCOMPLEX_H

#include <complex>

#define complex std::complex<double>
#define dwcomplex complex
#define I dwcomplex_I
#define _Complex_I dwcomplex_I

const dwcomplex dwcomplex_I(0,1);

#endif
