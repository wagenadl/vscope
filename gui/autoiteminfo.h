// autoiteminfo.h

#ifndef AUTOITEMINFO_H

#define AUTOITEMINFO_H

#include <gui/pagebuildgeom.h>

class AutoItemInfo {
public:
  AutoItemInfo(xmlPage const *parent);
  void setup(PageBuildGeom const &g, QDomElement e);
public:
  bool hasAutoItems;
  QDomElement doc;
  PageBuildGeom initialGeom;
};

#endif
