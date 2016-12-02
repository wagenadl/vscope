// ccdtiming.cpp

#include "ccdtiming.h"
#include "minmax.h"

CCDTiming::CCDTiming(int fs) {
  reset(fs);
}

CCDTiming &CCDTiming::reset(int fs) {
  scan_us = 1000000 / fs;
  nframes_= 0; 
  start_scans = 0;
  period_scans = 0;
  actv_scans = 0;  
  return *this;
}

CCDTiming &CCDTiming::setFrames(int nfr) {
  nframes_ = nfr;
  return *this;
}

CCDTiming &CCDTiming::setTimingI(int t0_us, int dt_us, int duty_pct) {
  duty_pct_ = duty_pct;
  start_scans = t0_us/scan_us;
  if (duty_pct==100) {
    int per_ms = dt_us/1000;
    period_scans = (per_ms*1000) / scan_us;
    actv_scans = period_scans;
  } else {
    period_scans = dt_us/scan_us;
    int actv_ms = (dt_us*duty_pct/100) / 1000;
    actv_scans = (actv_ms*1000) / scan_us;
  }
  return *this;
}
