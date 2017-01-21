// ccdconfig.cpp

#include "ccdconfig.h"

CCDConfig::CCDConfig() {
  nframes = 1;
  expose_us = 10000;
  clear_every_frame = false;
  trigmode = CCDTrigMode::Immediate;
  iscont = false;
  portspeed = PS_P1S1;
}

CCDConfig::CCDConfig(CCDConfig const &c) {
  *this = c;
}

CCDConfig &CCDConfig::operator=(CCDConfig const &c) {
  nframes = c.nframes;
  expose_us = c.expose_us;
  clear_every_frame = c.clear_every_frame;
  region = c.region;
  binning = c.binning;
  trigmode = c.trigmode;
  iscont = c.iscont;
  return *this;
}

int CCDConfig::getSerPix() const {
  return (region.smax+1-region.smin) / binning.sbin;
}

int CCDConfig::getParPix() const {
  return (region.pmax+1-region.pmin) / binning.pbin;
}

int CCDConfig::getPixPerFrame() const {
  return getSerPix() * getParPix();
}

int CCDConfig::getTotalPix() const {
  return getPixPerFrame() * nframes;
}

Transform CCDConfig::placement() const {
  Transform t;
  t.scale(binning.sbin, binning.pbin);
  t.translate(region.smin, region.pmin);
  return t;
}
