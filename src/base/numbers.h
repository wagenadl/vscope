// numbers.h

#ifndef Numbers_H
#define Numbers_H

#include <limits>
#include <cmath>

class Numbers {
public:
  static double pi() { static double p=2*std::acos(0); return p; }
  static double nan() { static double n = std::nan(""); return n; }
  static double inf() { return std::numeric_limits<double>::infinity(); }
  static bool isNaN(double x) { return std::isnan(x); }
};

#endif
