// gfx/visiblob.cpp - This file is part of VScope.
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

// visiblob.cpp

#include "visiblob.h"
#include <QPainter>
#include <QMouseEvent>
#include <base/dbg.h>

VisiBlob::VisiBlob(QWidget *parent): QWidget(parent) {
  pb = 0;
  newpoly = 0;
  indrag = false;
  setEnabled(false);
  setGeometry(parent->rect());
}

VisiBlob::~VisiBlob() {
  if (newpoly)
    delete newpoly;
}

void VisiBlob::setPen(QPen const &p) {
  pen = p;
  update();
}

void VisiBlob::setShape(PolyBlob *pb0, bool redraw) {
  //if (pb)
  //  disconnect(pb,SIGNAL(destroyed(QObject*)),this,SLOT(pbDied(QObject*)));
  pb = pb0;
  //if (pb)
  //  connect(pb,SIGNAL(destroyed(QObject*)),this,SLOT(pbDied(QObject*)));
  if (redraw)
    update();
}

void VisiBlob::setTransform(Transform const &t0) {
  t = t0;
  tinv = t.inverse();
  update();
}

void VisiBlob::paintEvent(class QPaintEvent *) {
  QPainter pntr(this);
  pntr.setPen(pen);
  if (newpoly)
    pntr.drawPolyline(*newpoly);
  else if (pb)
    pb->paint(&pntr, t);
}

void VisiBlob::startCreate(PolyBlob *dest, QMouseEvent *e) {
  setShape(dest, false);
  if (!pb)
    return;
  dragtype = CREATE;
  indrag = true;
  newpoly = new QPolygonF(0);
  drag(e);
}

void VisiBlob::drag(QMouseEvent *e) {
  if (!pb || !indrag)
    return;
  switch (dragtype) {
  case CREATE:
    newpoly->push_back(Transform::pixelCenter(e->pos()));
    break;
  case DISTORT:
    pb->adjust(tinv(e->pos()), false);
    break;
  case MOVE:
    // dbg("Move!");
    pb->recenter(startBlob
		 + tinv.mapDisplacement(Transform::pixelCenter(e->pos())
					- startScreen));
    break;
  }
  update();
}

bool VisiBlob::complete(QMouseEvent *) {
  if (!pb || !indrag)
    return false;
  switch (dragtype) {
  case CREATE: {
    int k = newpoly->size();
    QVector<double> xx(k);
    QVector<double> yy(k);
    for (int i=0; i<k; i++) {
      QPointF xy = tinv((*newpoly)[i]);
      xx[i] = xy.x();
      yy[i] = xy.y();
    }
    pb->build(xx, yy);
    delete newpoly;
    newpoly = 0;
  } break;
  case DISTORT:
    break;
  case MOVE:
    break;
  }
  update();
  indrag = false;
  return true;
}

void VisiBlob::startAdjust(class QMouseEvent *e) {
  if (!pb)
    return;
  dragtype = DISTORT;
  indrag = true;
  pb->adjust(tinv(Transform::pixelCenter(e->pos())), true);
}

void VisiBlob::startMove(QMouseEvent *e) {
  dbg("Visiblob::startmove. pb=%p",pb);
  if (!pb)
    return;
  dragtype = MOVE;
  startScreen = Transform::pixelCenter(e->pos());
  startBlob = QPointF(pb->x0(), pb->y0());
  indrag=true;
}

void VisiBlob::mousePressEvent(class QMouseEvent *) {
  dbg("visiblob: mouse press");
}

void VisiBlob::mouseReleaseEvent(class QMouseEvent *) {
  dbg("visiblob: mouse release");
}

void VisiBlob::mouseMoveEvent(class QMouseEvent *) {
  dbg("visiblob: mouse move");
}

void VisiBlob::mouseDoubleClickEvent(class QMouseEvent *) {
  dbg("visiblob: mouse doubleclick");
}

