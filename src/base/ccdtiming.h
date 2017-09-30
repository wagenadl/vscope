// base/ccdtiming.h - This file is part of VScope.
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

// ccdtiming.h

#ifndef CCDTIMING_H

#define CCDTIMING_H

class CCDTiming {
public:
  CCDTiming(int fs_hz=10000);
  CCDTiming &reset(int fs_hz);
  CCDTiming &setFrames(int nframes);
  CCDTiming &setTimingI(int t0_us, int dt_us, int duty_pct=100);
  int nframes() const { return nframes_; }
  int t0_us() const { return start_scans*scan_us; }
  int dt_us() const { return period_scans*scan_us; }
  int active_us() const { return actv_scans*scan_us; }
  int duty_percent() const { return duty_pct_; }
  int fs_hz() const { return 1000000/scan_us; }
  int startScans() const { return start_scans; }
  int periodScans() const { return period_scans; }
  int activeScans() const { return actv_scans; }
private:
  void recalc();
private:
  int nframes_; // Number of frames of CCD data acquisition 
  int duty_pct_;
  int scan_us;
  int start_scans; // Start of first CCD frame, in scans.
  int period_scans; // Number of scans between CCD frames (start-to-start).
  int actv_scans; // Number of scans CCD acquisition is active for each frame.
};

#endif
