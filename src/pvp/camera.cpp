// camera.cpp

#include "camera.h"
#include "pvpCamera.h"
#include <base/types.h>
#include <base/dbg.h>
#include <QTime>

Camera::Camera(QString camname) {
  pvpcam = new pvpCamera(camname);
  id = pvpcam->getSerialNumber();
  expose_ms=0; // i.e. undefined
  npixels_for_seq = 0;
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
}

QString Camera::getSerialNumber() const {
  return pvpcam->getSerialNumber();
}

CCDConfig const &Camera::getConfig() const {
  return cfg;
}

void Camera::setConfig(CCDConfig const &cfg0) {
  cfg = cfg0;

  rgn_type rgn = pvpcam->pvpRegion(cfg.region,cfg.binning);
  int trigmode = pvpcam->pvpTrigMode(cfg.trigmode);

  int32 expores_us;
  int32 expotime = pvpcam->pvpExposureTime(cfg.expose_us,
					   &expores_us);
  expose_ms = expotime * (expores_us / 1000.0);

  pvpcam->setClearMode(cfg.clear_every_frame
		       ? pvpCamera::ClearMode::ClearPreExposure
		       : pvpCamera::ClearMode::ClearPreSequence);

  npixels_for_seq = cfg.iscont
    ? pvpcam->configContinuous(rgn,trigmode,expotime)
    : pvpcam->configFinite(rgn,trigmode,expotime,cfg.nframes);

  if (cfg.iscont) {
    if (contBuffer.size()<int(npixels_for_seq))
      contBuffer.resize(npixels_for_seq);
  }
  Dbg() << "camera::setconfig npix=" << npixels_for_seq << " expotime=" << expotime << ":" << expores_us << ":" << expose_ms<< " nframes="<< cfg.nframes;
}

void Camera::startFinite(uint16_t *dest, size_t destsize_pix) {
    Dbg() << "startfinite";
  if (cfg.iscont)
    throw Exception("Camera","startFinite() is not for continuous acq.");
  if (destsize_pix<npixels_for_seq)
    throw Exception("Camera", "Destination buffer too small");
  /* For some reason, isRunning() causes a crash in PVCam 3.1+. So we simply don't check anymore. */
  //  if (isRunning())
  //    throw Exception("Camera", "Already running");
  pvpcam->startFinite(dest);
  Dbg() << "startfinite ok";
}

void Camera::startContinuous() {
  if (!cfg.iscont)
    throw Exception("Camera", "startContinuous() is not for finite acq.");
  if (isRunning())
    throw Exception("Camera", "Already running");
  pvpcam->startContinuous(contBuffer.data(), contBuffer.size());
}  

void Camera::stopContinuous() {
  //if (!isRunning())
  //  throw Exception("Camera", "Not running");
  if (!cfg.iscont)
    throw Exception("Camera", "stopContinuous() is not for finite acq.");
  pvpcam->stopContinuous();
  // wait for it to actually stop:
  QTime timeout = QTime::currentTime().addSecs(2); // timeout in two sec
  while (isRunning()) {
    if (QTime::currentTime()>=timeout)
      throw Exception("Camera", "Timeout while stopping acquisition");
  }
}

void Camera::abort() {
  pvpcam->abort();
}

#define GETSTATUS (cfg.iscont	    \
    ? pvpcam->getContinuousStatus() \
      : pvpcam->getFiniteStatus())

bool Camera::isRunning() {
    Dbg() << "camera::isrunning?";
  return GETSTATUS == pvpCamera::Acquiring;
}

bool Camera::hasCompleted() {
    Dbg() << "Camera::hascompleted?";
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
    ? pvpcam->nPixelsSoFarContinuous(contBuffer.size())
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
  res.append(pvpcam->getSerialNumber());
  res.append("Firmware vsn");
  int v = pvpcam->getCamFwVersion();
  res.append(QString("%1.%2.%3")
             .arg((v>>8)&0xff).arg((v>>4)&0x0f).arg(v&0x0f));
  res.append("Driver vsn");
  res.append(pvpcam->getDriverVersion());
  res.append("Set T");
  res.append(QString("%1 C").arg(pvpcam->getTempSetpoint()/100.0));
  res.append("Actual T");
  res.append(QString("%1 C").arg(pvpcam->getTemp()/100.0));
  return res;
}

void Camera::fullReport() {
    Dbg() << "Camera report for " << id;
  // pvpcam->reportStatus();
  pvpcam->reportSpeeds();
}

QString Camera::getID() const {
  return id;
}

void Camera::setID(QString newid) {
  id = newid;
}
