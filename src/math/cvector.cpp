// cvector.cpp

#include "cvector.h"
#include <stdio.h>

void real(cvector const &src, rvector &dst) {
  matchsize(src,dst);
  double const *s = *src.data();
  double *d = dst.data();
  int n = src.size();
  while (n--) {
    *d++ = *s;
    s+=2;
  }
}

void imag(cvector const &src, rvector &dst) {
  matchsize(src,dst);
  double const *s = *src.data()+1;
  double *d = dst.data();
  int n = src.size();
  while (n--) {
    *d++ = *s;
    s+=2;
  }
}

void abs2(cvector const &src, rvector &dst) {
  matchsize(src,dst);
  double const *s = *src.data();
  double *d = dst.data();
  int n = src.size();
  while (n--) {
    double r = *s++;
    double i = *s++;
    *d++ = r*r + i*i;
  }
}

void abs(cvector const &src, rvector &dst) {
  matchsize(src,dst);
  double const *s = *src.data();
  double *d = dst.data();
  int n = src.size();
  while (n--) {
    double r = *s++;
    double i = *s++;
    *d++ = sqrt(r*r + i*i);
  }
}

void phase(cvector const &src, rvector &dst) {
  matchsize(src,dst);
  double const *s = *(src.data());
  double *d = dst.data();
  int n = src.size();
  while (n--) {
    double r = *s++;
    double i = *s++;
    *d++ = atan2(i,r);
  }
}

void conj(cvector const &src, cvector &dst) {
  matchsize(src,dst);
  double const *s = *(src.data());
  double *d = *(dst.data());
  int n = src.size();
  while (n--) {
    *d++ = *s++;
    *d++ = -*s++;
  }
}

void conj(cvector  &zz) {
  double  *s = *(zz.data()) + 1;
  int n = zz.size();
  while (n--) {
    *s = - *s;
    s+=2;
  }
}

void copy(rvector const &src, cvector &dst) {
  matchsize(src,dst);
  double const *s = src.data();
  double *d = *dst.data();
  int n = src.size();
  while (n--) {
    *d++ = *s++;
    *d++ = 0;
  }
}

void add(rvector const &zz1, rvector const &zz2, rvector &dst) {
  matchsize(zz1,dst);
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"rvector: add does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = zz1.data();
  double const *s2 = zz2.data();
  double *d = dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ = *s1++ + *s2++;
  }
}

void add(rvector const &zz1, rvector &dst) {
  matchsize(zz1,dst);
  double const *s1 = zz1.data();
  double *d = dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ += *s1++;
  }
}

void sub(rvector const &zz1, rvector const &zz2, rvector &dst) {
  matchsize(zz1,dst);
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"rvector: sub does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = zz1.data();
  double const *s2 = zz2.data();
  double *d = dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ = *s1++ - *s2++;
  }
}

void sub(rvector const &zz1, rvector &dst) {
  if (dst.size() != zz1.size()) {
    fprintf(stderr,"rvector: sub does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = zz1.data();
  double *d = dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ -= *s1++;
  }
}

void mul(rvector const &zz1, rvector const &zz2, rvector &dst) {
  matchsize(zz1,dst);
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"rvector: mul does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = zz1.data();
  double const *s2 = zz2.data();
  double *d = dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ = *s1++ * *s2++;
  }
}

void mul(rvector const &zz1, rvector &dst) {
  if (dst.size() != zz1.size()) {
    fprintf(stderr,"rvector: mul does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = zz1.data();
  double *d = dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ *= *s1++;
  }
}

void mulsqrt(rvector const &zz1, rvector const &zz2, rvector &dst) {
  matchsize(zz1,dst);
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"rvector: mul does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = zz1.data();
  double const *s2 = zz2.data();
  double *d = dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ = sqrt(*s1++ * *s2++);
  }
}

void mulsqrt(rvector const &zz1, rvector &zz2) {
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"rvector: mul does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = zz1.data();
  double *s2 = zz2.data();
  int n = zz1.size();
  while (n--) {
    *s2 = sqrt(*s2 * *s1++);
    s2++;
  }
}

void mul(rvector const &zz1, rvector const &zz2, cvector &dst) {
  matchsize(zz1,dst);
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"rvector: mul does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = zz1.data();
  double const *s2 = zz2.data();
  double *d = *(dst.data());
  int n = zz1.size();
  while (n--) {
    *d++ = *s1++ * *s2++;
    *d++ = 0;
  }
}

void add(cvector const &zz1, cvector const &zz2, cvector &dst) {
  matchsize(zz1,dst);
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"cvector: add(a,b,c) does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = *zz1.data();
  double const *s2 = *zz2.data();
  double *d = *dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ = *s1++ + *s2++;
    *d++ = *s1++ + *s2++;
  }
}

void add(cvector const &zz1, cvector &zz2) {
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"cvector: add(a,b) does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = *zz1.data();
  double *s2 = *zz2.data();
  int n = zz1.size();
  while (n--) {
    *s2++ += *s1++;
    *s2++ += *s1++;
  }
}

void sum(cvector const &zz1, Cplx &dst) {
  dst=Cplx();
  double const *s1 = *zz1.data();
  int n = zz1.size();
  while (n--) {
    dst.re += *s1++;
    dst.im += *s1++;
  }
}

void sub(cvector const &zz1, cvector const &zz2, cvector &dst) {
  matchsize(zz1,dst);
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"cvector: sub does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = *zz1.data();
  double const *s2 = *zz2.data();
  double *d = *dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ = *s1++ + *s2++;
    *d++ = *s1++ + *s2++;
  }
}

void sub(cvector const &zz1, cvector &dst) {
  if (dst.size() != zz1.size()) {
    fprintf(stderr,"cvector: sub does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = *zz1.data();
  double *d = *dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ -= *s1++;
    *d++ -= *s1++;
  }
}

void div(cvector const &zz1, rvector const &zz2, cvector &dst) {
  matchsize(zz1,dst);
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"cvector: div does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = *zz1.data();
  double const *s2 = zz2.data();
  double *d = *dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ = *s1++ / *s2;
    *d++ = *s1++ / *s2++;
  }
}

void div(cvector &zz1, rvector const &zz2) {
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"cvector: div does not work for unequal vectors. undefined output.\n");
    return;
  }
  double *s1 = *zz1.data();
  double const *s2 = zz2.data();
  int n = zz1.size();
  while (n--) {
    *s1++ /= *s2;
    *s1++ /= *s2++;
  }
}

void mul(cvector const &zz1, cvector const &zz2, cvector &dst) {
  matchsize(zz1,dst);
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"cvector: mul does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = *zz1.data();
  double const *s2 = *zz2.data();
  double *d = *dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ = s1[0]*s2[0] - s1[1]*s2[1];
    *d++ = s1[0]*s2[1] + s1[1]*s2[0];
    s1+=2;
    s2+=2;
  }
}

void mul(cvector const &zz1, cvector &zz2) {
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"cvector: mul does not work for unequal vectors. undefined output.\n");
    return;
  }
  double const *s1 = *zz1.data();
  double *s2 = *zz2.data();
  int n = zz1.size();
  while (n--) {
    double x = s1[0]*s2[0] - s1[1]*s2[1];
    double y = s1[0]*s2[1] + s1[1]*s2[0];
    *s2++ = x;
    *s2++ = y;
    s1+=2;
  }
}

void mulconj(cvector const &zz1, cvector const &zz2, cvector &dst) {
  matchsize(zz1,dst);
  if (zz2.size() != zz1.size()) {
    fprintf(stderr,"cvector: mulconj does not work for unequal-size vectors. undefined output.\n");
    return;
  }
  double const *s1 = *zz1.data();
  double const *s2 = *zz2.data();
  double *d = *dst.data();
  int n = zz1.size();
  while (n--) {
    *d++ = s1[0]*s2[0] + s1[1]*s2[1];
    *d++ = -s1[0]*s2[1] + s1[1]*s2[0];
    s1+=2;
    s2+=2;
  }
}

void cmul(cvector const &src, double c, cvector &dst) {
  matchsize(src,dst);
  double const *s = *(src.data());
  double *d = *(dst.data());
  int n = src.size();
  while (n--) {
    *d++ = c * *s++;
    *d++ = c * *s++;
  }
}

void cmul(cvector &zz, double c) {
  double *s = *(zz.data());
  int n = zz.size();
  while (n--) {
    *s++ *= c;
    *s++ *= c;
  }
}

void cmul(rvector const &src, double c, rvector &dst) {
  matchsize(src,dst);
  double const *s = src.data();
  double *d = dst.data();
  int n = src.size();
  while (n--) {
    *d++ = c * *s++;
  }
}

void cmul(rvector &zz, double c) {
  double *s = zz.data();
  int n = zz.size();
  while (n--) {
    *s++ *= c;
  }
}
  
void matchsize(rvector const &src, rvector &dst) {
  if (dst.size() != src.size())
    dst.resize(src.size());
}
  
void matchsize(cvector const &src, rvector &dst) {
  if (dst.size() != src.size())
    dst.resize(src.size());
}
    
void matchsize(cvector const &src, cvector &dst) {
  if (dst.size() != src.size())
    dst.resize(src.size());
}

void matchsize(rvector const &src, cvector &dst) {
  if (dst.size() != src.size())
    dst.resize(src.size());
}
