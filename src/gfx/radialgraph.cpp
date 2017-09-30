// gfx/radialgraph.cpp - This file is part of VScope.
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

// radialgraph.cpp

#include "radialgraph.h"

#include <QPainter>
#include <QPaintEvent>

RadialGraph::RadialGraph(QWidget *parent): QFrame(parent) {
  setAutoFillBackground(true);
  QPalette p = palette();
  p.setColor(QPalette::Window, QColor("white"));
  setPalette(p);
  hMargin = vMargin = 10;
  minorColor = QColor("#aaaaaa");
  majorColor = QColor("#666666");
}

double RadialGraph::toScreenX(double x) const {
  double w = contentsRect().width()/2-hMargin;
  return contentsRect().left() + hMargin + w + w*x;
}


double RadialGraph::toScreenY(double y) const {
  double h = contentsRect().height()/2-vMargin;
  return contentsRect().top() + vMargin + h - h*y;
}

QPointF RadialGraph::toScreen(double x, double y) const {
  return QPointF(toScreenX(x), toScreenY(y));
}  

void RadialGraph::setColors(QColor major, QColor minor) {
  majorColor = major;
  minorColor = minor;
  update();
}

void RadialGraph::paintEvent(QPaintEvent *e) {
  QFrame::paintEvent(e);
  QPainter p(this);
  p.setPen(minorColor);
  for (double r = 0.25; r<=1.1; r+=0.25) {
    if (r>.8)
      p.setPen(majorColor);
    p.drawEllipse(toScreen(0,0),
		  toScreenX(r)-toScreenX(0),
		  toScreenY(r)-toScreenY(0));
  }

  p.drawLine(toScreen(-1.,0),toScreen(1.,0));
  p.drawLine(toScreen(0,-1.),toScreen(0,1.));
}
