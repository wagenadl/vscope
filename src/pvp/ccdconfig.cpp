// pvp/ccdconfig.cpp - This file is part of VScope.
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
  portspeed = c.portspeed;
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
