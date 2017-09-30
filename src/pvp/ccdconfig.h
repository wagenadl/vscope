// pvp/ccdconfig.h - This file is part of VScope.
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

// ccdconfig.h

#ifndef CCDCONFIG_H

#define CCDCONFIG_H

#include <pvp/ccdbinning.h>
#include <pvp/ccdregion.h>
#include <pvp/ccdtrigmode.h>
#include <base/transform.h>
#include <base/enums.h>

class CCDConfig {
public:
  CCDConfig();
  CCDConfig(CCDConfig const &);
  CCDConfig &operator=(CCDConfig const &);
  int getSerPix() const; // "Serial" means "X"
  int getParPix() const; // "Parallel" means "Y"
  int getPixPerFrame() const;
  int getTotalPix() const;
  Transform placement() const;
  /*:F placement
   *:D Returns a transformation matrix that transforms the current image
       region and binning to the full space of the camera.
   *:N Note that this matrix will always have positive scale, even if a flip()
       operation has been applied to the region. (This is natural, since
       the region always maintains smin<=smax and pmin<=pmax.)
  */
      
public:
  int nframes; // undefined for continuous
  int expose_us;
  bool clear_every_frame;
  CCDRegion region;
  CCDBinning binning;
  CCDTrigMode trigmode;
  PORTSPEED portspeed;
  bool iscont;
};

#endif
