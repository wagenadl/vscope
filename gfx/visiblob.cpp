// visiblob.cpp

#include "visiblob.h"
#include <QPainter>
#include <QMouseEvent>
#include <base/dbg.h>
#include <base/memalloc.h>

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
    newpoly->push_back(QPointF(e->x(),e->y()));
    break;
  case DISTORT:
    pb->adjust(tinv(e->pos()), false);
    break;
  case MOVE:
    // dbg("Move!");
    pb->recenter(startBlob + tinv(e->pos()-startScreen));
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
    double *xx = memalloc<double>(k, "VisiBlob");
    double *yy = memalloc<double>(k, "VisiBlob");
    for (int i=0; i<k; i++) {
      QPointF xy = tinv((*newpoly)[i]);
      xx[i] = xy.x();
      yy[i] = xy.y();
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
  pb->adjust(tinv(e->pos()), true);
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

void VisiBlob::pbDies(QObject *x) {
  //if (x==pb) {
  //  Dbg() << "VisiBlob Warning: My blob is dying";
  //  pb = 0;
  //}
}
