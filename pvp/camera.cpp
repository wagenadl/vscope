// camera.cpp

#include "camera.h"
#include "pvpCamera.h"
#include <base/types.h>
#include <base/dbg.h>
#include <base/memalloc.h>

Camera::Camera(QString camname) {
  pvpcam = new pvpCamera(camname);
  id = pvpcam->getSerialNumber();
  expose_ms=0; // i.e. undefined
  npixels_for_seq = 0;
  contBuffer = 0;
  npixels_in_buffer = 0;
}

Camera::~Camera() {
  try {
    if (isRunning()) {
      fprintf(stderr,"Camera: Destruction while running. Danger ahead!\n");
      abort();
    }
  } catch(Exception const &e) {
    fprintf(stderr,"Camera: caught exception in destructor:\n");
    e.report();
  }

  if (contBuffer)
    delete [] contBuffer;
}

QString Camera::getSerialNumber() const {
  return pvpcam->getSerialNumber();
}

void Camera::setConfig(CCDConfig const &cfg0) {
  cfg = cfg0;

  rgn_type rgn = pvpcam->pvpRegion(cfg.region,cfg.binning);
  int trigmode = pvpcam->pvpTrigMode(cfg.trigmode);

  double expores_ms;
  int expotime = pvpcam->pvpExposureTime(cfg.expose_ms, &expores_ms);
  expose_ms = expotime * expores_ms;

  pvpcam->setClearMode(cfg.clear_every_frame
		       ? pvpCamera::ClearMode::PreExposure
		       : pvpCamera::ClearMode::PreSequence);

  npixels_for_seq = cfg.iscont
    ? pvpcam->configContinuous(rgn,trigmode,expotime)
    : pvpcam->configFinite(rgn,trigmode,expotime,cfg.nframes);

  if (cfg.iscont) {
    if (!contBuffer || npixels_in_buffer<npixels_for_seq) {
      if (contBuffer)
	delete [] contBuffer;
      contBuffer = memalloc<uint16_t>(npixels_in_buffer = npixels_for_seq,
				      "Camera");
    }
  }
}

void Camera::startFinite(uint16_t *dest, size_t destsize_pix) {
  if (cfg.iscont)
    throw Exception("Camera","startFinite() is not for continuous acq.");
  if (destsize_pix<npixels_for_seq)
    throw Exception("Camera", "Destination buffer too small");
  if (isRunning())
    throw Exception("Camera", "Already running");
  pvpcam->startFinite(dest);
}

void Camera::startContinuous() {
  if (!cfg.iscont)
    throw Exception("Camera", "startContinuous() is not for finite acq.");
  if (isRunning())
    throw Exception("Camera", "Already running");
  pvpcam->startContinuous(contBuffer, npixels_in_buffer);
}  

void Camera::stopContinuous() {
  if (!isRunning())
    throw Exception("Camera", "Not running");
  if (!cfg.iscont)
    throw Exception("Camera", "stopContinuous() is not for finite acq.");
  pvpcam->stopContinuous();
}

void Camera::abort() {
  pvpcam->abort();
}

#define GETSTATUS (cfg.iscont	    \
    ? pvpcam->getContinuousStatus() \
      : pvpcam->getFiniteStatus())

bool Camera::isRunning() {
  return GETSTATUS == pvpCamera::Acquiring;
}

bool Camera::hasCompleted() {
  switch (GETSTATUS) {
  case pvpCamera::Failed:
    throw Exception("Camera", "Readout failed", "hasCompleted");
  case pvpCamera::Completed:
    return true;
  default:
    return false;
  }
}

size_t Camera::nPixelsSoFar() {
  return cfg.iscont
    ? pvpcam->nPixelsSoFarContinuous(npixels_in_buffer)
    : pvpcam->nPixelsSoFarFinite();
}

bool Camera::haveNewFrame() {
  if (!cfg.iscont)
    throw Exception("Camera", "haveNewFrame() is not for finite acq.");
  int r = pvpcam->haveNewFrame();
  if (r<0)
    throw Exception("Camera", "Readout failed", "haveNewFrame");
  return r>0;
}

uint16_t const *Camera::getLatestFrame() {
  if (!cfg.iscont)
    throw Exception("Camera", "getLatestFrame() is not for finite acq.");
  return pvpcam->getLatestFrame();
}

QStringList Camera::cameraInfo() {
  QStringList res;
  res.append("Ser. no.");
  res.append(pvpcam->getHeadSerNumAlpha());
  res.append("Firmware vsn");
  res.append(QString::number(pvpcam->getCamFwVersion()));
  res.append("Set T");
  res.append(QString("%1 C").arg(pvpcam->getTempSetpoint()/100.0));
  res.append("Actual T");
  res.append(QString("%1 C").arg(pvpcam->getTemp()/100.0));
  return res;
}

QString Camera::getID() const {
  return id;
}

void Camera::setID(QString newid) {
  id = newid;
}
