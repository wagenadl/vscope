// cohmaps.cpp

#include "cohmaps.h"
#include <base/exception.h>
#include <base/dbg.h>

CohMaps::CohMaps(QRect c): canvas(c) {
  sm = new QSignalMapper(this);
  zm = new QSignalMapper(this);
  connect(zm, SIGNAL(mapped(QString)), SLOT(shareZoom(QString)));
  connect(sm, SIGNAL(mapped(QString)), SLOT(shareSelection(QString)));
}

CohMaps::~CohMaps() {
}

void CohMaps::add(QString id, Coherence *img) {
  img->setCanvas(canvas);
  connect(img,SIGNAL(newZoom(QRect)), zm, SLOT(map()));
  connect(img,SIGNAL(newSelection(int)), sm, SLOT(map()));
  zm->setMapping(img, id);
  sm->setMapping(img, id);
  imgs[id] = img;
}

void CohMaps::del(QString id) {
  if (!imgs.contains(id))
    return;
  delete imgs[id];
  imgs.remove(id);
}

QStringList CohMaps::ids() const {
  QStringList r;
  foreach (QString id, imgs.keys())
    r.append(id);
  return r;
}

bool CohMaps::has(QString id) const {
  return imgs.contains(id);
}

Coherence *CohMaps::get(QString id) {
  if (imgs.contains(id))
    return imgs.value(id);
  else
    throw Exception("CohMaps", "No image known as " + id);
}

Coherence *CohMaps::first() {
  if (imgs.isEmpty())
    throw Exception("CohMaps", "No images");
  return imgs.begin().value();
}
      
void CohMaps::setMode(Coherence::ClickMode cm) {
  foreach (Coherence *ri, imgs.values())
    ri->setMode(cm);
}

void CohMaps::showROIs(SHOWROIS sm) {
  foreach (Coherence *ri, imgs.values())
    ri->showROIs(sm);
}

void CohMaps::setROIs(ROISet *rs) {
  foreach (Coherence *ri, imgs.values())
    ri->setROIs(rs);
}

void CohMaps::setCanvas(QRect const &r) {
  foreach (Coherence *ri, imgs.values())
    ri->setCanvas(r);
}

void CohMaps::shareZoom(QString id) {
  Dbg() << "CohMaps::shareZoom from " << id;
  QRect zr = imgs[id]->currentZoom();
  foreach (QString id1, imgs.keys()) {
    if (id1!=id) {
      imgs[id1]->updateZoom(zr);
    }
  }
  emit newZoom(zr);
}

void CohMaps::shareSelection(QString id) {
  Dbg() << "CohMaps::shareSelection from " << id;
  int sel = imgs[id]->currentROI();
  foreach (QString id1, imgs.keys()) {
    if (id1!=id) {
      imgs[id1]->updateSelection(sel);
    }
  }
  Dbg() << "CohMaps: emitting new selection: " << sel;
  emit newSelection(sel);
}

void CohMaps::updateZoom(QRect zr) {
  foreach (QString id, imgs.keys()) {
    imgs[id]->updateZoom(zr);
  }
}

void CohMaps::updateSelection(int sel) {
  foreach (QString id, imgs.keys()) {
    imgs[id]->updateSelection(sel);
  }
}
  
