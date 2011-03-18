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
	uint16_t val = 2000+100*(nx^2)+50*(ny^2)+val0*100+val1*0;
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
  }
}

CCDAcq::CCDAcq() {
  QStringList cams = Connections::allCams();
  ncams=0;
  foreach (QString id, cams) {
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

bool CCDAcq::prepare(ParamTree const *ptree, CCDTimingDetail const &timing) {
  if (isActive)
    throw Exception("CCDAcq","Cannot prepare while active");
  isActive = false; isGood=false; isDone = false;

  try {
    CCDConfig cfg;
    
    // I am calling ser=x, par=y.
    QRect reg(ptree->find("acqCCD/region").toRect());
    cfg.region = CCDRegion(reg.left(),reg.right(),reg.top(),reg.bottom());
    QRect bin(ptree->find("acqCCD/binning").toRect());
    cfg.binning = CCDBinning(bin.width(),bin.height());
  
    bool trigEach = DutyCycle::triggerEach(timing.duty_percent());
  
    t0_ms = timing.t0_ms();
    dt_ms = timing.dt_ms();
  
    cfg.nframes = timing.nframes();
    cfg.iscont = false;
    cfg.expose_ms = dt_ms*timing.duty_percent()/100;
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
      Dbg() << "CCDAcq: camera "<< k << ": " << camids[k]<<" is " << cameras[k] << "; dest is " << dest[k];

#if CCDACQ_ACQUIRE_EVEN_WITHOUT_CAMERA
      // we are going to generate mock data
#else
      if (!cameras[k])
	ccdcfg[k].nframes=0; // we won't acquire from dummy camera
#endif
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

  for (int k=0; k<ncams; k++)
    dest[k]->checkin(keys[k]);
  keys.clear();
  
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

  keys.clear();
  for (int k=0; k<ncams; k++)
    keys.append(dest[k]->checkout());
  
  try {
    for (int k=0; k<ncams; k++)
      if (cameras[k])
	cameras[k]->startFinite(dest[k]->frameData(keys[k]),
				dest[k]->getTotalPix());
      else if (dest[k])
	generateMockData(dest[k], keys[k], k);
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
  QString npix="";
  foreach (QString id, camids) 
    npix += QString(" %1:%2").arg(id).arg(nPixelsSoFar(id));
  Dbg() << "ccdacq: hasended? npix: " << npix;
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
      for (int k=0; k<ncams; k++)
	dest[k]->checkin(keys[k]);
      keys.clear();
    }
  } catch (Exception const &) {
    fprintf(stderr,"ccdAcq caught exception.\n");
    isActive = false;
    isDone = true;
    isGood = false;
    for (int k=0; k<ncams; k++)
      dest[k]->checkin(keys[k]);
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
  if (!camidx.contains(camid))
    throw Exception("CCDAcq","Unknown camera ID");
  dest[camidx[camid]] = destforid;
}

Transform CCDAcq::placement(QString id) const {
  if (camidx.contains(id)) {
    int idx = camidx[id];
    Transform t = caminfo[idx]->placement;
    return t(ccdcfg[idx].placement());
  } else {
    Dbg() << "CCDAcq::placement: Warning: no info for " << id;
    return Transform();
  }
}
