// ccdtiming.h

#ifndef CCDTIMING_H

#define CCDTIMING_H

class CCDTiming {
public:
  explicit CCDTiming(double fs_hz=0);
  void setRate(double fs_hz);
  void setFrames(int nframes);
  void setTiming(double t0_ms, double dt_ms, double duty_pct=100);
  int nframes() const { return nframes_; }
  double t0_ms() const { return t0_ms_; }
  double dt_ms() const { return dt_ms_; }
  double duty_percent() const { return duty_pct_; }
  double fs_hz() const { return fs_hz_; }
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
  double fs_hz_; // Sampling frequency for electrophysiology in and out
  int start_scans; // Start of first CCD frame, in scans.
  int period_scans; // Number of scans between CCD frames (start-to-start).
  int actv_scans; // Number of scans CCD acquisition is active for each frame.
};

#endif
