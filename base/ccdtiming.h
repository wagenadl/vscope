// ccdtiming.h

#ifndef CCDTIMING_H

#define CCDTIMING_H

class CCDTiming {
public:
  CCDTiming() { nframes=0; t0_ms=0; dt_ms=0; fs_hz=0; }
public:
  int nframes; // for ccd
  double t0_ms; // for ccd
  double dt_ms; // for ccd
  double fs_hz; // for ephys
};

#endif
