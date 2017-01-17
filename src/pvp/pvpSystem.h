// pvpSystem.h

#ifndef PVPSYSTEM_H

#define PVPSYSTEM_H

#include <QString>
#include <pvp/pvpException.h>

class pvpSystem {
public:
  pvpSystem();
  static int countCameras() /*throw(pvpException)*/;
  static QString getCamName(int n) /*throw(pvpException)*/;
  static int getPVCAMVersion() /*throw(pvpException)*/;
  static void reportStatus() /*throw(pvpException)*/;
private:
  static void initialize() /*throw(pvpException)*/;
  static void closedown() /*throw(pvpException)*/;
  private:
  static bool inited;
};

#endif
