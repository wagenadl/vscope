// ccdimages.cpp

#include "gfx/ccdimages.h"

#include <base/exception.h>
#include <base/dbg.h>

CCDImages::CCDImages(QRect c): canvas(c) {
}

CCDImages::~CCDImages() {
}

void CCDImages::add(QString id, CCDImage *img) {
  img->setCanvas(canvas);
   imgs[id] = img;
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
