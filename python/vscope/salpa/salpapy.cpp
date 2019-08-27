// salpapy.cpp - wrapper around SALPA for python

#include "LocalFit.h"
#include <stdio.h>

bool _sanity() {
  timeref_t TEST = INFTY;
  TEST++;
  if (TEST!=0) {
    printf("Warning! INFTY does not work on your platform. Please contact\nthe author, Daniel Wagenaar.\nFor contact information please visit http://www.danielwagenaar.net.\n");
    return false;
  }
  if (sizeof(timeref_t)!=8) {
    printf("Warning! I could not create an 8-bit integer type. Please contact\nthe author, Daniel Wagenaar.\nFor contact information please visit http://www.danielwagenaar.net.\n");
    return false;
  }
  return true;
}


extern "C" {
  void salpapy(double const *data, double *out, int N,
               double rail1,
               double rail2,
               double thresh,
               int tau,
               int t_blankdepeg,
               int t_ahead,
               int t_chi2) {
    if (!_sanity())
      return;
    LocalFit lf(data, out,
                0, N,
                thresh, tau,
                t_blankdepeg, t_ahead,
                t_chi2);
    lf.setrail(rail1, rail2);
    lf.process(N);
  }
}
