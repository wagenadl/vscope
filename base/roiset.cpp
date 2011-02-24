// roiset.cpp

#include "roiset.h"
#include <base/dbg.h>
#include <base/xml.h>

ROISet::ROISet(QString cam): dfltcam(cam) {
  lastid = 0;
}

ROISet::~ROISet() {
}

int ROISet::newROI(QString cam) {
  int id = ++lastid;
  allids.insert(id);
  cams[id] = cam.isEmpty() ? dfltcam : cam;
  return id;
}

QString const &ROISet::cam(int id) const {
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

bool ROISet::isXYRRA(int id) const {
  return contains(id) && map[id].isXYRRA();
}

bool ROISet::isPoly(int id) const {
  return contains(id) && map[id].isBlob();
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
    e.setAttribute("cam",cams[id]);
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
    if (e.hasAttribute("cam"))
      cams[id] = e.attribute("cam");
    else
      cams[id] = dfltcam;
    map[id].read(e);
  }
  emit changedAll();
}

QSet<int> const &ROISet::ids() const {
  return allids;
}

QSet<int> ROISet::idsForCam(QString cam) const {
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

double ROISet::centerX(int id) const {
  if (map.contains(id))
    return map[id].centerX();
  throw Exception("ROISet","centerX: no data");
}

double ROISet::centerY(int id) const {
  if (map.contains(id))
    return map[id].centerY();
  throw Exception("ROISet","centerY: no data");
}


bool ROISet::inside(int id, double x, double y, double marg) const {
  if (map.contains(id))
    return map[id].inside(x, y, marg);
  throw Exception("ROISet","inside: no data");
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
