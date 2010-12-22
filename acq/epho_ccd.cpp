// epho_ccd.cpp

#include "epho_ccd.h"
#include <base/dbg.h>
#include "dutycyclelimit.h"
#include <stdio.h>
#include <base/minmax.h>
#include <xml/enumerator.h>
#include <xml/paramtree.h>

#define PAR_OUTRATE "acqEphys/acqFreq"

EPhO_CCD::EPhO_CCD(ParamTree const *ptree, bool snap):
  ptree(ptree), isSnap(snap), timing_(0) {
  timing_.setRate(ptree->find(PAR_OUTRATE).toDouble());

  bool enableCCD = isSnap ? true : ptree->find("acqCCD/enable").toBool();
  double outrate_hz = ptree->find(PAR_OUTRATE).toDouble();
  double framerate_hz = ptree->find("acqCCD/rate").toDouble();
  double sequence_ms = enableCCD ? ptree->find("acqCCD/dur").toDouble() : 0;

  double preillum_ms = ptree->find("acqCCD/preIllum").toDouble();
  double postillum_ms = isSnap ? 1
    : ptree->find("acqCCD/postIllum").toDouble();
  double preshtr_ms = ptree->find("acqCCD/preShutter").toDouble();
  double postshtr_ms = isSnap ? 1
    : ptree->find("acqCCD/postShutter").toDouble();
  double preheat_ms = ptree->find("acqCCD/preHeat").toDouble();
  if (isSnap) 
    dbg("Post-illum/post-shtr ignored for snapshot. Using 1 ms fixed value.");
  double pre_ms = preillum_ms > preshtr_ms ? preillum_ms : preshtr_ms;
  double post_ms = postillum_ms > postshtr_ms ? postillum_ms : postshtr_ms;

  double delay_ms = isSnap ? pre_ms : ptree->find("acqCCD/delay").toDouble();
  
  double dutyCycle_percent =
    DutyCycle::clip(ptree->find("acqCCD/dutyCycle").toDouble());  
  trigEach = DutyCycle::triggerEach(dutyCycle_percent);
  double frame_ms = 1000 / framerate_hz;
  double trial_ms = isSnap
    ? pre_ms + frame_ms*dutyCycle_percent/100 + post_ms
    : ptree->find("acqEphys/acqTime").toDouble();
  if (delay_ms + sequence_ms + post_ms > trial_ms) {
    sequence_ms = trial_ms - post_ms - delay_ms;
    fprintf(stderr,"Warning: CCD Sequence shortened to fit inside trial.\n");
  }
  int nframes = isSnap ? 1 : floori(sequence_ms/frame_ms+.0001);

  timing_.setFrames(nframes);
  timing_.setTiming(delay_ms, frame_ms, dutyCycle_percent);

  illumprescans = floori(preillum_ms*outrate_hz/1000);
  illumpostscans = floori(postillum_ms*outrate_hz/1000);
  shtrprescans = floori(preshtr_ms*outrate_hz/1000);
  shtrpostscans = floori(postshtr_ms*outrate_hz/1000);
  nscans = floori(trial_ms * outrate_hz/1000);
  preheat_frames = isSnap ? 0 : floori(preheat_ms/frame_ms+.0001);
}

int EPhO_CCD::neededScans() const {
  return nscans;
}

void EPhO_CCD::prepare(DigitalData *ddata) const {
  Enumerator *lines = Enumerator::find("DIGILINES");

  int shtrline = lines->lookup("ExcShtr");
  int illumline = lines->lookup("ExcLight");
  int trigccline = lines->lookup("TrigCc");
  int trigoxline = lines->lookup("TrigOx");

  ddata->addLine(shtrline);
  ddata->addLine(illumline);
  ddata->addLine(trigccline);
  ddata->addLine(trigoxline);

  uint32_t one = 1;

  uint32_t shtrval = one<<shtrline;
  uint32_t illumval = one<<illumline;
  uint32_t trigccval = one<<trigccline;
  uint32_t trigoxval = one<<trigoxline;

  if (ddata->getNumScans() < nscans)
    throw Exception("EPhO_CCD","Insufficient space in DigitalData","prepare");

  uint32_t *data = ddata->allData();
  
  for (int n=-preheat_frames; n<timing_.nframes(); n++) {
    int framestart = timing_.startScans() + n*timing_.periodScans();
    int trigend = framestart + 10;
    int illumstart = framestart-illumprescans;
    int illumend = framestart+timing_.activeScans()+illumpostscans;
    int shtrstart = framestart-shtrprescans;
    int shtrend = framestart+timing_.activeScans()+shtrpostscans;
    if (illumstart<0) {
      if (n>=0)
	fprintf(stderr,
		"Warning: Illumination start delayed (by %i scans)\n",
		-illumstart);
      illumstart=0;
    }
    if (illumend>nscans-2) {
      if (n>=0)
	fprintf(stderr,
		"Warning: Illumination end shortened (by %i scans)\n",
		illumend-(nscans-2));
      illumend=nscans-2;
    }
    if (shtrstart<0) {
      if (n>=0)
	fprintf(stderr,
		"Warning: Shutter open delayed (by %i scans)\n",
		-shtrstart);
       shtrstart=0;
    }
    if (shtrend>nscans-2) {
      if (n>=0)
	fprintf(stderr,
		"Warning: Shutter close shortened (by %i scans)\n",
		shtrend-(nscans-2));
      shtrend=nscans-2;
    }

    for (int i=illumstart; i<illumend; i++)
      data[i] |= illumval;
    if (n>=0)
      for (int i=shtrstart; i<shtrend; i++)
	data[i] |= shtrval;
    if (n==0 || (trigEach && n>0)) 
      for (int i=framestart; i<trigend; i++)
	data[i] |= trigccval|trigoxval;
  }     
}
