// pvp.cpp

#include <pvp/pvpSystem.h>
#include <pvp/pvpCamera.h>
#include <base/dbg.h>
int main() {
  pvpSystem sys;
  Dbg() << "pvpSystem initialized. Status report follows.";
  sys.reportStatus();
  
  int ncams = sys.countCameras();
  Dbg() << "Camera count: " << ncams;
  for (int n=0; n<ncams; n++) {
    QString s = sys.getCamName(n);
    Dbg() << "Camera #" << n << " is " << s << ". Status report follows.";
    pvpCamera cam(s);
    cam.reportStatus();
    cam.reportSpeeds();
    Dbg();
  }
  return 0;
}


