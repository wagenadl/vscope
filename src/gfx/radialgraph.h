// gfx/radialgraph.h - This file is part of VScope.
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

// radialgraph.h

#ifndef RADIALGRAPH_H

#define RADIALGRAPH_H

#include <QWidget>
#include <QFrame>
#include <QPointF>

class RadialGraph: public QFrame {
  Q_OBJECT;
public:
  RadialGraph(QWidget *parent=0);
  virtual void paintEvent(class QPaintEvent *);
  void setColors(QColor major, QColor minor);
public:
  QPointF toScreen(double x, double y) const;
  double toScreenX(double x) const;
  double toScreenY(double x) const;
private:
  int hMargin;
  int vMargin;
  QColor majorColor;
  QColor minorColor;
};

#endif
