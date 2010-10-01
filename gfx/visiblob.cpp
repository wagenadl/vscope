// visiblob.cpp

#include "visiblob.h"
#include <QPainter>
#include <QMouseEvent>
#include <base/dbg.h>

VisiBlob::VisiBlob(QWidget *parent): QWidget(parent) {
  pb = 0;
  ax = 1; bx = 0;
  ay = 1; by = 0;
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
  pb = pb0;
  if (redraw)
    update();
}

void VisiBlob::setZoom(double ax0, double bx0, double ay0, double by0) {
  ax = ax0;
  bx = bx0;
  ay = ay0;
  by = by0;
  update();
}

void VisiBlob::paintEvent(class QPaintEvent *) {
  QPainter pntr(this);
  pntr.setPen(pen);
  if (newpoly)
    pntr.drawPolyline(*newpoly);
  else if (pb)
    pb->paint(&pntr, ax,bx, ay,by);
}

void VisiBlob::startCreate(PolyBlob *dest, QMouseEvent *e) {
  pb = dest;
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
    newpoly->push_back(QPointF(e->x(),e->y()));
    break;
  case DISTORT:
    pb->adjust((e->x()-bx)/ax, (e->y()-by)/ay, false);
    break;
  case MOVE:
    // dbg("Move!");
    pb->recenter(startBlob.x() + (e->x()-startScreen.x())/ax,
		 startBlob.y() + (e->y()-startScreen.y())/ay);
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
    double *xx = new double[k];
    double *yy = new double[k];
    for (int i=0; i<k; i++) {
      xx[i] = ((*newpoly)[i].x()-bx)/ax;
      yy[i] = ((*newpoly)[i].y()-by)/ay;
    }
    pb->build(k, xx, yy);
    delete newpoly;
    newpoly = 0;
    delete [] xx;
    delete [] yy;
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
  pb->adjust((e->x()-bx)/ax, (e->y()-by)/ay, true);
}

void VisiBlob::startMove(QMouseEvent *e) {
  dbg("Visiblob::startmove. pb=%p",pb);
  if (!pb)
    return;
  dragtype = MOVE;
  startScreen = e->posF();
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
