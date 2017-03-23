// gfx/pathbar.cpp - This file is part of VScope.
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

// pathbar.cpp

#include "pathbar.h"
#include <gfx/button.h>
#include <base/dbg.h>
#include <QHBoxLayout>

PathBar::PathBar(QWidget *parent): QScrollArea(parent) {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  area = new QWidget();
  layout = new QHBoxLayout(area);
  layout->setMargin(2);
  layout->setSpacing(4);
  layout->addStretch(20);
  //  area->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed));
  
  setWidget(area);
}

PathBar::~PathBar() {
}

void PathBar::validateArea() {
  return;
  dbg("validate hint %i %i",area->sizeHint().width(),area->sizeHint().height());
  if (asizeValid)
    return;
  if (elements.size())
    area->resize(area->width()+2-elements[0]->geometry().left(),
		 height());
  else
    area->resize(width(),height());
  int w = area->geometry().width();
  ensureVisible(w-1,1,1,1);
  asizeValid = true;
}
  

void PathBar::resizeEvent(class QResizeEvent *e) {
  QScrollArea::resizeEvent(e);
  if (isVisible())
    validateArea();
  dbg("resize hint %i %i",area->sizeHint().width(),area->sizeHint().height());
}

void PathBar::setPath(QString path) {
  QStringList elts = path.split("/", QString::SkipEmptyParts);
  elts.push_front("/");
  int N0 = elements.size();
  int N = elts.size();
  for (int n=0; n<N; n++) {
    if (n>=N0) 
      elements.push_back(new Button(area,0,0,QString::number(n)));
    Button *b = elements[n];
    b->setText(elts[n]);
    if (n>=N0) {
      layout->addWidget(b);
      connect(b,SIGNAL(activated(QString,QString)),
	      this,SLOT(elementClicked(QString)));
    }
  }
  if (N<N0) {
    for (int n=N; n<N0; n++) 
      delete elements[n];
    elements.remove(N,N0-N);
  }
  area->resize(10000,area->sizeHint().height());
  dbg("newpath hint %i %i",area->sizeHint().width(),area->sizeHint().height());
  int w=0;
  for (int n=0; n<N; n++) {
    int dw=elements[n]->sizeHint().width();
    dbg("  hint %i: %i",n,dw);
    w+=dw+2;
  }
  area->resize(w,height());
  ensureVisible(w-1,1,1,1);
  //  asizeValid = false;
}

void PathBar::paintEvent(class QPaintEvent *e) {
  QScrollArea::paintEvent(e);
  dbg("paintevent");
  if (elements.size() && elements[0]->geometry().left()>2)
    asizeValid = false;
  validateArea();
}

void PathBar::elementClicked(QString id) {
  int N = id.toInt(0);
  QString path="";
  if (N==0) {
    path="/";
  } else {
    for (int n=1; n<=N; n++) {
      path+="/";
      path+=elements[n]->text();
    }
  }
  emit pathSelected(path);
  dbg("path selected: %s",qPrintable(path));
}
