// ccdtiming.cpp

#include "ccdtiming.h"
#include "minmax.h"

CCDTiming::CCDTiming(double fs_hz) {
  nframes_= 0; 
  t0_ms_ = dt_ms_ = 0;
  setRate(fs_hz);
}

void CCDTiming::setRate(double fs_hz) {
  fs_hz_ = fs_hz;
  recalc();
}

void CCDTiming::recalc() {
  start_scans = floori(t0_ms_*fs_hz_/1000);
  period_scans = floori(dt_ms_*fs_hz_/1000);
  actv_scans = floori(dt_ms_*fs_hz_/1000*duty_pct_/100);
}  

void CCDTiming::setFrames(int nfr) {
  nframes_ = nfr;
}

void CCDTiming::setTiming(double t0_ms, double dt_ms,
			  double duty_pct) {
  t0_ms_ = t0_ms;
  dt_ms_ = dt_ms;
  duty_pct_ = duty_pct;
  recalc();
}
