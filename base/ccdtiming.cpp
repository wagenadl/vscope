// ccdtiming.cpp

#include "ccdtiming.h"
#include "minmax.h"

CCDTiming::CCDTiming() {
  reset();
}

CCDTiming &CCDTiming::reset() {
  nframes_= 0; 
  t0_ms_ = dt_ms_ = 0;
  setRate(0);
  return *this;
}

CCDTiming &CCDTiming::setRate(double fs_hz) {
  fs_hz_ = fs_hz;
  recalc();
  return *this;
}

void CCDTiming::recalc() {
  start_scans = floori(t0_ms_*fs_hz_/1000);
  period_scans = floori(dt_ms_*fs_hz_/1000);
  actv_scans = floori(dt_ms_*fs_hz_/1000*duty_pct_/100);
}  

CCDTiming &CCDTiming::setFrames(int nfr) {
  nframes_ = nfr;
  return *this;
}

CCDTiming &CCDTiming::setTiming(double t0_ms, double dt_ms, double duty_pct) {
  t0_ms_ = t0_ms;
  dt_ms_ = dt_ms;
  duty_pct_ = duty_pct;
  recalc();
  return *this;
}
