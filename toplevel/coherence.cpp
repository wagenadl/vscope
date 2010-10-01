// coherence.cpp

#include "coherence.h"
#include <QPainter>
#include <QPaintEvent>
#include <base/roiset.h>
#include <base/polyblob.h>
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
#include <math/taperbank.h>
#include <base/dbg.h>
#include <xml/paramtree.h>
#include <math.h>
#include <math/cohdata.h>
#include <acq/datatrove.h>

Coherence::Coherence(VSDTraces *s, CohData *dat, QWidget *p):
  CCDImage(p), source(s) {
  if (dat) {
    data = dat;
    owndata = false;
  } else {
    data = new CohData();
    data->newROISet(source->getROIs());
    data->newCCDData(&Globals::trove->roidata());
    owndata = true;
  }
  connect(source,SIGNAL(dataChanged()), this,SLOT(newData()));
}

Coherence::~Coherence() {
  if (owndata && data) 
    delete data;
}

void Coherence::paintEvent(QPaintEvent *e) {
  CCDImage::paintEvent(e);
  dbg("coherence::paintevent");
  ROISet const *roiset = source->getROIs();
  if (!roiset || !data)
    return;
  
  QPainter p(this);
  ZoomInfo z = makeZoomInfo();
  
  QSet<int> const &ids = roiset->ids();
  for (QSet<int>::const_iterator i=ids.begin(); i!=ids.end(); i++) {
    int id = *i;
    if (id<0)
      continue;
    double x0 = z.ax * roiset->centerX(id) + z.bx;
    double y0 = z.ay * roiset->centerY(id) + z.by;
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
    
    if (roiset->isXYRRA(id)) {
      XYRRA el = roiset->get(id);
      if (hasZoom) {
	el.x0 = z.ax*el.x0 + z.bx;
	el.y0 = z.ay*el.y0 + z.by;
	el.R *= z.ax; // Note: this means
	el.r *= z.ax; // trouble if ax!=ay
      }
      el.paint(&p);
    } else if (roiset->isPoly(id)) {
      PolyBlob const &pb = roiset->getp(id);
      pb.paint(&p, z.ax,z.bx, z.ay,z.by);
    } else {
      // unknown ROI style
    }

    Enumerator *sm = Enumerator::find("SHOWROIS");
    if (showmode==sm->lookup("IDs") ||
	showmode==sm->lookup("Full")) {
      p.setPen(QColor("#000000"));
      p.drawText(QRectF(x0,y0-1,2,2),
		 Qt::AlignCenter | Qt::TextDontClip,
		 num2az(id));
      p.setPen(QColor("#ffffff")); // anti b.g.
      p.drawText(QRectF(x0-1,y0,2,2),
		 Qt::AlignCenter | Qt::TextDontClip,
		 num2az(id));
    }
  }

  p.setPen(QColor("#000000"));
  p.drawText(5,10,QString("f* = %1 Hz").arg(data->getFStarHz(),0,'f',2));
  p.setPen(QColor("#ffffff"));
  p.drawText(4,11,QString("f* = %1 Hz").arg(data->getFStarHz(),0,'f',2));
  
}

void Coherence::showEvent(QShowEvent *e) {
  CCDImage::showEvent(e);
  //  if (outdated) {
  //recalc();
    update();
    //  }
}


void Coherence::newData() {
  dbg("coherence:newdata");
  if (data) {
    data->newTiming(source->getTiming());
    data->newEPhys(source->getAnalog(),source->getDigital());
    data->newCCDData(&Globals::trove->roidata());
  }
  perhapsRefresh();
}

void Coherence::perhapsRefresh() {
  if (data) 
    data->invalidate();
  if (isVisible())
    update();
}

void Coherence::setRefDigi(int digiline) {
  if (data)
    data->setRefDigi(digiline);
  perhapsRefresh();
}

void Coherence::setRefTrace(int ach) {
  if (data)
    data->setRefTrace(ach);
  perhapsRefresh();
}

void Coherence::setRefFreq(double fref_hz) {
  dbg("coherence setRefFreq: %g",fref_hz);
  if (data)
    data->setRefFreq(fref_hz);
  perhapsRefresh();
}

void Coherence::setShowMode(ROIImage::ShowMode sm) {
  showmode = sm;
  update();
} 
