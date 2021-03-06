// gfx/coherence.cpp - This file is part of VScope.
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
#include <QRubberBand>

Coherence::Coherence(CohData *dat, QWidget *p): CCDImage(p) {
  data = dat;
  connect(data, SIGNAL(newData()), SLOT(updateData()));
  selectedID=0;
}

Coherence::~Coherence() {
}

void Coherence::paintEvent(QPaintEvent *e) {
  CCDImage::paintEvent(e);
  if (!data)
    return;
  ROISet const *roiset = data->currentData()->getROISet();
  if (!roiset)
    return;
  
  QPainter p(this);

  foreach (int id, roiset->ids()) {
    if (id<=0)
      continue; // this should not happen
    if (roiset->cam(id) != cp)
      continue; // only current camera
    ROICoords const &roi = roiset->get(id);
    QPointF xy = canvasToScreen()(roi.center());
    double pha = data->phase(id);
    double mag = data->magnitude(id);
    if (Numbers::isNaN(mag)) 
      mag = pha = 0;
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

    if (showmode==SR_IDs || showmode==SR_Full) {
      if (id==selectedID)
	p.setPen(QColor("white"));
      else
	p.setPen(QColor("black"));
      p.drawText(QRectF(xy.x(),xy.y()-1,2,2),
		 Qt::AlignCenter | Qt::TextDontClip,
		 num2az(id));
      if (id==selectedID)
	p.setPen(QColor("black"));
      else
	p.setPen(QColor("white")); // anti b.g.
      p.drawText(QRectF(xy.x()-1,xy.y(),2,2),
		 Qt::AlignCenter | Qt::TextDontClip,
		 num2az(id));
    }
  }

  p.setPen(QColor("black"));
  double fstar = data->getTypicalFStarHz();
  p.drawText(5,10,QString("f* = %1 Hz").arg(fstar,0,'f',2));
  p.setPen(QColor("white"));
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
  perhapsRefresh();
}

void Coherence::perhapsRefresh() {
  if (data) 
    data->invalidate();
  if (isVisible())
    update();
}

void Coherence::setRefDigi(QString digiline) {
  if (data)
    data->setRefDigi(digiline);
  perhapsRefresh();
}

void Coherence::setRefTrace(QString ach) {
  if (data)
    data->setRefTrace(ach);
  perhapsRefresh();
}

void Coherence::setRefStim(StimulusDef const &s, bool p) {
  if (data)
    data->setRefStim(s, p);
  perhapsRefresh();
}

void Coherence::setRefFreq(double fref_hz) {
  if (data)
    data->setRefFreq(fref_hz);
  perhapsRefresh();
}

void Coherence::setShowMode(SHOWROIS sm) {
  showmode = sm;
  update();
} 

void Coherence::mouseReleaseEvent(QMouseEvent *e) {
  if (clickPoint.x()<0)
    return;  // this is the release of a double click; we do not care.
  QRect r = rubberband->geometry().normalized();
  rubberband->hide();
  if (r.width(),5 && r.height()<5) {
    // This is a click. If inside a ROI, let's select it.
    int id = insideROI(clickPoint);
    if (id>0)
      select(id);
    else
      CCDImage::mouseReleaseEvent(e);
  } else {
    CCDImage::mouseReleaseEvent(e);
  }
}

inline double sq(double x) { return x*x; }

inline double euclideanDist2(QPointF p1, QPointF p2) {
  return sq(p1.x()-p2.x()) + sq(p1.y()-p2.y());
}

int Coherence::insideROI(QPoint xy) {
  ROISet const *roiset = data->currentData()->getROISet();
  if (!roiset)
    return false;
  // following code copied from roiimage.
  Transform tinv = canvasToScreen().inverse();
  QPointF xy_ = tinv(Transform::pixelCenter(xy));
  int bestid=0;
  double dd = Numbers::inf();
  foreach (int id, roiset->ids()) {
    double d = euclideanDist2(xy_, roiset->get(id).center());
    if (bestid==0 || d<dd) {
      bestid = id;
      dd = d;
    }
  }
  if (bestid) 
    if (roiset->get(bestid).inside(xy_, 0))
      return bestid;
  return 0;
}

void Coherence::select(int id) {
  selectedID = id;
  emit newSelection(selectedID);
  update();
}


void Coherence::updateSelection(int id) {
  selectedID = id;
  update();
}

void Coherence::setCamPair(CamPair const &c) {
  cp = c;
  update();
}
