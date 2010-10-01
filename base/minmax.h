// minmax.h

#ifndef MinMax_H
#define MinMax_H


#include <math.h>

inline double max(double a, double b) {
  return a>b ? a : b;
}

inline double min(double a, double b) {
  return a<b ? a : b;
}

inline int maxi(int a, int b) {
  return a>b ? a : b;
}

inline int mini(int a, int b) {
  return a<b ? a : b;
}

inline int ceili(double a) {
  return int(ceil(a));
}

inline int floori(double a) {
  return int(floor(a));
}

inline int roundi(double a) {
  return int(round(a));
}

#endif
