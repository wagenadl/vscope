// ccdtimingdetail.cpp

#include "ccdtimingdetail.h"
#include <base/dbg.h>
#include "dutycyclelimit.h"
#include <stdio.h>
#include <base/minmax.h>

#define PAR_OUTRATE "acqEphys/acqFreq"

CCDTimingDetail::CCDTimingDetail(ParamTree const *ptree, bool snap) {
  setRate(ptree->find(PAR_OUTRATE).toDouble());
  is_snap = snap;
  double outrate_hz = fs_hz();

  bool enableCCD = is_snap ? true : ptree->find("acqCCD/enable").toBool();
  double framerate_hz = ptree->find("acqCCD/rate").toDouble();
  double sequence_ms = enableCCD ? ptree->find("acqCCD/dur").toDouble() : 0;

  double preillum_ms = ptree->find("acqCCD/preIllum").toDouble();
  double postillum_ms = is_snap ? 1
    : ptree->find("acqCCD/postIllum").toDouble();
  double preshtr_ms = ptree->find("acqCCD/preShutter").toDouble();
  double postshtr_ms = is_snap ? 1
    : ptree->find("acqCCD/postShutter").toDouble();
  double preheat_ms = ptree->find("acqCCD/preHeat").toDouble();
  if (is_snap) 
    dbg("Post-illum/post-shtr ignored for snapshot. Using 1 ms fixed value.");
  double pre_ms = preillum_ms > preshtr_ms ? preillum_ms : preshtr_ms;
  double post_ms = postillum_ms > postshtr_ms ? postillum_ms : postshtr_ms;

  double delay_ms = is_snap ? pre_ms : ptree->find("acqCCD/delay").toDouble();
  
  double dutyCycle_percent =
    DutyCycle::clip(ptree->find("acqCCD/dutyCycle").toDouble());  
  trig_each = DutyCycle::triggerEach(dutyCycle_percent);
  double frame_ms = 1000 / framerate_hz;
  double trial_ms = is_snap
    ? pre_ms + frame_ms*dutyCycle_percent/100 + post_ms
    : ptree->find("acqEphys/acqTime").toDouble();
  if (delay_ms + sequence_ms + post_ms > trial_ms) {
    sequence_ms = trial_ms - post_ms - delay_ms;
    fprintf(stderr,"Warning: CCD Sequence shortened to fit inside trial.\n");
  }
  int nframes = is_snap ? 1 : floori(sequence_ms/frame_ms+.0001);

  setFrames(nframes);
  setTiming(delay_ms, frame_ms, dutyCycle_percent);

  illumprescans = floori(preillum_ms*outrate_hz/1000);
  illumpostscans = floori(postillum_ms*outrate_hz/1000);
  shtrprescans = floori(preshtr_ms*outrate_hz/1000);
  shtrpostscans = floori(postshtr_ms*outrate_hz/1000);
  nscans = floori(trial_ms * outrate_hz/1000);
  preheat_frames = is_snap ? 0 : floori(preheat_ms/frame_ms+.0001);
}
