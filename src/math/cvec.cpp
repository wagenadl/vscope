// math/cvec.cpp - This file is part of VScope.
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

// cvec.cpp

#include "cvec.h"
#include <base/dbg.h>

//////////////////////// rvec bookkeeping ////////////////////////////
rvec::rvec(): QVector<double>(0) {
}

void rvec::matchsize(cvec const &src) {
  if (size()!=src.size())
    resize(src.size());
}

/////////////////////// rvec to rvec /////////////////////////////////
rvec &rvec::operator=(rvec const &src) {
  matchsize(src);
  double const *s = src.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ = *s++;
  return *this;
}

double sum(rvec const &src) {
  double v=0;
  double const *s = src.data();
  int n = src.size();
  while (n--)
   v += *s++;
  return v;
}

rvec &rvec::operator+=(rvec const &src) {
  assertsizematch(src);
  double const *s = src.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ += *s++;
  return *this;
}

rvec &rvec::operator-=(rvec const &src) {
  assertsizematch(src);
  double const *s = src.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ -= *s++;
  return *this;
}


rvec &rvec::operator*=(rvec const &src) {
  assertsizematch(src);
  double const *s = src.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ *= *s++;
  return *this;
}

rvec &rvec::operator/=(rvec const &src) {
  assertsizematch(src);
  double const *s = src.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ /= *s++;
  return *this;
}

rvec &rvec::operator+=(double src) {
  double *d = data();
  int n = size();
  while (n--) 
    *d++ += src;
  return *this;
}

rvec &rvec::operator-=(double src) {
  double *d = data();
  int n = size();
  while (n--) 
    *d++ -= src;
  return *this;
}

rvec &rvec::operator*=(double src) {
  double *d = data();
  int n = size();
  while (n--) 
    *d++ *= src;
  return *this;
}

rvec &rvec::operator/=(double src) {
  double *d = data();
  int n = size();
  while (n--) 
    *d++ /= src;
  return *this;
}

rvec &rvec::operator=(rvec::rvec_add const &src) {
  matchsize(src.a);
  double const *sa = src.a.data();
  double const *sb = src.b.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ = *sa++ + *sb++;
  return *this;
}

rvec &rvec::operator=(rvec::rvec_sub const &src) {
  matchsize(src.a);
  double const *sa = src.a.data();
  double const *sb = src.b.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ = *sa++ - *sb++;
  return *this;
}

rvec &rvec::operator=(rvec::rvec_mul const &src) {
  matchsize(src.a);
  double const *sa = src.a.data();
  double const *sb = src.b.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ = *sa++ * *sb++;
  return *this;
}

rvec &rvec::operator=(rvec::rvec_div const &src) {
  matchsize(src.a);
  double const *sa = src.a.data();
  double const *sb = src.b.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ = *sa++ / *sb++;
  return *this;
}

rvec &rvec::operator=(rvec::rvec_mulsqrt const &src) {
  matchsize(src.a);
  double const *sa = src.a.data();
  double const *sb = src.b.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ = ::sqrt(*sa++ * *sb++);
  return *this;
}


rvec &rvec::operator=(rvec::rvec_sqrt const &src) {
  matchsize(src.a);
  double const *sa = src.a.data();
  double *d = data();
  int n = size();
  while (n--) 
    *d++ = ::sqrt(*sa++);
  return *this;
}

rvec &rvec::operator=(rvec::rvec_algcsqr const &src) {
  matchsize(src.a);
  double const *x = src.a.data();
  double *y = data();
  int n = size();
  while (n--) {
    double z = *x++;
    z *= z;
    if (z<1e-15)
      z=1e-15;
    if (z>1-1e-15)
      z=1e-15;
    *y++ = -log(1/z - 1);
  }
  return *this;
}

rvec &rvec::operator=(rvec::rvec_sqrtlgc const &src) {
  matchsize(src.a);
  double const *x = src.a.data();
  double *y = data();
  int n = size();
  while (n--)
    *y++ = 1/sqrt(1+exp(-*x++));
  return *this;
}


//////////////////// cvec to rvec ////////////////////////////////////
rvec &rvec::operator=(cvec_real const &src) {
  matchsize(src.a);
  double const *sa = *src.a.data();
  double *d = data();
  int n = size();
  while (n--) {
    *d++ = *sa;
    sa+=2;
  }
  return *this;
}

rvec &rvec::operator=(cvec_imag const &src) {
  matchsize(src.a);
  double const *sa = *src.a.data() + 1;
  double *d = data();
  int n = size();
  while (n--) {
    *d++ = *sa;
    sa+=2;
  }
  return *this;
}

rvec &rvec::operator=(cvec_abs2 const &src) {
  matchsize(src.a);
  double const *sa = *src.a.data();
  double *d = data();
  int n = size();
  while (n--) {
    double r = *sa++;
    double c = *sa++;
    *d++ = r*r + c*c;
  }
  return *this;
}

rvec &rvec::operator=(cvec_abs const &src) {
  matchsize(src.a);
  double const *sa = *src.a.data();
  double *d = data();
  int n = size();
  while (n--) {
    double r = *sa++;
    double c = *sa++;
    *d++ = ::sqrt(r*r + c*c);
  }
  return *this;
}

rvec &rvec::operator=(cvec_phase const &src) {
  matchsize(src.a);
  double const *sa = *src.a.data();
  double *d = data();
  int n = size();
  while (n--) {
    double r = *sa++;
    double c = *sa++;
    *d++ = ::atan2(c,r);
  }
  return *this;
}

/////////////////////// cvec to cvec /////////////////////////////////
cvec::cvec(): QVector<Cplx>(0) {
}


cvec &cvec::operator=(cvec const &src) {
  matchsize(src);
  double const *s = *src.data();
  double *d = *data();
  int n = 2*size();
  while (n--) 
    *d++ = *s++;
  return *this;
}

cvec &cvec::operator=(rvec const &src) {
  matchsize(src);
  double const *s = src.data();
  double *d = *data();
  int n = size();
  while (n--) {
    *d++ = *s++;
    *d++ = 0;
  }
  return *this;
}

Cplx sum(cvec const &src) {
  double const *s = *src.data();
  double re=0;
  double im=0;
  int n = src.size();
  while (n--) {
    re += *s++;
    im += *s++;
  }
  return Cplx(re,im);
}

cvec &cvec::operator+=(cvec const &src) {
  assertsizematch(src);
  double const *s = *src.data();
  double *d = *data();
  int n = 2*size();
  while (n--) 
    *d++ += *s++;
  return *this;
}

cvec &cvec::operator-=(cvec const &src) {
  assertsizematch(src);
  double const *s = *src.data();
  double *d = *data();
  int n = 2*size();
  while (n--) 
    *d++ -= *s++;
  return *this;
}

cvec &cvec::operator*=(cvec const &src) {
  assertsizematch(src);
  double const *s = *src.data();
  double *d = *data();
  int n = size();
  while (n--) {
    double x = s[0]*d[0] - s[1]*d[1];
    double y = s[0]*d[1] + s[1]*d[0];
    *d++ = x;
    *d++ = y;
    s+=2;
  }
  return *this;
}

cvec &cvec::operator/=(rvec const &src) {
  assertsizematch(src);
  double const *s = src.data();
  double *d = *data();
  int n = size();
  while (n--) {
    double y = *s++;
    *d++ /= y;
    *d++ /= y;
  }
  return *this;
}

cvec &cvec::operator+=(Cplx const &src) {
  double r = src.re;
  double i = src.im;
  double *d = *data();
  int n = size();
  while (n--) {
    *d++ += r;
    *d++ += i;
  }
  return *this;
}

cvec &cvec::operator-=(Cplx const &src) {
  double r = src.re;
  double i = src.im;
  double *d = *data();
  int n = size();
  while (n--) {
    *d++ -= r;
    *d++ -= i;
  }
  return *this;
}

cvec &cvec::operator*=(double src) {
  double *d = *data();
  int n = 2*size();
  while (n--) 
    *d++ *= src;
  return *this;
}

cvec &cvec::operator/=(double src) {
  double *d = *data();
  int n = 2*size();
  while (n--) 
    *d++ /= src;
  return *this;
}

cvec &cvec::conj() {
  double *d = *data() + 1;
  int n = size();
  while (n--) {
    *d= -*d;
    d+=2;
  }
  return *this;
}

cvec &cvec::operator=(cvec::cvec_add const &src) {
  matchsize(src.a);
  double const *sa = *src.a.data();
  double const *sb = *src.b.data();
  double *d = *data();
  int n = 2*size();
  while (n--) 
    *d++ = *sa++ + *sb++;
  return *this;
}

cvec &cvec::operator=(cvec::cvec_sub const &src) {
  matchsize(src.a);
  double const *sa = *src.a.data();
  double const *sb = *src.b.data();
  double *d = *data();
  int n = 2*size();
  while (n--) 
    *d++ = *sa++ - *sb++;
  return *this;
}

cvec &cvec::operator=(cvec::cvec_mul const &src) {
  matchsize(src.a);
  double const *sa = *src.a.data();
  double const *sb = *src.b.data();
  double *d = *data();
  int n = size();
  while (n--) {
    double x = sa[0]*sb[0] - sa[1]*sb[1];
    double y = sa[0]*sb[1] + sa[1]*sb[0];
    *d++ = x;
    *d++ = y;
  }
  return *this;
}

cvec &cvec::operator=(rvec::rvec_mul const &src) {
  matchsize(src.a);
  double const *sa = src.a.data();
  double const *sb = src.b.data();
  double *d = *data();
  int n = size();
  while (n--) {
    *d++ = *sa++ * *sb++;
    *d++ = 0;
  }
  return *this;
}

cvec &cvec::operator=(cvec::cvec_mulconj const &src) {
  matchsize(src.a);
  double const *sa = *src.a.data();
  double const *sb = *src.b.data();
  double *d = *data();
  int n = size();
  while (n--) {
    double x = sa[0]*sb[0] + sa[1]*sb[1];
    double y = -sa[0]*sb[1] + sa[1]*sb[0];
    *d++ = x;
    *d++ = y;
    sa+=2;
    sb+=2;
  }
  return *this;
}

cvec &cvec::operator=(cvec::cvec_div const &src) {
  matchsize(src.a);
  double const *sa = *src.a.data();
  double const *sb = src.b.data();
  double *d = *data();
  int n = size();
  while (n--) {
    double y = *sb++;
    *d++ = *sa++ / y;
    *d++ = *sa++ / y;
  }
  return *this;
}
