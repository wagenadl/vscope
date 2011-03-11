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
    Dbg() << "Camera #" << n;
  }
  return 0;
}


