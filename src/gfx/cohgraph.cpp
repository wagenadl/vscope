// gfx/cohgraph.cpp - This file is part of VScope.
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
#include <QMouseEvent>

#define MAXCLICKDIST 15

CohGraph::CohGraph(CohData *dat, QWidget *parent): RadialGraph(parent) {
  data = dat;
  selectedID = 0;  

  connect(data, SIGNAL(newData()), this, SLOT(updateData()));

  QPalette p = palette();
  p.setColor(QPalette::Window, QColor("black"));
  setPalette(p);
  setColors(QColor("#aaaaaa"), QColor("#666666"));
}

CohGraph::~CohGraph() {
}

void CohGraph::paintEvent(QPaintEvent *e) {
  RadialGraph::paintEvent(e);
  if (!data)
    return;
  ROISet const *roiset = data->currentData()->getROISet();
  if (!roiset)
    return;

  QPainter p(this);

  p.setPen(QColor("yellow"));
  for (int deg=0; deg<360; deg+=90) {
    double phi = Numbers::pi()*deg/180;
    double x = toScreenX(cos(phi));
    double y = toScreenX(-sin(phi));
    p.drawText(QRectF(x-1,y-1,2,2),
	       Qt::AlignCenter | Qt::TextDontClip,
	       QString::number(deg));
  }
  QFont f0 = p.font();
  QFont f1 = f0;
  f1.setWeight(QFont::Bold);
  labelxy.clear();
  foreach (int id, roiset->ids()) {
    if (id<=0)
      continue;
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
    double x = toScreenX(mag*cos(pha));
    double y = toScreenY(mag*sin(pha));
    labelxy[id] = QPointF(x,y);
    p.setPen(c); 
    if (id==selectedID) {
      p.setFont(f1);
      p.setPen(QColor("white"));
    }
    p.drawText(QRectF(x-1,y-1,2,2),
	       Qt::AlignCenter | Qt::TextDontClip,
	       num2az(id));
    if (id==selectedID)
      p.setFont(f0);
  }
  
  p.setPen(QColor("white"));
  p.drawText(5,12,QString("f* = %1 Hz").arg(data->getTypicalFStarHz(),0,'f',2));
}

void CohGraph::showEvent(QShowEvent *e) {
  RadialGraph::showEvent(e);
  update();
}


void CohGraph::updateData() {
  perhapsRefresh();
}

void CohGraph::perhapsRefresh() {
  if (data) 
    data->invalidate();
  if (isVisible())
    update();
}

void CohGraph::setRefDigi(QString digiline) {
  if (data)
    data->setRefDigi(digiline);
  perhapsRefresh();
}

void CohGraph::setRefTrace(QString ach) {
  if (data)
    data->setRefTrace(ach);
  perhapsRefresh();
}

void CohGraph::setRefStim(class StimulusDef const &s, bool p) {
  if (data)
    data->setRefStim(s, p);
  perhapsRefresh();
}

void CohGraph::setRefFreq(double fref_hz) {
  if (data)
    data->setRefFreq(fref_hz);
  perhapsRefresh();
}

void CohGraph::updateSelection(int id) {
  selectedID = id;
  update();
}

void CohGraph::mousePressEvent(QMouseEvent *e) {
  QPointF xy0 = e->pos();
  double d0 = 1e9;
  int id0 = 0;
  foreach (int id, labelxy.keys()) {
    QPointF xy = labelxy[id];
    double dx = xy.x()-xy0.x();
    double dy = xy.y()-xy0.y();
    double dd = dx*dx + dy*dy;
    if (dd<d0) {
      id0 = id;
      d0 = dd;
    }
  }
  if (d0>MAXCLICKDIST*MAXCLICKDIST)
    id0 = 0;
  updateSelection(id0);
  newSelection(id0);
}
    
      
  
