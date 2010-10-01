// ccdconfig.h

#ifndef CCDCONFIG_H

#define CCDCONFIG_H

#include <pvp/ccdbinning.h>
#include <pvp/ccdregion.h>
#include <pvp/ccdtrigmode.h>

class CCDConfig {
public:
  CCDConfig();
  CCDConfig(CCDConfig const &);
  CCDConfig &operator=(CCDConfig const &);
  int getSerPix() const; // "Serial" means "X"
  int getParPix() const; // "Parallel" means "Y"
  int getPixPerFrame() const;
  int getTotalPix() const;
public:
  int nframes; // undefined for continuous
  double expose_ms;
  bool clear_every_frame;
  CCDRegion region;
  CCDBinning binning;
  CCDTrigMode trigmode;
  bool iscont;
};

#endif
