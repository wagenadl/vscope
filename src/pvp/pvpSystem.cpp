// pvpSystem.cpp

#include <pvp/pvpSystem.h>
#include <stdio.h>
#include <pvp/dwpvcam.h>

bool pvpSystem::inited = false;

pvpSystem::pvpSystem() {
  initialize();
}

void pvpSystem::initialize() throw(pvpException) {
  if (inited)
    return;
  
  pl_pvcam_init();
  pl_exp_init_seq();

  inited = true;
}

void pvpSystem::closedown() throw(pvpException) {
  if (!inited)
    return;
  
  pl_exp_uninit_seq();
  pl_pvcam_uninit();
  inited = false;
}

int pvpSystem::countCameras() throw(pvpException) {
  initialize();
  
  int16 count;
  if (!pl_cam_get_total(&count))
    return 0; // throw pvpException("pvpSystem: Could not count cameras");
  return count;
}

QString pvpSystem::getCamName(int n) throw(pvpException) {
  initialize();
  
  char buffer[CAM_NAME_LEN];
  int16 camno = n;
  if (!pl_cam_get_name(camno, buffer))
    throw pvpException("pvpSystem: Cannot read camera name");
  return buffer;
}

int pvpSystem::getDriverVersion() throw(pvpException) {
  initialize();
  
  uns16 version;
  if (!pl_ddi_get_ver(&version))
    return -1; // throw pvpException("pvpSystem: Cannot read driver version");
  return version;
}

int pvpSystem::getPVCAMVersion() throw(pvpException) {
  initialize();
  
  uns16 version;
  if (!pl_pvcam_get_ver(&version))
    return -1; // throw pvpException("pvpSystem: Cannot read PVCAM version");
  return version;
}

void pvpSystem::reportStatus() throw(pvpException) {
  initialize();
  
  fprintf(stdout, "PVP System status report\n");
  fprintf(stdout, "Driver version: 0x%04x\n",getDriverVersion());
  fprintf(stdout, "PVCam version:  0x%04x\n",getPVCAMVersion());
  fprintf(stdout, "Camera count:   %i\n",countCameras());

  int N = countCameras();
  for (int n=0; n<N; n++) {
    QString s = getCamName(n);
    fprintf(stdout, "Camera name #%i: %s\n",n,s.toUtf8().data());
  }
  fprintf(stdout, "End of PVP System status report\n\n");
}
