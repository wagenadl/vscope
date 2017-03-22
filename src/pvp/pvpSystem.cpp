// pvp/pvpSystem.cpp - This file is part of VScope.
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

// pvpSystem.cpp

#include <pvp/pvpSystem.h>
#include <stdio.h>
#include <pvp/dwpvcam.h>
#include <base/dbg.h>

bool pvpSystem::inited = false;

pvpSystem::pvpSystem() {
  initialize();
}

void pvpSystem::initialize() /*throw(pvpException)*/ {
  if (inited)
    return;
  
  if (pl_pvcam_init() != PV_OK)
    Warning(false) << "Could not initialize pvcam";
  // pl_exp_init_seq(); // Deprecated in driver vsn 3.1
  Dbg() << "pvpSystem::initialize";

  inited = true;
}

void pvpSystem::closedown() /*throw(pvpException)*/ {
  if (!inited)
    return;
  
  // pl_exp_uninit_seq(); // Deprecated in driver vsn 3.1
  Dbg() << "pvpSystem::closedown";

  pl_pvcam_uninit();
  inited = false;
}

int pvpSystem::countCameras() /*throw(pvpException)*/ {
  initialize();
  
  int16 count;
  if (!pl_cam_get_total(&count)) {
    Warning(false) << "Could not count cameras";
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
