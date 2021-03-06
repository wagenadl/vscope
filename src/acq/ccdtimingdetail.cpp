// acq/ccdtimingdetail.cpp - This file is part of VScope.
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

// ccdtimingdetail.cpp

#include "ccdtimingdetail.h"
#include <base/dbg.h>
#include "dutycyclelimit.h"
#include <stdio.h>
#include <base/minmax.h>
#include <xml/outrate.h>

CCDTimingDetail::CCDTimingDetail() {
  nscans = 0;
}

void CCDTimingDetail::generalPrep(ParamTree const *ptree,
				  ParamTree const *camtree) {
  reset(ptree->find(PAR_OUTRATE).toInt());
  double outrate_hz = fs_hz();

  bool enableCCD = (is_snap ? true : ptree->find("acqCCD/enable").toBool())
    && camtree->find("enable").toBool();
  double framerate_hz = camtree->find("rate").toDouble();
  double sequence_ms = enableCCD ? camtree->find("dur").toDouble() : 0;
  // Note: sequence_ms is not used for snapshots
  double preillum_ms = camtree->find("preIllum").toDouble();
  double postillum_ms = camtree->find("postIllum").toDouble();
  double preshtr_ms = camtree->find("preShutter").toDouble();
  double postshtr_ms = camtree->find("postShutter").toDouble();
  double preheat_ms = camtree->find("preHeat").toDouble();
  double pre_ms = preillum_ms > preshtr_ms ? preillum_ms : preshtr_ms;
  double post_ms = postillum_ms > postshtr_ms ? postillum_ms : postshtr_ms;

  double delay_ms = is_snap ? pre_ms : camtree->find("delay").toDouble();
  
  double dutyCycle_percent =
    DutyCycle::clip(camtree->find("dutyCycle").toDouble());  
  trig_each = DutyCycle::triggerEach(dutyCycle_percent);
  double frame_ms = 1000 / framerate_hz;
  double trial_ms = is_snap
    ? pre_ms + frame_ms*dutyCycle_percent/100 + post_ms
    : ptree->find("acqEphys/acqTime").toDouble();
  if (!is_snap && delay_ms + sequence_ms + post_ms > trial_ms) {
    Dbg() << trial_ms << post_ms << delay_ms << sequence_ms;
    sequence_ms = trial_ms - post_ms - delay_ms;
    fprintf(stderr,"Warning: CCD Sequence shortened to fit inside trial.\n");
  }
  int nframes = is_snap ? 1 : floori(sequence_ms/frame_ms+.0001);

  setFrames(nframes);
  setTimingI(delay_ms*1e3, frame_ms*1e3, dutyCycle_percent);

  illumprescans = floori(preillum_ms*outrate_hz/1000);
  illumpostscans = floori(postillum_ms*outrate_hz/1000);
  shtrprescans = floori(preshtr_ms*outrate_hz/1000);
  shtrpostscans = floori(postshtr_ms*outrate_hz/1000);
  nscans = floori(trial_ms * outrate_hz/1000);
  preheat_frames = is_snap ? 0 : floori(preheat_ms/frame_ms+.0001);
}

void CCDTimingDetail::prepTrial(ParamTree const *ptree,
				ParamTree const *camtree) {
  is_snap = false;
  generalPrep(ptree, camtree);
}
 
void CCDTimingDetail::prepSnap(ParamTree const *ptree,
				ParamTree const *camtree) {
  is_snap = true;
  generalPrep(ptree, camtree);
}
 
