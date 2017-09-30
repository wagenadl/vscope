// gfx/hscrollbar.h - This file is part of VScope.
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

// hscrollbar.h

#ifndef HSCROLLBAR_H

#define HSCROLLBAR_H

#include <QFrame>

class HScrollBar: public QFrame {
  Q_OBJECT;
public:
  HScrollBar(QWidget *parent);
  virtual ~HScrollBar();
  void setRange(double x0, double w);
  void setSlider(double x0, double w, bool constrain=false);
signals:
  void moved(double x0);
  void moving(double x0);
protected:
  virtual void paintEvent(class QPaintEvent *);
  virtual void mousePressEvent(class QMouseEvent *);
  virtual void mouseMoveEvent(class QMouseEvent *);
  virtual void mouseReleaseEvent(class QMouseEvent *);
private:
  double x0_range;
  double w_range;
  double x0_slider;
  double w_slider;
  double x0_dragstart;
  double x0_dragcurrent;
  int xs0_dragstart;
  int xs0_dragcurrent;
  bool dragging;
};

#endif
