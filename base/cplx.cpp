// cplx.cpp

#include "cplx.h"

bool operator==(cplx a, cplx b) {
  return a.re==b.re && a.im==b.im;
}

cplx operator+(cplx a, cplx b) {
  return cplx(a.re+b.re, a.im+b.im);
}

cplx operator-(cplx a, cplx b) {
  return cplx(a.re-b.re, a.im-b.im);
}

cplx operator-(cplx a) {
  return cplx(-a.re, -a.im);
}

cplx operator*(cplx a, cplx b) {
  return cplx(a.re*b.re - a.im*b.im, a.re*b.im + a.im*b.re);
}

cplx operator/(cplx a, cplx b) {
  double denom = b.re*b.re + b.im*b.im;
  return cplx((a.re*b.re + a.im*b.im)/denom, (a.im*b.re - a.re*b.im)/denom);
}

double fabs(cplx a) {
  return sqrt(a.re*a.re + a.im*a.im);
}

double arg(cplx a) {
  return atan2(a.im,a.re);
}

cplx sqrt(cplx a) {
  double len = fabs(a);
  cplx r= cplx(sqrt((len+a.re)/2), sqrt((len-a.re)/2)*(a.im<0?-1:1));
  return r;
}

cplx curt(cplx a) {
  double len = fabs(a);
  double phi = arg(a);
  len = pow(len,1./3);
  phi = phi/3;
  return cplx(len*cos(phi),len*sin(phi));
}


cplx cusq(cplx a) {
  double len = fabs(a);
  double phi = arg(a);
  len = pow(len,2./3);
  phi = phi*2./3;
  return cplx(len*cos(phi),len*sin(phi));
}
