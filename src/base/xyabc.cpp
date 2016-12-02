// xyabc.cpp

#include "xyabc.h"
#include <math.h>

XYABC::XYABC(double x0, double y0, double a, double b, double c):
  x0(x0), y0(y0), a(a), b(b), c(c) {
}

static double sq(double a) {
  return a*a;
}

XYABC::XYABC(XYRRA const &xyrra) {
  /* This is based on my matlab code, elrra2abc.m */
  x0 = xyrra.x0;
  y0 = xyrra.y0;
  double cs = cos(xyrra.a);
  double sn = sin(xyrra.a);
  a = sq(cs)/sq(xyrra.R) + sq(sn)/sq(xyrra.r);
  b = sq(sn)/sq(xyrra.R) + sq(cs)/sq(xyrra.r);
  c = cs*sn*(1/sq(xyrra.R) - 1/sq(xyrra.r));
}

XYRRA XYABC::toXYRRA() const {
  XYRRA xyrra;
  xyrra.x0 = x0;
  xyrra.y0 = y0;
  xyrra.a = 0.5*atan2(2*c, a-b);
  double cs = cos(xyrra.a);
  double sn = sin(xyrra.a);
  double pi = 2*acos(0);
  xyrra.R = 1/sqrt(a*sq(cs) + b*sq(sn) + 2*c*cs*sn);
  xyrra.r = 1/sqrt(a*sq(sn) + b*sq(cs) - 2*c*cs*sn);
  if (xyrra.R<xyrra.r) {
    double r = xyrra.r;
    xyrra.r = xyrra.R;
    xyrra.R = r;
    if (xyrra.a>=pi)
      xyrra.a -= 2*pi;
  }
  return xyrra;
}
