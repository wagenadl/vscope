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
#include <xml/enumerator.h>

CCDAcq::CCDAcq() {
  Enumerator *camenum = Enumerator::find("CAMERAS");
  ncams=0;
  for (int k=camenum->getSmallestValue();
       k<=camenum->getLargestValue(); k++) {
    QString id = camenum->reverseLookup(k);
    camids.append(id);
    caminfo.append(Connections::findpCam(id));
    camidx[id]=ncams;
    Dbg() << "CCDAcq: found camera #"<<ncams<<": " << id;
    ncams++;
  }
  ccdcfg.resize(ncams);
  dest.fill(0, ncams);
  cameras.fill(0, ncams);
  Dbg() << "CCDAcq: camera count: "<<ncams;

  isActive=false;
  isGood=false;
  isDone=false;
}

bool CCDAcq::prepare(ParamTree *ptree, bool snap) {
  if (isActive)
    throw Exception("CCDAcq","Cannot prepare while active");
  isActive = false; isGood=false; isDone = false;

  try {
    CCDConfig cfg;
    
    // I am calling ser=x, par=y. This may not be the right thing.
    QRect reg(ptree->find("acqCCD/region").toRect());
    cfg.region = CCDRegion(reg.left(),reg.right(),reg.top(),reg.bottom());
    QRect bin(ptree->find("acqCCD/binning").toRect());
    cfg.binning = CCDBinning(bin.width(),bin.height());
  
    double rate_hz = ptree->find("acqCCD/rate").toDouble();
    double frame_ms = 1000 / rate_hz;
    double dur_ms = ptree->find("acqCCD/dur").toDouble();
    double delay_ms = ptree->find("acqCCD/delay").toDouble();
    double dutyCycle_Percent = ptree->find("acqCCD/dutyCycle").toDouble();
    bool trigEach = dutyCycle_TriggerEach(dutyCycle_Percent);
    dutyCycle_Percent = dutyCycle_Clip(dutyCycle_Percent);
  
    t0_ms = delay_ms;
    dt_ms = 1e3/rate_hz;
  
    cfg.nframes = snap ? 1 : int(dur_ms*rate_hz/1000);
    cfg.iscont = false;
    cfg.expose_ms = frame_ms*dutyCycle_Percent/100;
    cfg.trigmode = trigEach ? CCDTrigMode::EachFrame : CCDTrigMode::FirstFrame;
    cfg.clear_every_frame = false; // trigEach ? true : false;
  
    for (int k=0; k<ncams; k++) {
      ccdcfg[k] = cfg;
      if (caminfo[k]->flipx)
        ccdcfg[k].region = cfg.region.flipSerial(caminfo[k]->xpix);
      if (caminfo[k]->flipy)
        ccdcfg[k].region = cfg.region.flipParallel(caminfo[k]->ypix);
    }
    
    for (int k=0; k<ncams; k++) {
      cameras[k] = CamPool::findp(camids[k]);
      // actually, we should check whether this camera is enabled in the ptree.
      Dbg() << "CCDAcq: camera "<< k << ": " << camids[k]<<" is " << cameras[k];
    }
      
    for (int k=0; k<ncams; k++)
      if (dest[k])
	dest[k]->reshape(ccdcfg[k].getSerPix(),ccdcfg[k].getParPix(),
			 ccdcfg[k].nframes);

    for (int k=0; k<ncams; k++) 
      if (dest[k])
	dest[k]->setTimeBase(t0_ms,dt_ms);

    for (int k=0; k<ncams; k++)
      if (cameras[k])
        cameras[k]->setConfig(ccdcfg[k]);
  
    for (int k=0; k<ncams; k++)
      if (!cameras[k])
	return false;

    return true;
  
  } catch (Exception const &) {
    dbg("CCDAcq: exception in prepare");
    return false;
  }
}

CCDAcq::~CCDAcq() {
}

void CCDAcq::abort() {
  for (int k=0; k<ncams; k++)
    if (cameras[k])
      cameras[k]->abort();
  
  isActive=false;
  isGood=false;
  isDone=true;
}

void CCDAcq::start() {
  if (isActive)
    throw Exception("CCDAcq","Cannot start while active");
  isGood=false; isDone = false;

  for (int k=0; k<ncams; k++)
    if (!dest[k] && cameras[k])
      throw Exception("CCDAcq","Cannot start without destination buffers");

    for (int k=0; k<ncams; k++)
      if (dest[k])
	dest[k]->reshape(ccdcfg[k].getSerPix(),ccdcfg[k].getParPix(),
			 ccdcfg[k].nframes);

    for (int k=0; k<ncams; k++) 
      if (dest[k])
	dest[k]->setTimeBase(t0_ms,dt_ms);

  try {
    for (int k=0; k<ncams; k++)
      if (cameras[k])
	cameras[k]->startFinite(dest[k]->frameData(),
				dest[k]->getTotalPix());
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

int CCDAcq::nPixelsSoFar(QString camid) {
  if (!camidx.contains(camid))
    return 0;
  int k = camidx[camid];
  if (cameras[k])
    return cameras[k]->nPixelsSoFar();
  else
    return 0;
}

bool CCDAcq::hasEnded() {
  dbg("ccdacq: hasended? npix: %8i %8i",nPixelsSoFar(camids[0]),nPixelsSoFar(camids[1]));
  dbg("ccdacq::hasended actv=%i done=%i good=%i",isActive,isDone,isGood);
  if (!isActive)
    return isDone;

  try {
    bool anyIncomplete = false;
    for (int k=0; k<ncams; k++)
      if (cameras[k] && !cameras[k]->hasCompleted())
	anyIncomplete = true;
    if (!anyIncomplete) {
      isActive = false;
      isDone = true;
      isGood = true;
    }
  } catch (Exception const &) {
    fprintf(stderr,"ccdAcq caught exception.\n");
    isActive = false;
    isDone = true;
    isGood = false;
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
  if (!camidx.contains(camid))
    throw Exception("CCDAcq","Unknown camera ID");
  dest[camidx[camid]] = destforid;
}

  
