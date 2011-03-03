// ccdconfig.h

#ifndef CCDCONFIG_H

#define CCDCONFIG_H

#include <pvp/ccdbinning.h>
#include <pvp/ccdregion.h>
#include <pvp/ccdtrigmode.h>
#include <base/transform.h>

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
  double expose_ms;
  bool clear_every_frame;
  CCDRegion region;
  CCDBinning binning;
  CCDTrigMode trigmode;
  bool iscont;
};

#endif
