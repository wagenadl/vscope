// numbers.h

#ifndef Numbers_H
#define Numbers_H

#include <limits>
#include <cmath>

class Numbers {
public:
  static inline double pi() { static double p=2*std::acos(0); return p; }
  static inline double nan() { static double n = std::nan(""); return n; }
  static inline double inf() { return std::numeric_limits<double>::infinity(); }
  static inline bool isNaN(double x) { return std::isnan(x); }
};

#endif
