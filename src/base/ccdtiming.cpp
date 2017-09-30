// base/ccdtiming.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

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
