// roiset.cpp

#include "roiset.h"
#include <base/dbg.h>
#include <base/xml.h>

ROISet::ROISet(QObject *p): QObject(p) {
  lastid = 0;
}

ROISet::~ROISet() {
}

int ROISet::newROI(CamPair const &cam) {
  int id = ++lastid;
  allids.insert(id);
  cams[id] = cam;
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

ROICoords &ROISet::checkout(int id) {
  if (map.contains(id))
    return map[id];
  else
    throw Exception("ROISet",
		    QString("No ROI with ID=%1").arg(id),"get");
}

void ROISet::checkin(int id) {
  emit changed(id);
}

void ROISet::cancel(int) {
}

bool ROISet::contains(int id) const {
  return allids.contains(id);
}

void ROISet::remove(int id) {
  if (!allids.contains(id))
    return;
  allids.remove(id);
  cams.remove(id);
  map.remove(id);
  emit changed(id);
}

void ROISet::write(QDomElement dst) const {
  QDomDocument doc = dst.ownerDocument();
  foreach (int id, allids) {
    QDomElement e = doc.createElement("roi");
    dst.appendChild(e);
    e.setAttribute("id",QString("%1").arg(id));
    e.setAttribute("cam",cams[id].donor + ":" + cams[id].acceptor);
    map[id].write(e);
  }
}

void ROISet::read(QDomElement src) {
  clearNoSignal();
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
      if (c.size()>=2) {
	cams[id].donor = c[0];
	cams[id].acceptor = c[1];
      }
    }
    map[id].read(e);
  }
  emit changedAll();
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

void ROISet::clearNoSignal() {
  map.clear();
  cams.clear();
  allids.clear();
  lastid = 0;
}

void ROISet::clear() {
  clearNoSignal();
  emit changedAll();
}


void ROISet::save(QString fn) const {
  XML xml(0,"vsdscopeROIs");
  QDomElement roidoc = xml.append("rois");
  write(roidoc);
  xml.write(fn);
}
  
void ROISet::load(QString fn) {
  XML xml(fn);
  QDomElement roidoc = xml.find("rois");
  read(roidoc);
}
  
ROIWriter::ROIWriter(ROISet &rs, int id):
  roi(rs.checkout(id)), rs(rs), id(id)  {
  canceled = false;
}

void ROIWriter::cancel() {
  canceled = true;
}

ROIWriter::~ROIWriter() {
  if (!canceled)
    rs.checkin(id);
}
