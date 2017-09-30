// gfx/ccdimages.cpp - This file is part of VScope.
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

// ccdimages.cpp

#include "gfx/ccdimages.h"

#include <base/exception.h>
#include <base/dbg.h>

CCDImages::CCDImages(QRect c): canvas(c) {
  zm = new QSignalMapper(this);
  connect(zm, SIGNAL(mapped(QString)), SLOT(shareZoom(QString)));
}

CCDImages::~CCDImages() {
}

void CCDImages::add(QString id, CCDImage *img) {
  img->setCanvas(canvas);
  imgs[id] = img;
  connect(img,SIGNAL(newZoom(QRect)), zm, SLOT(map()));
  zm->setMapping(img, id);
}

void CCDImages::del(QString id) {
  if (!imgs.contains(id))
    return;
  delete imgs[id];
  imgs.remove(id);
}

QStringList CCDImages::ids() const {
  QStringList r;
  foreach (QString id, imgs.keys())
    r.append(id);
  return r;
}

bool CCDImages::has(QString id) const {
  return imgs.contains(id);
}

CCDImage *CCDImages::get(QString id) {
  if (imgs.contains(id))
    return imgs.value(id);
  else
    throw Exception("CCDImages", "No image known as " + id);
}

CCDImage *CCDImages::first() {
  if (imgs.isEmpty())
    throw Exception("CCDImages", "No images");
  return imgs.begin().value();
}
      

void CCDImages::setCanvas(QRect const &r) {
  foreach (CCDImage *ri, imgs.values())
    ri->setCanvas(r);
}

void CCDImages::shareZoom(QString id) {
  Dbg() << "CCDImages::shareZoom from " << id;
  QRect zr = get(id)->currentZoom();
  foreach (QString id1, imgs.keys()) {
    if (id1!=id) {
      imgs[id1]->updateZoom(zr);
    }
  }
  emit newZoom(zr);
}

void CCDImages::updateZoom(QRect zr) {
  foreach (CCDImage *ri, imgs.values())
    ri->updateZoom(zr);
}
