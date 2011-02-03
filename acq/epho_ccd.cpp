// epho_ccd.cpp

#include "epho_ccd.h"
#include <base/dbg.h>
#include "dutycyclelimit.h"
#include <stdio.h>
#include <base/minmax.h>
#include <xml/enumerator.h>
#include <xml/paramtree.h>

#define PAR_OUTRATE "acqEphys/acqFreq"

EPhO_CCD::EPhO_CCD(CCDTimingDetail const &timing): timing(timing) {
}

void EPhO_CCD::prepare(DigitalData *ddata) const {
  Enumerator *lines = Enumerator::find("DIGILINES");

  int shtrline = lines->lookup("ExcShtr",-1);
  int illumline = lines->lookup("ExcLight",-1);
  int trigccline = lines->lookup("TrigCc");
  int trigoxline = lines->lookup("TrigOx");

  if (shtrline>=0)
    ddata->addLine(shtrline);
  if (illumline>=0)
    ddata->addLine(illumline);
  ddata->addLine(trigccline);
  ddata->addLine(trigoxline);

  uint32_t one = 1;

  uint32_t shtrval = (shtrline>=0) ? (one<<shtrline) : 0;
  uint32_t illumval = (illumline>=0) ? (one<<illumline) : 0;
  uint32_t trigccval = one<<trigccline;
  uint32_t trigoxval = one<<trigoxline;

  int nscans = timing.neededScans();
  
  if (ddata->getNumScans() < nscans)
    throw Exception("EPhO_CCD","Insufficient space in DigitalData","prepare");

  uint32_t *data = ddata->allData();

  for (int n=-timing.preHeatFrames(); n<timing.nframes(); n++) {
    int framestart = timing.startScans() + n*timing.periodScans();
    int trigend = framestart + 10;
    int illumstart = framestart-timing.illumPreScans();
    int illumend = framestart+timing.activeScans()+timing.illumPostScans();
    int shtrstart = framestart-timing.shtrPreScans();
    int shtrend = framestart+timing.activeScans()+timing.shtrPostScans();
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
    if (n==0 || (timing.trigEach() && n>0)) 
      for (int i=framestart; i<trigend; i++)
	data[i] |= trigccval|trigoxval;
  }     
}
