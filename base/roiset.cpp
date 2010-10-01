// roiset.cpp

#include "roiset.h"
#include <base/dbg.h>
#include <base/xml.h>

ROISet::ROISet() {
}

ROISet::~ROISet() {
}

void ROISet::set(int id, XYRRA const &roi) {
  allids.insert(id);
  emap.insert(id, roi);
  pmap.remove(id);
}

void ROISet::set(int id, PolyBlob const &roip) {
  allids.insert(id);
  pmap.insert(id, roip);
  emap.remove(id);
}

PolyBlob &ROISet::newblob(int id) {
  allids.insert(id);
  emap.remove(id);
  pmap.insert(id, PolyBlob());
  return getp(id);
}
    
PolyBlob &ROISet::newblob(int id, int log2n) {
  allids.insert(id);
  emap.remove(id);
  pmap.insert(id, PolyBlob(log2n));
  return getp(id);
}
    

XYRRA ROISet::get(int id) const {
  if (emap.contains(id))
    return emap.value(id);
  else
    throw Exception("ROISet",
		    QString("No XYRRA-style ROI with ID=%1").arg(id),"get");
}

PolyBlob const &ROISet::getp(int id) const {
  QMap<int, PolyBlob>::const_iterator i = pmap.find(id);
  if (i!=pmap.end())
    return i.value();
  else
    throw Exception("ROISet",
		    QString("No PolyBlob-style ROI with ID=%1").arg(id),"getp");
}

PolyBlob &ROISet::getp(int id) {
  if (pmap.contains(id))
    return pmap[id];
  else
    throw Exception("ROISet",
		    QString("No PolyBlob-style ROI with ID=%1").arg(id),"getp");
}

bool ROISet::contains(int id) const {
  return allids.contains(id);
}

bool ROISet::isXYRRA(int id) const {
  return emap.contains(id);
}

bool ROISet::isPoly(int id) const {
  return pmap.contains(id);
}

void ROISet::remove(int id) {
  allids.remove(id);
  emap.remove(id);
  pmap.remove(id);
}

void ROISet::write(QDomElement dst) const {
  QDomDocument doc = dst.ownerDocument();
  for (QMap<int, XYRRA>::const_iterator i=emap.begin(); i!=emap.end(); ++i) {
    XYRRA const &roi = i.value();
    QDomElement e = doc.createElement("roi");
    dst.appendChild(e);
    e.setAttribute("id",QString("%1").arg(i.key()));
    dbg("roiset::write xyrra %i",i.key());
    roi.write(e);
  }
  for (QMap<int, PolyBlob>::const_iterator i=pmap.begin();
       i!=pmap.end(); ++i) {
    PolyBlob const &roi = i.value();
    QDomElement e = doc.createElement("roi");
    dst.appendChild(e);
    e.setAttribute("id",QString("%1").arg(i.key()));
    roi.write(e);
  }
}

void ROISet::read(QDomElement src) {
  clear();
  bool ok;
  for (QDomElement e=src.firstChildElement("roi");
       !e.isNull(); e=e.nextSiblingElement("roi")) {
    int id = e.attribute("id").toInt(&ok);
    if (!ok)
      throw Exception("ROISet","Cannot read id from xml");
    if (e.hasAttribute("n")) {
      // it must be polyblob-style
      PolyBlob &roi = newblob(id, 0);
      roi.read(e);
    } else {
      // it must be xyrra-style
      XYRRA roi;
      roi.read(e);
      set(id, roi);
    }
  }
}

QSet<int> const &ROISet::ids() const {
  return allids;
}

void ROISet::clear() {
  pmap.clear();
  emap.clear();
  allids.clear();
}

double ROISet::centerX(int id) const {
  if (emap.contains(id))
    return emap.value(id).x0;
  else if (pmap.contains(id))
    return pmap.value(id).x0();
  else
    return 0;
}

double ROISet::centerY(int id) const {
  if (emap.contains(id))
    return emap.value(id).y0;
  else if (pmap.contains(id))
    return pmap.value(id).y0();
  else
    return 0;
}

bool ROISet::inside(int id, double x, double y, double marg) const {
  if (emap.contains(id))
    return emap.value(id).inside(x, y, marg);
  else if (pmap.contains(id))
    return pmap.value(id).inside(x, y, marg);
  else
    return false;
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
  
