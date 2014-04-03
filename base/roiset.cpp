// roiset.cpp

#include "roiset.h"
#include <base/dbg.h>
#include <base/xml.h>

ROISet::ROISet(QObject *p): IDKeyAccess(p) {
  lastid = 0;
}

ROISet::~ROISet() {
}

int ROISet::newROI(CamPair const &cam) {
  int id = ++lastid;
  allids.insert(id);
  cams[id] = cam;
  map[id] = ROICoords();
  return id;
}

CamPair const &ROISet::cam(int id) const {
  if (cams.contains(id))
    return *cams.find(id);
  else
    throw Exception("ROISet",
		    QString("No ROI with ID=%1").arg(id),"get");
}
    
ROICoords const &ROISet::get(int id) const {
  if (map.contains(id))
    return *map.find(id);
  else
    throw Exception("ROISet",
		    QString("No ROI with ID=%1").arg(id),"get");
}

ROICoords &ROISet::access(KeyAccess::WriteKey *key) {
  int id = getID(key);
  if (map.contains(id))
    return *map.find(id);
  else
    throw Exception("ROISet",
		    QString("No ROI with ID=%1").arg(id),"access");
}

ROICoords &ROISet::access(KeyAccess::WriteKey *key, int id) {
  verifyAllKey(key);
  if (map.contains(id))
    return *map.find(id);
  else
    throw Exception("ROISet",
		    QString("No ROI with ID=%1").arg(id),"access");
}

bool ROISet::contains(int id) const {
  return allids.contains(id);
}

void ROISet::remove(int id) {
  IDKeyGuard guard(*this, id);
  //Dbg() << "ROISet::remove("<<id<<")";
  if (!allids.contains(id))
    return;
  allids.remove(id);
  cams.remove(id);
  map.remove(id);
}

void ROISet::write(QDomElement dst) const {
  QDomDocument doc = dst.ownerDocument();
  foreach (int id, allids) {
    QDomElement e = doc.createElement("roi");
    dst.appendChild(e);
    e.setAttribute("id",QString("%1").arg(id));
    QString pairid = cams[id].donor;
    if (!cams[id].acceptor.isEmpty())
      pairid += ":" + cams[id].acceptor;
    e.setAttribute("cam", pairid);
    map[id].write(e);
  }
}

void ROISet::read(QDomElement src) {
  AllKeyGuard guard(*this);
  clear();
  bool ok;
  lastid = 0;
  for (QDomElement e=src.firstChildElement("roi");
       !e.isNull(); e=e.nextSiblingElement("roi")) {
    int id = e.attribute("id").toInt(&ok);
    if (!ok)
      throw Exception("ROISet","Cannot read id from xml");
    if (id>=lastid)
      lastid = id;
    allids.insert(id);
    cams[id] = CamPair();
    if (e.hasAttribute("cam")) {
      QStringList c = e.attribute("cam").split(":"); 
      if (c.size()>=1)
	cams[id].donor = c[0];
      if (c.size()>=2) 
	cams[id].acceptor = c[1];
    }
    if (cams[id].donor=="" && cams[id].acceptor=="")
      cams[id] = dfltcam;
    map[id].read(e);
  }
}

void ROISet::setDefaultCamPair(CamPair const &cp) {
  dfltcam = cp;
}

QSet<int> const &ROISet::ids() const {
  return allids;
}

QSet<int> ROISet::idsForCam(CamPair const &cam) const {
  QSet<int> ids;
  foreach (int id, cams.keys()) 
    if (cams[id]==cam)
      ids.insert(id);
  return ids;
}

void ROISet::clear() {
  AllKeyGuard guard(*this);
  map.clear();
  cams.clear();
  allids.clear();
  lastid = 0;
}

void ROISet::save(QString fn) const {
  XML xml(0,"vsdscopeROIs");
  QDomElement roidoc = xml.append("rois");
  write(roidoc);
  xml.write(fn);
}
  
void ROISet::load(QString fn) {
  AllKeyGuard guard(*this);
  XML xml(fn);
  QDomElement roidoc = xml.find("rois");
  read(roidoc);
}
