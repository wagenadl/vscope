// gfx/blackout.cpp - This file is part of VScope.
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

// blackout.cpp

#include "blackout.h"
#include <QTimer>

Blackout::Blackout(QWidget *parent): QLabel(parent) {
  timer = new QTimer(this);
  setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
  QPalette p=palette();
  p.setColor(QPalette::Window,QColor("black"));
  p.setColor(QPalette::WindowText,QColor("red"));
  setPalette(p);
  setAutoFillBackground(true);
  setGeometry(parent ? parent->rect() : QRect(0,0,1024,768));
  connect(timer, SIGNAL(timeout()), SLOT(timeout()));
}

Blackout::~Blackout() {
}

void Blackout::setText(QString s) {
  txt = s;
  timer->stop();
  QLabel::setText(s);
}

void Blackout::countUp() {
  t0.start();
  timer->start(0.1);
}

void Blackout::timeout() {
  QLabel::setText(txt + QString(" %1").arg(t0.elapsed()/1e3, 0, 'f', 3));
}
