// roiimages.cpp

#include "roiimages.h"

ROIImages::ROIImages() {
  sm = new QSignalMapper(this);
  connect(sm, SIGNAL(mapped(QString)), SLOT(shareZoom(QString)));
}

void ROIImages::add(QString id, ROIImage *img) {
  imgs[id] = img;
  connect(img,SIGNAL(newZoom(QRect)), sm, SLOT(map()));
  sm->setMapping(img, id);
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

void ROIImages::shareZoom(QString id) {
  QRect zr = imgs[id].currentZoom();
  // we should convert zr to global coordinates
  foreach (QString id1, imgs.keys()) {
    if (id1!=id) {
      // we should convert zr to id1's image coordinates
      imgs[id1].updateZoom(zr);
    }
  }
  emit newZoom(zr);
}

void ROIImages::updateZoom(QRect zr) {
  foreach (QString id, imgs.keys()) {
    // we should convert zr to id1's image coordinates
    imgs[id].updateZoom(zr);
  }
}
