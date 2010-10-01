// trialsig.cpp

#include "trialsig.h"

 #define VOLT_HEADER 0.5
#define VOLT_ZERO 0.2
#define VOLT_ONE  0.35
#define NSCANS_HALFPERIOD 5
#define MAXBITS_PER_WORD 8
#define VOLT_END 0.5

#include <xml/enumerator.h>

static double *output(double volt, double *ad, int step) {
  for (int n=0; n<NSCANS_HALFPERIOD; n++) {
    *ad = volt;
    ad+=step;
  }
  for (int n=0; n<NSCANS_HALFPERIOD; n++) {
    *ad = -volt;
    ad+=step;
  }
  return ad;
}
  

bool TrialSig::sign(int trialno, AnalogData *adata) {
  int ch;
  try {
    ch = Enumerator::find("AOCHAN")->lookup("SIGNATURE");
  } catch (Exception const &e) {
    e.report();
    return false;
  }

  if (adata==0 || !adata->contains(ch))
    return false;

  double *dd = adata->channelData(ch);
  int nch = adata->getNumChannels();
  int nsc = adata->getNumScans();
  
  if (nsc < (MAXBITS_PER_WORD+2)*2*NSCANS_HALFPERIOD)
    return false;

  double *ed = dd+nsc*nch;

  dd = output(VOLT_HEADER, dd, nch);
  for (int k=0; k<MAXBITS_PER_WORD; k++) {
    output((trialno&1) ? VOLT_ONE : VOLT_ZERO, dd, nch);
    trialno = trialno>>1;
    if (trialno==0)
      break;
  }
  dd = output(VOLT_END, dd, nch);

  while (dd<ed) {
    *dd=0;
    dd+=nch;
  }
  
  return true;
}
