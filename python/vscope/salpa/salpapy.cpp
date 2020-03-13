// salpapy.cpp - wrapper around SALPA for python

#include "LocalFit.h"
#include <stdio.h>

#ifdef _WIN32
#define EXPORTED   __declspec(dllexport) 
#else
#define EXPORTED
#endif
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
  EXPORTED LocalFit *salpa_start(raw_t const *data, raw_t *out, uint64_t N,
				 double rail1,
				 double rail2,
				 double thresh,
				 int tau,
				 int t_blankdepeg,
				 int t_ahead,
				 int t_chi2) {
    /* Prepare the filter.
       You should call salpa_partial and/or salpa_forcepeg to process
       the data piecewise, then call salpa_end to free resources.
       Returns 0 if the filter could not be constructed.
     */ 
    if (!_sanity())
      return 0;
    LocalFit *lf = new LocalFit(data, out,
				0, N,
				thresh, tau,
				t_blankdepeg, t_ahead,
				t_chi2);
    lf->setrail(rail1, rail2);
    return lf;
  }

  EXPORTED void salpa_partial(LocalFit *lf, uint64_t t_end) {
    // process up to given time point
    lf->process(t_end);
  }

  EXPORTED void salpa_forcepeg(LocalFit *lf, uint64_t t_from, uint64_t t_to) {
    // processes up to t_to, forcing peg between t_from and t_to.
    lf->forcepeg(t_from, t_to);
  }

  EXPORTED void salpa_end(LocalFit *lf) {
    // free resources
    delete lf;
  }
}
