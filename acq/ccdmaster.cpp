// ccdmaster.cpp

#include "ccdmaster.h"
#include <base/exception.h>
#include <QSet>

ParamTree const *camTreep(ParamTree const *ptree, QString camid) {
  if (!ptree)
    return 0;
  ParamTree const *camtree = &ptree->tree("acqCCD/camera:"+camid);
  if (!camtree)
    return 0;
  QString master = camtree->find("master").toString();
  QSet<ParamTree const *> seen;
  seen.insert(camtree);
  while (master.toLower() != "self") {
    camtree = ptree->treep("acqCCD/camera:"+master);
    if (!camtree)
      return 0;
    master = camtree->find("master").toString();
    if (seen.contains(camtree))
      throw Exception("CCDMaster::camTree", "Recursive master settings");
    seen.insert(camtree);
  }
  return camtree;
}

ParamTree const &camTree(ParamTree const *ptree, QString camid) {
  ParamTree const *camtree = camTreep(ptree, camid);
  if (camtree)
    return *camtree;
  else
    throw Exception("CCDMaster::camTree", "Camera " + camid + " not found");
}



QString immediateCCDMaster(ParamTree const *ptree, QString id) {
  /* Returns the master of camera ID, or "" if self. */
  QString pval = ptree->find("acqCCD/camera:" + id + "/master").toString();
  if (pval.toLower()=="self")
    return "";
  else
    return pval;
}

QString ultimateCCDMaster(ParamTree const *ptree, QString id) {
  /* Returns the (master of the master of the) master of camera ID,
     or "" if self, or "LOOP" if loop detected. */
  QString m = immediateCCDMaster(ptree, id);
  if (m=="")
    return "";
  QSet<QString> seen;
  seen.insert(id);
  while (true) {
    seen.insert(m);
    QString mm = immediateCCDMaster(ptree, m);
    if (mm=="")
      return m;
    if (seen.contains(mm))
      return "LOOP";
    m = mm;
  }
}  
