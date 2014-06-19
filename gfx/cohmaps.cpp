// cohmaps.cpp

#include "cohmaps.h"
#include <base/exception.h>
#include <base/dbg.h>

CohMaps::CohMaps(QRect c): CCDImages(c) {
  sm = new QSignalMapper(this);
  connect(sm, SIGNAL(mapped(QString)), SLOT(shareSelection(QString)));
}

CohMaps::~CohMaps() {
}

void CohMaps::add(QString id, Coherence *img) {
  CCDImages::add(id, img);
  connect(img,SIGNAL(newSelection(int)), sm, SLOT(map()));
  sm->setMapping(img, id);
}

Coherence *CohMaps::get(QString id) {
  Coherence *img = dynamic_cast<Coherence *>(CCDImages::get(id));
  if (img)
    return img;
  else
    throw Exception("Coherences", "No image known as " + id);
}

Coherence *CohMaps::first() {
  if (imgs.isEmpty())
    throw Exception("Coherences", "No images");
  Coherence *img = dynamic_cast<Coherence *>(imgs.begin().value());
  if (img)
    return img;
  else
    throw Exception("Coherences", "First image is no Coherence");    
}

void CohMaps::shareSelection(QString id) {
  int sel = get(id)->currentROI();
  foreach (QString id1, imgs.keys()) 
    if (id1!=id) 
      get(id1)->updateSelection(sel);
  emit newSelection(sel);
}

void CohMaps::updateSelection(int sel) {
  foreach (Coherence *c, images())
    c->updateSelection(sel);
}

void CohMaps::setRefFreq(double f) {
  foreach (Coherence *c, images())
    c->setRefFreq(f);
}

void CohMaps::setShowMode(SHOWROIS m) {
  foreach (Coherence *c, images())
    c->setShowMode(m);
}

void CohMaps::setRefTrace(QString s) {
  foreach (Coherence *c, images())
    c->setRefTrace(s);
}

void CohMaps::setRefDigi(QString s) {
  foreach (Coherence *c, images())
    c->setRefDigi(s);
}

QList<Coherence *> CohMaps::images() {
  QList<Coherence *> res;
  foreach (CCDImage *ci, imgs.values()) {
    Coherence *ri = dynamic_cast<Coherence *>(ci);
    if (ri)
      res << ri;
    else
      throw Exception("Coherences", "Not a ROI Image");
  }
  return res;
}

void CohMaps::adjustedRange(QString camid, uint16_t const *data, int X, int Y) {
  if (has(camid)) {
    Coherence *img = get(camid);
    if (img)
      img->adjustedRange(data, X, Y);
    else
      Dbg() << "CohMaps::adjustedRange: bad id: " << camid;
  } else {
    Dbg() << "CohMaps::adjustedRange: no such id: " << camid;
  }
}

void CohMaps::newImage(QString camid, uint16_t const *data, int X, int Y,
                       Transform const &t) {
  if (has(camid)) {
    Coherence *img = get(camid);
    if (img)
      img->newImage(data, X, Y, t);
    else
      Dbg() << "CohMaps::newImage: bad id: " << camid;
  } else {
    Dbg() << "CohMaps::newImage: no such id: " << camid;
  }
}
