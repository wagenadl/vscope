// acq/ccdacq.cpp - This file is part of VScope.
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


// ccdacq.cpp

#include "ccdacq.h"
#include <xml/paramtree.h>
#include <base/ccddata.h>
#include <pvp/camera.h>
#include <pvp/ccdconfig.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <pvp/campool.h>
#include "dutycyclelimit.h"
#include <math.h>
#include <acq/ccdmaster.h>

static void generateMockData(CCDData *dest, CCDData::WriteKey *key, int seed) {
  int wid = dest->getSerPix();
  int hei = dest->getParPix();
  int nfr = dest->getNFrames();
  double frmdt_s = dest->getDTms()/1e3;
  double f0_hz = 1;
  int NX = 4; // number of patches
  int NY = 4;
  uint16_t *data = dest->frameData(key);
  for (int n=0; n<nfr; n++) {
    int x0 = 0;
    for (int nx=0; nx<NX; nx++) {
      int x1 = wid*(nx+1)/NX;
      int y0 = 0;
      for (int ny=0; ny<NY; ny++) {
	int y1 = hei*(ny+1)/NY;
	uint16_t *patch = data + n*(wid*hei) + y0*wid + x0;
	double val0 = cos(2*3.14159265*f0_hz*(1+(nx&1)+(ny&1))*n*frmdt_s
			 +nx*2345+ny*278+37*seed);
	double val1 = 0;
	for (int k=0; k<10; k++)
	  val1 += double(rand())/RAND_MAX - 0.5;
	uint16_t val = uint16_t(2000+100*(nx^2)+50*(ny^2)+val0*100+val1*0);
	int w = x1-x0;
	int h = y1-y0;
	for (int y=0; y<h; y++) {
	  uint16_t *ptr = patch+y*wid;
	  for (int x=0; x<w; x++)
	    *ptr++ = val;
	}
	y0 = y1;
      }
      x0 = x1;
    }
    x0=30;
    int x1=70;
    int y0=50;
    int y1=90;
    if (x1>wid)
      x1=wid;
    if (y1>hei)
      y1=hei;
    double val0 = n%5;
    uint16_t val = uint16_t(2500+100*val0);
    for (int y=y0; y<y1; y++) {
      uint16_t *patch = data + n*(wid*hei) + y*wid;
      for (int x=x0; x<x1; x++)
	patch[x] = val;
    }
  }
}

CCDAcq::CCDAcq() {
  camids = Connections::allCams();
  foreach (QString id, camids) {
    caminfo[id] = Connections::findpCam(id);
    ccdcfg[id] = CCDConfig();
    dest[id] = 0;
    cameras[id] = 0;
  }

  isActive=false;
  isGood=false;
  isDone=false;
}

bool CCDAcq::prepare(ParamTree const *ptree, 
		     AllCCDTimingDetail const &timing) {
  if (isActive)
    throw Exception("CCDAcq","Cannot prepare while active");
  isActive = false; isGood=false; isDone = false;

  try {
    
    // I am calling ser=x, par=y.

    QSet<QString> enabledCams;
    
    foreach (QString id, camids) {
      bool ena = ptree->find("acqCCD/camera:" + id + "/enable").toBool();
      if (ena) {
	enabledCams.insert(id);
	ParamTree const *rootTree = &::camTree(ptree, id); // _now_ find master
	CCDConfig cfg;
	cfg.iscont = false;
	QRect reg(rootTree->find("region").toRect());
	cfg.region = CCDRegion(reg.left(),reg.right(),reg.top(),reg.bottom());
	QRect bin(rootTree->find("binning").toRect());
	cfg.binning = CCDBinning(bin.width(),bin.height());
        cfg.portspeed = PORTSPEED(rootTree->find("portspeed").toInt());
        Dbg() << "ccdacq:portspeed" << cfg.portspeed;
	CCDTimingDetail const &detail = timing[id];
	bool trigEach = DutyCycle::triggerEach(detail.duty_percent());
	
	t0_us = detail.t0_us();
	dt_us = detail.dt_us();
	frdur_us = detail.active_us();
	
	cfg.nframes = detail.nframes();
        cfg.expose_us = frdur_us;
	cfg.trigmode = trigEach ? CCDTrigMode::EachFrame
          : CCDTrigMode::FirstFrame;
	cfg.clear_every_frame = false; // trigEach ? true : false;
	
	cfg.region = CCDRegion(caminfo[id]->placement.inverse()(reg));
	ccdcfg[id] = cfg;
      } else {
	ccdcfg[id] = CCDConfig();
      }
    }
    
    foreach (QString id, camids) {
      if (enabledCams.contains(id))
	cameras[id] = CamPool::findp(id);
      else
	cameras[id] = 0;

#if CCDACQ_ACQUIRE_EVEN_WITHOUT_CAMERA
      // we are going to generate mock data
#else
      if (!cameras[id])
	ccdcfg[id].nframes = 0; // we won't acquire from dummy camera
#endif
    }

    foreach (QString id, camids) 
      if (dest[id])
	dest[id]->reshape(ccdcfg[id].getSerPix(),ccdcfg[id].getParPix(),
			 ccdcfg[id].nframes);

    foreach (QString id, camids) 
      if (dest[id])
	dest[id]->setTimeBase(t0_us/1e3, dt_us/1e3, frdur_us/1e3);

    foreach (QString id, camids)
      if (cameras[id])
        cameras[id]->setConfig(ccdcfg[id]);
  
    foreach (QString id, camids)
      if (enabledCams.contains(id) && !cameras[id])
	return false;

    return true;
  
  } catch (Exception const &) {
    dbg("CCDAcq: exception in prepare");
    return false;
  }
}

CCDAcq::~CCDAcq() {
}

void CCDAcq::finish() {
  foreach (QString id, camids)
    if (cameras[id])
      cameras[id]->finishFinite();
}

void CCDAcq::abort() {
  foreach (QString id, camids)
    if (cameras[id])
      cameras[id]->abort();

  foreach (QString id, camids)
    if (dest[id])
      dest[id]->checkin(keys[id]);
  keys.clear();
  
  isActive=false;
  isGood=false;
  isDone=true;
}

void CCDAcq::start() {
  if (isActive)
    throw Exception("CCDAcq","Cannot start while active");
  isGood=false; isDone = false;

  foreach (QString id, camids)
    if (cameras[id] && !dest[id])
      throw Exception("CCDAcq","Cannot start without destination buffers");

  foreach (QString id, camids)
    if (dest[id])
      dest[id]->reshape(ccdcfg[id].getSerPix(),ccdcfg[id].getParPix(),
		       ccdcfg[id].nframes);
  
  foreach (QString id, camids) 
    if (dest[id])
      dest[id]->setTimeBase(t0_us/1e3, dt_us/1e3, frdur_us/1e3);

  Q_ASSERT(keys.isEmpty());
  foreach (QString id, camids)
    if (dest[id])
      keys[id] = dest[id]->checkout();
  
  try {
    int k = 0;
    foreach (QString id, camids)
      if (cameras[id])
	cameras[id]->startFinite(dest[id]->frameData(keys[id]),
				dest[id]->getTotalPix());
      else if (dest[id])
	generateMockData(dest[id], keys[id], k++);
    isActive = true;
  } catch (Exception const &) {
    dbg("CCDAcq: exception during start");
    try {
      abort();
    } catch(Exception const &) {
      dbg("CCDAcq: exception during aborted start");
    }
    throw;
  }
}

size_t CCDAcq::nPixelsSoFar(QString id) {
  if (cameras.contains(id) && cameras[id])
    return cameras[id]->nPixelsSoFar();
  else
    return 0;
}

bool CCDAcq::hasEnded() {
  QString npix="";
  foreach (QString id, camids) 
    npix += QString(" %1:%2").arg(id).arg(nPixelsSoFar(id));
  Dbg() << "ccdacq: hasended? npix=" << npix 
	<< " actv="<<isActive
	<< " done="<<isDone
	<< " good="<<isGood;
  if (!isActive)
    return isDone;

  try {
    bool anyIncomplete = false;
    foreach (QString id, camids)
      if (cameras[id] && !cameras[id]->hasCompleted())
	anyIncomplete = true;
    if (!anyIncomplete) {
      isActive = false;
      isDone = true;
      isGood = true;
      foreach (QString id, camids)
	dest[id]->checkin(keys[id]);
      keys.clear();
    }
  } catch (Exception const &) {
    fprintf(stderr,"ccdAcq caught exception.\n");
    isActive = false;
    isDone = true;
    isGood = false;
    foreach (QString id, camids)
      dest[id]->checkin(keys[id]);
    keys.clear();
  }

  return isActive ? false : isDone;
}

bool CCDAcq::wasSuccessful() {
  if (hasEnded())
    return isGood;
  else
    return false;
}

void CCDAcq::setDest(QString camid, CCDData *destforid) {
  dest[camid] = destforid;
}

Transform CCDAcq::placement(QString id) const {
  if (caminfo.contains(id)) {
    Transform t = caminfo[id]->placement;
    return t(ccdcfg[id].placement());
  } else {
    Dbg() << "CCDAcq::placement: Warning: no info for " << id;
    return Transform();
  }
}
