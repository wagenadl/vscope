// psdest.cpp

#include "psdest.h"
#include <base/minmax.h>
#include <base/exception.h>
#include <math/fft.h>

PSDEst::PSDEst() {
}

PSDEst::~PSDEst() {
}

void PSDEst::compute(rvec const &xx,
		     double dt, double df,
		     Tapers const &tpr) {
  int length_in = xx.size();
  TaperID tid(length_in,dt,df);
  int K = tid.K;
  int nfft = 1<<ceili(log2(length_in));
  int nhalf = nfft/2;
  int length_out = nhalf+1;
  
  if (tpr.length!=length_in || tpr.nTapers!=K)
    throw Exception("PSDEst","Incompatible Tapers given","compute");

  freqbase.resize(length_out);
  psd.resize(length_out);
  psds.resize(K);
  for (int k=0; k<K; k++)
    psds[k].resize(length_out);

  double fs = 1/dt;

  for (int n=0; n<length_out; n++)
    freqbase[n] = n*fs/nfft;

  for (int k=0; k<K; k++) {
    xtap = tpr.taper(k) * xx;
    xtap.resize(nfft);

    FFT_insitu(xtap);

    double *psdk = psds[k].data();
    
    psdk[0] = abs2(xtap[0]) / fs;
    for (int n=1; n<nhalf; n++)
      psdk[n] = (abs2(xtap[n]) + abs2(xtap[nfft-n]))/fs;

    /* Because I know that xx is real, I could equivalently have written:
         psdk[n] = 2*abs2(xtap[n])/fs;
       After I test things, I'll make that change: it should be a little
       faster.
    */
    
    psdk[nhalf] = abs2(xtap[nhalf]) / fs;
  }

  psd.fill(0);
  for (int k=0; k<K; k++)
    psd += psds[k];
  psd /= K;
}
