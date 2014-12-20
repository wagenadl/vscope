// epho_ccd.cpp

#include "epho_ccd.h"
#include <base/dbg.h>
#include "dutycyclelimit.h"
#include <stdio.h>
#include <base/minmax.h>
#include <xml/enumerator.h>
#include <xml/paramtree.h>
#include <xml/connections.h>
#include <xml/outrate.h>

EPhO_CCD::EPhO_CCD(AllCCDTimingDetail const &timing): timing(timing) {
}

void EPhO_CCD::prepare(DigitalData *ddata) const {
  KeyGuard guard(*ddata);
  
  Enumerator *lines = Enumerator::find("DIGILINES");

  QMap<QString,uint32_t> trigvals; // by camid
  QMap<QString,uint32_t> shtrvals; // by camid, 0 if none
  QMap<QString,uint32_t> lampvals; // by camid, 0 if none
  QStringList camnames = Connections::allCams();
  uint32_t one = 1;
  foreach (QString id, camnames) {
    Connections::CamCon const &cam = Connections::findCam(id);
    int l = lines->lookup("Trigger:" + id);
    ddata->addLine(l);
    trigvals[id] = one<<l;
    if (cam.shtrid=="") {
      shtrvals[id] = 0;
    } else {
      int l = lines->lookup("Shutter:" + cam.shtrid);
      ddata->addLine(l);
      shtrvals[id] = one<<l;
    }
    if (cam.lampid=="") {
      lampvals[id] = 0;
    } else {
      int l = lines->lookup("Lamp:" + cam.lampid);
      ddata->addLine(l);
      lampvals[id] = one<<l;
    }
  }
    
  int nscans = timing.neededScans();
  
  if (ddata->getNumScans() < nscans)
    throw Exception("EPhO_CCD","Insufficient space in DigitalData","prepare");

  uint32_t *data = ddata->allData(guard.key());

  foreach (QString id, camnames) {
    if (!timing.has(id))
      continue; // this happens if we are reconstructing stim for
                // a loaded trial that doesn't have all our connected
                // cameras. Really, of course, this should be rethought,
                // but I think just skipping is OK; the reconstructed stim
                // will not, after all, ever be actually used.
    
    for (int n=-timing[id].preHeatFrames(); n<timing[id].nframes(); n++) {
      int framestart = timing[id].startScans() + n*timing[id].periodScans();
      int trigend = framestart + 10;
      int illumstart = framestart - timing[id].illumPreScans();
      int illumend = framestart + timing[id].activeScans()
	+ timing[id].illumPostScans();
      int shtrstart = framestart - timing[id].shtrPreScans();
      int shtrend = framestart + timing[id].activeScans()
	+ timing[id].shtrPostScans();
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
	data[i] |= lampvals[id];
      if (n>=0)
	for (int i=shtrstart; i<shtrend; i++)
	  data[i] |= shtrvals[id];
      if (n==0 || (timing[id].trigEach() && n>0)) 
	for (int i=framestart; i<trigend; i++)
	  data[i] |= trigvals[id];
    }
  }
}
