// gfx/roiimages.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// roiimages.cpp

#include "roiimages.h"
#include <base/exception.h>
#include <base/dbg.h>

ROIImages::ROIImages(QRect c): CCDImages(c) {
  rs = 0;
  sm = new QSignalMapper(this);
  connect(sm, SIGNAL(mapped(QString)), SLOT(shareSelection(QString)));
}

ROIImages::~ROIImages() {
}

void ROIImages::add(QString id, ROIImage *img) {
  CCDImages::add(id, img);
  img->setROIs(rs);
  connect(img,SIGNAL(newSelection(int)), sm, SLOT(map()));
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

void ROIImages::setROIs(ROISet *rs_) {
  rs = rs_;
  foreach (ROIImage *ri, images())
    ri->setROIs(rs);
}

void ROIImages::shareSelection(QString id) {
  int sel = get(id)->currentROI();
  foreach (QString id1, imgs.keys()) 
    if (id1!=id) 
      get(id1)->updateSelection(sel);
  emit newSelection(sel);
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
