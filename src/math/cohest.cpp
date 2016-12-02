// cohest.cpp

#include "cohest.h"
#include <base/minmax.h>
#include <base/exception.h>
#include <base/numbers.h>
#include <math/fft.h>
#include <base/dbg.h>

CohEst::CohEst() {
}

void CohEst::compute(rvec const &xx, rvec const &yy,
		     double dt, double df, Tapers const &tpr,
		     double fstar) {
  int length_in = xx.size();
  if (yy.size()!=length_in)
    throw Exception("CohEst","Inputs must be of equal length");
  
  TaperID tid(length_in,dt,df);
  int K = tid.K;
  int nfft = 1<<ceili(log2(length_in));
  int nhalf = ceili(nfft/2);
  int length_out = fstar>=0 ? 1 : nhalf;

  if (tpr.length!=length_in || tpr.nTapers!=K)
    throw Exception("CohEst","Incompatible Tapers given","compute");

  freqbase.resize(length_out);
  double fs = 1/dt;
  if (fstar<0)
    for (int n=0; n<length_out; n++)
      freqbase[n] = n*fs/nfft;
  else
    freqbase[0] = fstar;

  if (sumpxy.size()!=length_out) {
    sumpxx.resize(length_out);
    sumpyy.resize(length_out);
    sumpxy.resize(length_out);
  }
  sumpxx.fill(0);
  sumpyy.fill(0);  
  sumpxy.fill(Cplx(0,0));
  
  for (int k=0; k<K; k++) {
    xtap = tpr.taper(k) * xx;
    ytap = tpr.taper(k) * yy;

    if (fstar<0) {
      xtap.resize(nfft); // zero-pad
      ytap.resize(nfft);
      FFT_insitu(xtap);
      FFT_insitu(ytap);
      xtap.resize(nhalf);
      ytap.resize(nhalf);
    } else {
      cvec w; w.resize(length_in);
      double phistar = fstar/fs;
      for (int n=0; n<length_in; n++) {
	double phi = -2*Numbers::pi()*phistar*n;
	w[n].re = cos(phi);
	w[n].im = sin(phi);
      }
      xtap *= w;
      ytap *= w;
      xtap[0] = sum(xtap);
      ytap[0] = sum(ytap);
      xtap.resize(1); 
      ytap.resize(1); 
    }      

    pxx = abs2(xtap);
    pxx /= fs;
    pyy = abs2(ytap);
    pyy /= fs;
    pxy = mulconj(xtap, ytap);
    pxy /= fs;

    sumpxx += pxx;
    sumpyy += pyy;
    sumpxy += pxy;
  }

  sumpyy = mulsqrt(sumpxx, sumpyy);
  coherence = sumpxy / sumpyy;
  magnitude = abs(coherence);
  phase = ::phase(coherence);
}
