// acq/epho_ccd.cpp - This file is part of VScope.
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
    int l = lines->lookup("Trigger:" + cam.trigid);
    ddata->addLine(l);
    trigvals[id] = one<<l;
    if (cam.shtrid=="") {
      shtrvals[id] = 0;
    } else {
      try {
        int l = lines->lookup("Shutter:" + cam.shtrid);
        ddata->addLine(l);
        shtrvals[id] = one<<l;
      } catch (Exception const &) {
        // Warn about missing shutter
      }
    }
    if (cam.lampid=="") {
      lampvals[id] = 0;
    } else {
      try {
        int l = lines->lookup("Lamp:" + cam.lampid);
        ddata->addLine(l);
        lampvals[id] = one<<l;
      } catch (Exception const&) {
        // Warn about missing lamp
      }
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
          Warning(0) << "Illumination start delayed (by" << -illumstart << "scans)";
	illumstart=0;
      }
      if (illumend>nscans-2) {
	if (n>=0)
          Warning(0) << "Illumination end shortened (by " << illumend-(nscans-2) << "scans)";
	illumend=nscans-2;
      }
      if (shtrstart<0) {
	if (n>=0)
          Warning(0) << "Shutter open delayed (by " << -shtrstart << "scans)";
	shtrstart=0;
      }
      if (shtrend>nscans-2) {
	if (n>=0)
          Warning(0) << "Shutter close shortened (by " << shtrend-(nscans-2) << "scans)\n";
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
