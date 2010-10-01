// numbers.cpp

#include <base/numbers.h>
#include <math.h>

Numbers::Numbers() {
  pi = 4*atan(1);
  double x=0;
  nan = x/x;
  inf = 1e10;
  for (int k=0; k<10; k++)
    inf*=inf;
}

bool Numbers::isNaN(double x) {
  return isnan(x);
}

Numbers numbers;
