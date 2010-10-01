// xyabc.h

#ifndef XYABC_H
#define XYABC_H

#include <base/xyrra.h>

class XYABC {
  /*:C XYABC
   *:D Ellipse in the representation

         a*(x-x0)^2 + b*(y-y0)^2 + c*(x-x0)*(y-y0) = 1.
  */
public:
  XYABC(double x0=0, double y0=0, double a=1, double b=1, double c=0);
  XYABC(XYRRA const &xyrra);
  XYRRA toXYRRA() const;
public:
  double x0, y0;
  double a, b, c;
};

#endif
