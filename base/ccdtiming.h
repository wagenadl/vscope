// ccdtiming.h

#ifndef CCDTIMING_H

#define CCDTIMING_H

class CCDTiming {
public:
  CCDTiming(int fs_hz=10000);
  CCDTiming &reset(int fs_hz);
  CCDTiming &setFrames(int nframes);
  CCDTiming &setTiming(double t0_ms, double dt_ms, double duty_pct=100);
  int nframes() const { return nframes_; }
  double t0_ms() const { return t0_ms_; }
  double dt_ms() const { return dt_ms_; }
  double duty_percent() const { return duty_pct_; }
  int fs_hz() const { return fs_hz_; }
  int startScans() const { return start_scans; }
  int periodScans() const { return period_scans; }
  int activeScans() const { return actv_scans; }
private:
  void recalc();
private:
  int nframes_; // Number of frames of CCD data acquisition 
  double t0_ms_; // Start of first CCD frame, in ms
  double dt_ms_; // Time between CCD frames (start-to-start), in ms
  double duty_pct_;
  int fs_hz_; // Sampling frequency for electrophysiology in and out
  int start_scans; // Start of first CCD frame, in scans.
  int period_scans; // Number of scans between CCD frames (start-to-start).
  int actv_scans; // Number of scans CCD acquisition is active for each frame.
};

#endif
