// roiimages.cpp

#include "roiimages.h"
#include <base/exception.h>
#include <base/dbg.h>

ROIImages::ROIImages(QRect c): CCDImages(c) {
  sm = new QSignalMapper(this);
  zm = new QSignalMapper(this);
  connect(zm, SIGNAL(mapped(QString)), SLOT(shareZoom(QString)));
  connect(sm, SIGNAL(mapped(QString)), SLOT(shareSelection(QString)));
}

ROIImages::~ROIImages() {
}

void ROIImages::add(QString id, ROIImage *img) {
  CCDImages::add(id, img);
  connect(img,SIGNAL(newZoom(QRect)), zm, SLOT(map()));
  connect(img,SIGNAL(newSelection(int)), sm, SLOT(map()));
  zm->setMapping(img, id);
  sm->setMapping(img, id);
}

ROIImage *ROIImages::get(QString id) {
  ROIImage *img = dynamic_cast<ROIImage *>(CCDImages::get(id));
  if (img)
    return img;
  else
    throw Exception("ROIImages", "No image known as " + id);
}

ROIImage *ROIImages::first() {
  if (imgs.isEmpty())
    throw Exception("ROIImages", "No images");
  ROIImage *img = dynamic_cast<ROIImage *>(imgs.begin().value());
  if (img)
    return img;
  else
    throw Exception("ROIImages", "First image is no ROIImage");    
}
      
void ROIImages::setMode(ROIImage::ClickMode cm) {
  foreach (ROIImage *ri, images())
    ri->setMode(cm);
}

void ROIImages::showROIs(SHOWROIS sm) {
  foreach (ROIImage *ri, images())
    ri->showROIs(sm);
}

void ROIImages::setROIs(ROISet *rs) {
  foreach (ROIImage *ri, images())
    ri->setROIs(rs);
}

void ROIImages::shareZoom(QString id) {
  Dbg() << "ROIImages::shareZoom from " << id;
  QRect zr = get(id)->currentZoom();
  foreach (QString id1, imgs.keys()) {
    if (id1!=id) {
      imgs[id1]->updateZoom(zr);
    }
  }
  emit newZoom(zr);
}

void ROIImages::shareSelection(QString id) {
  Dbg() << "ROIImages::shareSelection from " << id;
  int sel = get(id)->currentROI();
  foreach (QString id1, imgs.keys()) 
    if (id1!=id) 
      get(id1)->updateSelection(sel);
  Dbg() << "ROIImages: emitting new selection: " << sel;
  emit newSelection(sel);
}

void ROIImages::updateZoom(QRect zr) {
  foreach (ROIImage *ri, images())
    ri->updateZoom(zr);
}

void ROIImages::updateSelection(int sel) {
  foreach (ROIImage *ri, images())
    ri->updateSelection(sel);
}
  
QList<ROIImage *> ROIImages::images() {
  QList<ROIImage *> res;
  foreach (CCDImage *ci, imgs.values()) {
    ROIImage *ri = dynamic_cast<ROIImage *>(ci);
    if (ri)
      res << ri;
    else
      throw Exception("ROIImages", "Not a ROI Image");
  }
  return res;
}
