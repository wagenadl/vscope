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
#include <base/roidata3set.h>
#include <xml/connections.h>

Coherence::Coherence(CohData *dat, QWidget *p): CCDImage(p) {
  if (dat) {
    data = dat;
    owndata = false;
  } else {
    data = new CohData();
    data->setROISet(&Globals::trove->rois());
    data->setCCDData(&Globals::trove->roidata());
    data->setEPhys(Globals::trove->trial().analogData(),
		   Globals::trove->trial().digitalData());
    Enumerator const *digilines = Enumerator::find("DIGILINES");
    foreach (QString cam, Connections::allCams()) {
      if (digilines->has("Frame"+cam))
	data->setFrameLine(cam, digilines->lookup("Frame"+cam));
    }
    owndata = true;
  }
  connect(data, SIGNAL(newData()), SLOT(updateData()));
}

Coherence::~Coherence() {
  if (owndata && data) 
    delete data;
}

void Coherence::paintEvent(QPaintEvent *e) {
  CCDImage::paintEvent(e);
  dbg("coherence::paintevent");
  if (!data)
    return;
  ROISet const *roiset = data->currentData()->getROISet();
  if (!roiset)
    return;
  
  QPainter p(this);

  foreach (int id, roiset->ids()) {
    if (id<=0)
      continue; // this should not happen
    ROICoords const &roi = roiset->get(id);
    QPointF xy = canvasToScreen()(roi.center());
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
    
    if (roi.isXYRRA()) 
      roi.xyrra().transformed(canvasToScreen()).paint(&p);
    else if (roi.isBlob())
	roi.blob().paint(&p, canvasToScreen());

    if (showmode==ROIImage::SM_IDs || showmode==ROIImage::SM_Full) {
      p.setPen(QColor("black"));
      p.drawText(QRectF(xy.x(),xy.y()-1,2,2),
		 Qt::AlignCenter | Qt::TextDontClip,
		 num2az(id));
      p.setPen(QColor("white")); // anti b.g.
      p.drawText(QRectF(xy.x()-1,xy.y(),2,2),
		 Qt::AlignCenter | Qt::TextDontClip,
		 num2az(id));
    }
  }

  p.setPen(QColor("#000000"));
  double fstar = data->getTypicalFStarHz();
  p.drawText(5,10,QString("f* = %1 Hz").arg(fstar,0,'f',2));
  p.setPen(QColor("#ffffff"));
  p.drawText(4,11,QString("f* = %1 Hz").arg(fstar,0,'f',2));
  
}

void Coherence::showEvent(QShowEvent *e) {
  CCDImage::showEvent(e);
  //  if (outdated) {
  //recalc();
    update();
    //  }
}

void Coherence::updateData() {
  dbg("coherence:newdata");
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
