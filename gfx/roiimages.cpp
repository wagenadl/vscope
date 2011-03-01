// roiimages.cpp

#include "roiimages.h"
#include <base/exception.h>
#include <base/dbg.h>

ROIImages::ROIImages(QRect c): canvas(c) {
  sm = new QSignalMapper(this);
  zm = new QSignalMapper(this);
  connect(zm, SIGNAL(mapped(QString)), SLOT(shareZoom(QString)));
  connect(sm, SIGNAL(mapped(QString)), SLOT(shareSelection(QString)));
}

ROIImages::~ROIImages() {
}

void ROIImages::add(QString id, ROIImage *img) {
  img->setCanvas(canvas);
  connect(img,SIGNAL(newZoom(QRect)), zm, SLOT(map()));
  connect(img,SIGNAL(newSelection(int)), sm, SLOT(map()));
  zm->setMapping(img, id);
  sm->setMapping(img, id);
  imgs[id] = img;
}

ROIImage *ROIImages::get(QString id) {
  if (imgs.contains(id))
    return imgs.value(id);
  else
    throw Exception("ROIImages", "No image known as " + id);
}

ROIImage *ROIImages::first() {
  if (imgs.isEmpty())
    throw Exception("ROIImages", "No images");
  return imgs.begin().value();
}
      
void ROIImages::setMode(ROIImage::ClickMode cm) {
  foreach (ROIImage *ri, imgs.values())
    ri->setMode(cm);
}

void ROIImages::showROIs(ROIImage::ShowMode sm) {
  foreach (ROIImage *ri, imgs.values())
    ri->showROIs(sm);
}

void ROIImages::setROIs(ROISet *rs) {
  foreach (ROIImage *ri, imgs.values())
    ri->setROIs(rs);
}

void ROIImages::setCanvas(QRect const &r) {
  foreach (ROIImage *ri, imgs.values())
    ri->setCanvas(r);
}

void ROIImages::shareZoom(QString id) {
  Dbg() << "ROIImages::shareZoom from " << id;
  QRect zr = imgs[id]->currentZoom();
  foreach (QString id1, imgs.keys()) {
    if (id1!=id) {
      imgs[id1]->updateZoom(zr);
    }
  }
  emit newZoom(zr);
}

void ROIImages::shareSelection(QString id) {
  Dbg() << "ROIImages::shareSelection from " << id;
  int sel = imgs[id]->currentROI();
  foreach (QString id1, imgs.keys()) {
    if (id1!=id) {
      imgs[id1]->updateSelection(sel);
    }
  }
  Dbg() << "ROIImages: emitting new selection: " << sel;
  emit newSelection(sel);
}

void ROIImages::updateZoom(QRect zr) {
  foreach (QString id, imgs.keys()) {
    imgs[id]->updateZoom(zr);
  }
}
