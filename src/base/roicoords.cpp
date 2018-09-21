// base/roicoords.cpp - This file is part of VScope.
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

// roicoords.cpp

#include "roicoords.h"
#include <base/exception.h>
#include <base/dbg.h>

ROICoords::ROICoords() {
  dataXYRRA = 0;
  dataBlob = 0;
}

ROICoords::ROICoords(XYRRA const &xyrra) {
  dataXYRRA = new XYRRA(xyrra);
  dataBlob = 0;
}

ROICoords::ROICoords(PolyBlob const &blob) {
  dataBlob = new PolyBlob(blob);
  dataXYRRA = 0;
}

ROICoords::ROICoords(ROICoords const &roi) {
  dataBlob = 0;
  dataXYRRA = 0;
  if (roi.dataBlob)
    dataBlob = new PolyBlob(*roi.dataBlob);
  if (roi.dataXYRRA)
    dataXYRRA = new XYRRA(*roi.dataXYRRA);
}

ROICoords &ROICoords::operator=(ROICoords const &roi) {
  if (dataBlob)
    delete dataBlob;
  dataBlob = 0;
  if (dataXYRRA)
    delete dataXYRRA;
  dataXYRRA = 0;
  if (roi.dataBlob)
    dataBlob = new PolyBlob(*roi.dataBlob);
  if (roi.dataXYRRA)
    dataXYRRA = new XYRRA(*roi.dataXYRRA);
  return *this;
}


ROICoords::~ROICoords() {
  if (dataXYRRA)
    delete dataXYRRA;
  if (dataBlob)
    delete dataBlob;
}

ROICoords::ROICoords(QDomElement doc) {
  dataXYRRA = 0;
  dataBlob = 0;
  read(doc);
}

void ROICoords::makeBlob(int log2n) {
  if (dataXYRRA) {
    delete dataXYRRA;
    dataXYRRA=0;
  }
  if (dataBlob && dataBlob->log2nPoints()!=log2n) {
    delete dataBlob;
    dataBlob = 0;
  }
  if (!dataBlob)
    dataBlob = new PolyBlob(log2n);
}

void ROICoords::makeXYRRA() {
  if (dataBlob)
    delete dataBlob;
  dataBlob = 0;
  if (!dataXYRRA)
    dataXYRRA = new XYRRA();
}

ROICoords &ROICoords::operator=(XYRRA const &xyrra) {
  if (dataBlob)
    delete dataBlob;
  dataBlob=0;
  if (!dataXYRRA)
    dataXYRRA = new XYRRA(xyrra);
  else
    *dataXYRRA = xyrra;
  return *this;
}

ROICoords &ROICoords::operator=(PolyBlob const &blob) {
  if (dataXYRRA)
    delete dataXYRRA;
  dataXYRRA=0;
  if (dataBlob)
    *dataBlob = blob;
  else 
    dataBlob = new PolyBlob(blob);
  return *this;
}

bool ROICoords::isXYRRA() const {
  return dataXYRRA;
}

bool ROICoords::isBlob() const {
  return dataBlob;
}

PolyBlob const &ROICoords::blob() const {
  if (dataBlob)
    return *dataBlob;
  throw Exception("ROICoords","Not in PolyBlob form");
}

PolyBlob &ROICoords::blob() {
  if (dataBlob)
    return *dataBlob;
  throw Exception("ROICoords","Not in PolyBlob form");
}

XYRRA const &ROICoords::xyrra() const {
  if (dataXYRRA)
    return *dataXYRRA;
  throw Exception("ROICoords","Not in XYRRA form");
}

XYRRA &ROICoords::xyrra() {
  if (dataXYRRA)
    return *dataXYRRA;
  throw Exception("ROICoords","Not in XYRRA form");
}

void ROICoords::write(QDomElement doc) const {
  if (dataXYRRA)
    dataXYRRA->write(doc);
  else if (dataBlob)
    dataBlob->write(doc);
  else
    Dbg() << "ROICoords::write: Caution: nothing to write";
}

void ROICoords::read(QDomElement doc) {
  if (doc.hasAttribute("n")) {
    // polyblob style
    if (dataXYRRA)
      delete dataXYRRA;
    dataXYRRA = 0;
    if (dataBlob)
      dataBlob->read(doc);
    else
      dataBlob = new PolyBlob(doc);
  } else {
    if (dataBlob)
      delete dataBlob;
    dataBlob = 0;
    if (dataXYRRA)
      dataXYRRA->read(doc);
    else
      dataXYRRA = new XYRRA(doc);
  }
}

QPointF ROICoords::center() const {
  return QPointF(centerX(), centerY());
}

double ROICoords::centerX() const {
  if (dataXYRRA)
    return dataXYRRA->x0;
  if (dataBlob)
    return dataBlob->x0();
  Dbg() << "ROICoords::centerX(): no data";
  return 0;
}

double ROICoords::centerY() const {
  if (dataXYRRA)
    return dataXYRRA->y0;
  if (dataBlob)
    return dataBlob->y0();
  Dbg() << "ROICoords::centerY(): no data";
  return 0;
}

bool ROICoords::inside(double x, double y, double marg) const {
  if (dataXYRRA)
    return dataXYRRA->inside(x, y, marg);
  if (dataBlob)
    return dataBlob->inside(x, y, marg);
  Dbg() << "ROICoords::inside(): no data";
  return false;
}
 
bool ROICoords::inside(QPointF xy, double marg) const {
  return inside(xy.x(), xy.y(), marg);
}

void ROICoords::translate(double dx, double dy) {
  if (dataXYRRA)
    dataXYRRA->translate(dx, dy);
  if (dataBlob)
    dataBlob->translate(dx, dy);
}
