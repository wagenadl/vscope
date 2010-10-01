// trialsig.h - Signing a trial into an analog channel

#ifndef TRIALSIG_H

#define TRIALSIG_H

#include <base/analogdata.h>

class TrialSig {
public:
  static bool sign(int trialno, AnalogData *adata);
  // returns true iff something was signed, i.e., if there is a
  // channel named "SIGNATURE" in AOCHAN.
};

#endif
