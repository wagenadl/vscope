// pvpSystem.cpp

#include <pvp/pvpSystem.h>
#include <stdio.h>
#include <pvp/dwpvcam.h>

bool pvpSystem::inited = false;

pvpSystem::pvpSystem() {
  initialize();
}

void pvpSystem::initialize() /*throw(pvpException)*/ {
  if (inited)
    return;
  
  pl_pvcam_init();
  // pl_exp_init_seq(); // Deprecated in driver vsn 3.1

  inited = true;
}

void pvpSystem::closedown() /*throw(pvpException)*/ {
  if (!inited)
    return;
  
  // pl_exp_uninit_seq(); // Deprecated in driver vsn 3.1

  pl_pvcam_uninit();
  inited = false;
}

int pvpSystem::countCameras() /*throw(pvpException)*/ {
  initialize();
  
  int16 count;
  if (!pl_cam_get_total(&count)) {
    fprintf(stdout, "Could not count cameras\n");
    // throw pvpException("pvpSystem: Could not count cameras");
    return 0;
  }
  return count;
}

QString pvpSystem::getCamName(int n) /*throw(pvpException)*/ {
  initialize();
  
  char buffer[CAM_NAME_LEN];
  int16 camno = n;
  if (!pl_cam_get_name(camno, buffer))
    throw pvpException("pvpSystem: Cannot read camera name");
  return buffer;
}

int pvpSystem::getPVCAMVersion() /*throw(pvpException)*/ {
  initialize();
  
  uns16 version;
  if (!pl_pvcam_get_ver(&version)) {
    fprintf(stdout, "Could not get pvcam version\n");
    // throw pvpException("pvpSystem: Cannot read PVCAM version");
    return -1;
  }
  return version;
}

void pvpSystem::reportStatus() /*throw(pvpException)*/ {
  initialize();
  
  fprintf(stdout, "PVP System status report\n");
  fprintf(stdout, "PVCam version:  0x%04x\n",getPVCAMVersion());
  fprintf(stdout, "Camera count:   %i\n",countCameras());

  int N = countCameras();
  for (int n=0; n<N; n++) {
    QString s = getCamName(n);
    fprintf(stdout, "Camera name #%i: %s\n",n,s.toUtf8().data());
  }
  fprintf(stdout, "End of PVP System status report\n\n");
}
