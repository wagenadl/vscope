// gui/timebutton.cpp - This file is part of VScope.
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

// timebutton.cpp

#include "timebutton.h"
#include <gui/guibutton.h>
#include <base/dbg.h>

TimeButton::TimeButton(QObject *parent, guiButton *button):
  QObject(parent), button(button) {
  style = None;
}

TimeButton::~TimeButton() {
}

void TimeButton::startCountUp(bool onlyifstopped) {
  if (onlyifstopped && style!=None)
    return;
  time.start();
  style = Up;
  relabel();
}

void TimeButton::startCountDown(double time_ms) {
  time.start();
  time.addMSecs(time_ms);
  style = Down;
  relabel();
}

void TimeButton::showWallTime() {
  style = Wall;
  relabel();
}

void TimeButton::reset() {
  style = None;
}

void TimeButton::timerEvent(class QTimerEvent *e) {
  //  dbg("timebutton::event. elapsed=%i",time.elapsed());
  QObject::timerEvent(e);
  relabel();
}

void TimeButton::show() {
  relabel();
  timer.start(1000,this);
  //  dbg("timebutton::show");
}

void TimeButton::hide() {
  timer.stop();
  //  dbg("timebutton::hide");
}

void TimeButton::setFormat(QString f) {
  button->setFormat(f);
}

void TimeButton::relabel() {
  switch (style) {
  case None:
    button->setValue("--:--");
    break;
  case Wall:
    button->setValue(QTime::currentTime().toString("hh:mm:ss"));
    break;
  case Up: case Down: {
    int ms = time.elapsed();
    int s = ms/1000;
    bool isNegative = style==Down ? (s>0) : (s<0);
    if (s<0)
      s=-s;
    int m = s/60;
    s -= m*60;
    int h = m/60;
    m -= h*60;
    QString t;
    if (isNegative)
      t="-";
    if (h>0)
      t += QString("%1").arg(h);
    t += QString("%1:%2").arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0'));
    button->setValue(t);
  }
  }
}
