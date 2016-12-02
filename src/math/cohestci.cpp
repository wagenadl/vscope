// cohestci.cpp

#include "cohestci.h"
#include <base/minmax.h>
#include <base/exception.h>
#include <math/fft.h>
#include <base/numbers.h>
#include <base/dbg.h>

CohEstCI::CohEstCI() {
}

void CohEstCI::compute(rvec const &xx, rvec const &yy,
		       double dt, double df,
		       Tapers const &tpr, double ci_factor,
		       double fstar) {
  int length_in = xx.size();
  if (yy.size()!=length_in)
    throw Exception("CohEstCI","Inputs must be of equal length");

  bool multif = fstar<0;
  
  TaperID tid(length_in,dt,df);
  int K = tid.K;
  int nfft = 1<<ceili(log2(length_in));
  int nhalf = ceili(nfft/2);
  int length_out = multif ? nhalf : 1;

  if (tpr.length!=length_in || tpr.nTapers!=K)
    throw Exception("CohEstCI","Incompatible Tapers given","compute");

  freqbase.resize(length_out);
  double fs = 1/dt;
  if (multif)
    for (int n=0; n<length_out; n++)
      freqbase[n] = n*fs/nfft;
  else
    freqbase[0] = fstar;

  if (pxy.size()!=K) {
    pxx.resize(K);
    pyy.resize(K);
    pxy.resize(K);
  }
  if (sumpxy.size()!=length_out) {
    sumpxx.resize(length_out); 
    sumpyy.resize(length_out); 
    sumpxy.resize(length_out);
  }
  sumpxx.fill(0);
  sumpyy.fill(0);
  sumpxy.fill(Cplx());  
  
  for (int k=0; k<K; k++) {
    rvec const &tp(tpr.tapr(k));
    xtap = tp * xx;
    ytap = tp * yy;

    if (fstar<0) {
      xtap.resize(nfft);
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
      Cplx xt; xt = sum(xtap);
      Cplx yt; yt = sum(ytap);
      xtap.resize(1); xtap[0] = xt;
      ytap.resize(1); ytap[0] = yt;
    }

    pxx[k] = abs2(xtap);
    pyy[k] = abs2(ytap);
    pxy[k] = mulconj(xtap, ytap);

    pxx[k] /= fs;
    pyy[k] /= fs;
    pxy[k] /= fs;

    sumpxx += pxx[k];
    sumpyy += pyy[k];
    sumpxy += pxy[k];
  }

  sqrtsum = mulsqrt(sumpxx, sumpyy);
  coherence = sumpxy/sqrtsum;
  magnitude = abs(coherence);
  phase = ::phase(coherence);

  // caculate the transformed coherence
  mag_xf = algcsqr(magnitude);
  if (phase_tao.size()!=K) {
    phase_tao.resize(K);
    mag_tao_xf.resize(K);
  }
  if (coh_tao_hat_mean.size()!=length_out)
    coh_tao_hat_mean.resize(length_out);
  coh_tao_hat_mean.fill(Cplx());
  for (int k=0; k<K; k++) {
    // calculate the take-away-one spectra
    pxx_tao = sumpxx - pxx[k];
    pyy_tao = sumpyy - pyy[k];
    pxy_tao = sumpxy - pxy[k];
    pxx_tao /= K-1;
    pyy_tao /= K-1;
    pxy_tao /= K-1;

    // calculate the take-away-one coherence
    sqrtsum_tao = mulsqrt(pxx_tao, pyy_tao);
    coh_tao = pxy_tao / sqrtsum_tao;
    mag_tao = abs(coh_tao);
    mag_tao_xf[k] = algcsqr(mag_tao);
    phase_tao[k] = ::phase(coh_tao);
    coh_tao_hat = coh_tao / mag_tao;
    coh_tao_hat_mean += coh_tao_hat;
  }
  coh_tao_hat_mean /= K;

  // calc the magnitude mean and sigma
  mag_tao_xf_avg.resize(length_out); mag_tao_xf_avg.fill(0);
  for (int k=0; k<K; k++)
    mag_tao_xf_avg += mag_tao_xf[k];
  mag_tao_xf_avg /= K;

  mag_xf_sig.resize(length_out); mag_xf_sig.fill(0);
  for (int k=0; k<K; k++) {
    dmag_tao_xf = mag_tao_xf[k] - mag_tao_xf_avg;
    dmag_tao_xf *= dmag_tao_xf; 
    mag_xf_sig += dmag_tao_xf;
  }
  mag_xf_sig *= (K-1.)/K;
  mag_xf_sig = sqrt(mag_xf_sig);

  // calc the phase sigma
  phase_sigma = abs(coh_tao_hat_mean);
  phase_sigma -= 1;
  phase_sigma *= -2*(K-1);
  phase_sigma = sqrt(phase_sigma);

  // calc c.i. bounds
  mag_xf_sig *= ci_factor;
  magnitude_lo = mag_xf - mag_xf_sig;
  magnitude_lo = sqrtlgc(magnitude_lo);
  magnitude_hi = mag_xf + mag_xf_sig;
  magnitude_hi = sqrtlgc(magnitude_hi);

  phase_sigma *= ci_factor;
  phase_lo = phase - phase_sigma;
  phase_hi = phase + phase_sigma;
}

