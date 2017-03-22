// math/cvec.h - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// cvec.h

#ifndef CVEC_H

#define CVEC_H

#include <QVector>
#include <math/cplx.h>
#include <stdio.h>
#include <math.h>
#include <base/exception.h>

class cvec;
class rvec;

#define RVEC_ONE(x) \
  class x {	    \
  public: \
    x(rvec const &a): a(a) { } \
    rvec const &a; \
  };

#define RVEC_TWO(x)				\
  class x { \
  public: \
    x(rvec const &a, rvec const &b): a(a), b(b) { a.assertsizematch(b); } \
    rvec const &a; \
    rvec const &b; \
  };

#define CVEC_ONE(x) \
  class x {	    \
  public: \
    x(cvec const &a): a(a) { } \
    cvec const &a; \
  }; 

#define CVEC_TWO(x)				\
  class x { \
  public: \
    x(cvec const &a, cvec const &b): a(a), b(b) { a.assertsizematch(b); } \
    cvec const &a; \
    cvec const &b; \
  };

#define CVEC_RVEC(x)				\
  class x { \
  public: \
    x(cvec const &a, rvec const &b): a(a), b(b) { a.assertsizematch(b); } \
    cvec const &a; \
    rvec const &b; \
  };

CVEC_ONE(cvec_abs);
CVEC_ONE(cvec_abs2);
CVEC_ONE(cvec_real);
CVEC_ONE(cvec_imag);
CVEC_ONE(cvec_phase);

class rvec: public QVector<double> {
public: // really private
  RVEC_ONE(rvec_sqrt);
  RVEC_ONE(rvec_algcsqr);
  RVEC_ONE(rvec_sqrtlgc);
  RVEC_TWO(rvec_add);
  RVEC_TWO(rvec_sub);
  RVEC_TWO(rvec_mul);
  RVEC_TWO(rvec_div);
  RVEC_TWO(rvec_mulsqrt);
public:
  rvec();
  rvec &operator=(rvec const &src);
  rvec &operator+=(rvec const &src);
  rvec &operator-=(rvec const &src);
  rvec &operator*=(rvec const &src);
  rvec &operator/=(rvec const &src);

  rvec &operator+=(double src);
  rvec &operator-=(double src);
  rvec &operator*=(double src);
  rvec &operator/=(double src);
  
  rvec &operator=(rvec_add const &src);
  rvec &operator=(rvec_sub const &src);
  rvec &operator=(rvec_mul const &src);
  rvec &operator=(rvec_div const &src);
  rvec &operator=(rvec_sqrt const &src);
  rvec &operator=(rvec_algcsqr const &src);
  rvec &operator=(rvec_sqrtlgc const &src);
  rvec &operator=(rvec_mulsqrt const &src);

  rvec &operator=(cvec_abs const &src);
  rvec &operator=(cvec_abs2 const &src);
  rvec &operator=(cvec_phase const &src);
  rvec &operator=(cvec_real const &src);
  rvec &operator=(cvec_imag const &src);

public: // really private
  inline void matchsize(rvec const &src) {
    if (size()!=src.size())
      resize(src.size());
  }
  inline void matchsize(cvec const &src);
  inline void assertsizematch(rvec const &src) const {
    if (src.size() == size())
      return;
    throw Exception("rvec","size mismatch");
  }    
};

double sum(rvec const &a);

inline rvec::rvec_add operator+(rvec const &a, rvec const &b) {
  return rvec::rvec_add(a,b);
}

inline rvec::rvec_sub operator-(rvec const &a, rvec const &b) {
  return rvec::rvec_sub(a,b);
}

inline rvec::rvec_mul operator*(rvec const &a, rvec const &b) {
  return rvec::rvec_mul(a,b);
}

inline rvec::rvec_div operator/(rvec const &a, rvec const &b) {
  return rvec::rvec_div(a,b);
}

inline rvec::rvec_mulsqrt mulsqrt(rvec const &a, rvec const &b) {
  return rvec::rvec_mulsqrt(a,b);
}

inline rvec::rvec_sqrt sqrt(rvec const &a) {
  return rvec::rvec_sqrt(a);
}

inline rvec::rvec_algcsqr algcsqr(rvec const &a) {
  return rvec::rvec_algcsqr(a);
}

inline rvec::rvec_sqrtlgc sqrtlgc(rvec const &a) {
  return rvec::rvec_sqrtlgc(a);
}

//////////////////////////////////////////////////////////////////////

class cvec: public QVector<Cplx> {
public: // really private
  //  CVEC_ONE(cvec_sqrt);
  CVEC_ONE(cvec_conj);
  CVEC_TWO(cvec_add);
  CVEC_TWO(cvec_sub);
  CVEC_TWO(cvec_mul);
  CVEC_RVEC(cvec_div);
  CVEC_TWO(cvec_mulconj);
public:
  cvec();
  cvec &operator=(rvec const &src);
  cvec &operator=(cvec const &src);
  cvec &operator+=(cvec const &src);
  cvec &operator-=(cvec const &src);
  cvec &operator*=(cvec const &src);
  cvec &operator/=(rvec const &src);

  cvec &operator+=(Cplx const &src);
  cvec &operator-=(Cplx const &src);
  cvec &operator*=(double src);
  cvec &operator/=(double src);
  
  cvec &operator=(cvec_add const &src);
  cvec &operator=(cvec_sub const &src);
  cvec &operator=(rvec::rvec_mul const &src);
  cvec &operator=(cvec_mul const &src);
  cvec &operator=(cvec_mulconj const &src);
  cvec &operator=(cvec_div const &src);

  cvec &conj();
  
public: // really private
  inline void matchsize(cvec const &src) {
    if (size()!=src.size())
      resize(src.size());
  }
  inline void matchsize(rvec const &src) {
    if (size()!=src.size())
      resize(src.size());
  }
  inline void assertsizematch(cvec const &src) const {
    if (src.size() == size())
      return;
    throw Exception("cvec","size mismatch");
  }
  inline void assertsizematch(rvec const &src) const {
    if (src.size() == size())
      return;
    throw Exception("cvec","size mismatch w/ rvec");
  }
};

inline cvec::cvec_add operator+(cvec const &a, cvec const &b) {
  return cvec::cvec_add(a,b);
}

inline cvec::cvec_sub operator-(cvec const &a, cvec const &b) {
  return cvec::cvec_sub(a,b);
}

inline cvec::cvec_mul operator*(cvec const &a, cvec const &b) {
  return cvec::cvec_mul(a,b);
}

inline cvec::cvec_mulconj mulconj(cvec const &a, cvec const &b) {
  return cvec::cvec_mulconj(a,b);
}

inline cvec::cvec_div operator/(cvec const &a, rvec const &b) {
  return cvec::cvec_div(a,b);
}

inline cvec::cvec_conj conj(cvec const &a) {
  return cvec::cvec_conj(a);
}

inline cvec_real real(cvec const &a) {
  return cvec_real(a);
}
inline cvec_imag imag(cvec const &a) {
  return cvec_imag(a);
}
inline cvec_abs abs(cvec const &a) {
  return cvec_abs(a);
}
inline cvec_abs2 abs2(cvec const &a) {
  return cvec_abs2(a);
}
inline cvec_phase phase(cvec const &a) {
  return cvec_phase(a);
}

Cplx sum(cvec const &a);

#endif
