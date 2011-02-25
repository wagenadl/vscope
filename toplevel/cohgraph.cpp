// cohgraph.cpp

#include "cohgraph.h"
#include <QPainter>
#include <QPaintEvent>
#include <base/roiset.h>
#include <base/xyrra.h>
#include <base/numbers.h>
#include <base/base26.h>
#include <acq/trial.h>
#include <math/cohest.h>
#include <math/psdest.h>
#include <gui/guiexc.h>
#include <toplevel/vsdtraces.h>
#include <xml/enumerator.h>
#include <base/analogdata.h>
#include <base/digitaldata.h>
#include <base/roi3data.h>
#include <base/roidata3set.h>
#include <math/taperbank.h>
#include <base/dbg.h>
#include <xml/paramtree.h>
#include <math.h>
#include <math/cohdata.h>
#include <acq/datatrove.h>

CohGraph::CohGraph(CohData *dat, QWidget *p): RadialGraph(p) {
  if (dat) {
    data = dat;
    owndata = false;
  } else {
    data = new CohData();
    data->newROISet(&Globals::trove->rois());
    data->newCCDData(&Globals::trove->roidata());
    owndata = true;
  }
  connect(data->currentData(), SIGNAL(changedOne(int)), this, SLOT(newData()));
  connect(data->currentData(), SIGNAL(changedAll()), this, SLOT(newData()));
}

CohGraph::~CohGraph() {
  if (owndata && data) 
    delete data;
}

void CohGraph::paintEvent(QPaintEvent *e) {
  RadialGraph::paintEvent(e);
  dbg("cohgraph::paintevent");
  if (!data)
    return;
  ROISet const *roiset = data->currentData()->getROISet();
  if (!roiset)
    return;

  QPainter p(this);
  foreach (int id, roiset->ids()) {
    if (id<=0)
      continue;
    double pha = data->phase(id);
    double mag = data->magnitude(id);
    int hue = int(pha*180/3.141592);
    if (hue<0)
      hue+=360;
    int val = int(mag*255);
    int sat = int(mag*255);
    QColor c; c.setHsv(hue,sat,val);
    p.setBrush(c);
    p.setPen(QPen(Qt::NoPen));
    double x = toScreenX(mag*cos(pha));
    double y = toScreenY(-mag*sin(pha));
    // p.drawEllipse(QPointF(x,y), 2.0, 2.0);

//    p.setPen(QColor("#000000"));
//    p.drawText(QRectF(x,y-1,2,2),
//	       Qt::AlignCenter | Qt::TextDontClip,
//	       num2az(id));
    p.setPen(c); // QColor("#ffffff")); // anti b.g.
    p.drawText(QRectF(x-1,y-1,2,2),
	       Qt::AlignCenter | Qt::TextDontClip,
	       num2az(id));
  }
  
  p.setPen(QColor("#000000"));
  p.drawText(5,12,QString("f* = %1 Hz").arg(data->getFStarHz(),0,'f',2));
}

void CohGraph::showEvent(QShowEvent *e) {
  RadialGraph::showEvent(e);
  update();
}


void CohGraph::newData() {
  perhapsRefresh();
}

void CohGraph::perhapsRefresh() {
  if (data) 
    data->invalidate();
  if (isVisible())
    update();
}

void CohGraph::setRefDigi(int digiline) {
  if (data)
    data->setRefDigi(digiline);
  perhapsRefresh();
}

void CohGraph::setRefTrace(int ach) {
  if (data)
    data->setRefTrace(ach);
  perhapsRefresh();
}

void CohGraph::setRefFreq(double fref_hz) {
  dbg("cohgraph setRefFreq: %g",fref_hz);
  if (data)
    data->setRefFreq(fref_hz);
  perhapsRefresh();
}

