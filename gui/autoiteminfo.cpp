// autoiteminfo.cpp

#include "autoiteminfo.h"

AutoItemInfo::AutoItemInfo(xmlPage const *parent): initialGeom(parent) {
  hasAutoItems = false;
}

void AutoItemInfo::setup(PageBuildGeom const &g, QDomElement e) {
  initialGeom = g;
  doc = e;
  hasAutoItems = true;
}
