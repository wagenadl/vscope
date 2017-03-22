// gfx/scrollbuttonbar.cpp - This file is part of VScope.
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

// scrollbuttonbar.cpp

#include "scrollbuttonbar.h"
#include <gfx/button.h>
#include <base/exception.h>
#include <base/dbg.h>

#define MINWIDTH 40
#define PADDING 5
#define MARGIN 2
#define SPACING 4

ScrollButtonBar::ScrollButtonBar(QWidget *parent): QFrame(parent) {
  fg = QColor("black");
  bg = QColor("#eeeeee");
}

ScrollButtonBar::~ScrollButtonBar() {
}

void ScrollButtonBar::clear() {
  for (QVector<Button*>::iterator i=buttons.begin(); i!=buttons.end(); ++i)
    (*i)->deleteLater();
  buttons.clear();
}

void ScrollButtonBar::setAllLabels(QStringList const &labels) {
  clear(); // I'm lazy
  for (QStringList::const_iterator i=labels.begin();
       i!=labels.end(); ++i)
    addButton(*i);
}

void ScrollButtonBar::addButton(QString label) {
  Button *b = new Button(this);
  b->setText(label);
  b->setForeground(fg);
  b->setBackground(bg);
  connect(b,SIGNAL(textChanged(QString,QString)),
	  this,SLOT(recvTextChange(QString,QString)));
  connect(b,SIGNAL(activated(QString,QString)),
	  this,SLOT(recvClick(QString,QString)));
  b->setID(QString::number(buttons.size()));
  if (isVisible())
    b->show();
  buttons.push_back(b);
  relayout();
}

void ScrollButtonBar::setButtonLabel(int i, QString label) {
  if (i<0)
    throw Exception("ScrollButtonBar","Index must be non-negative","setButtonLabel");
  while (i>=buttons.size())
    addButton("-");
  buttons[i]->setText(label);
}

void ScrollButtonBar::setButtonBackground(QColor const &bg0) {
  bg = bg0;
  for (QVector<Button*>::iterator i=buttons.begin(); i!=buttons.end(); ++i)
    (*i)->setBackground(bg);
}

void ScrollButtonBar::setButtonForeground(QColor const &fg0) {
  fg = fg0;
  for (QVector<Button*>::iterator i=buttons.begin(); i!=buttons.end(); ++i)
    (*i)->setForeground(fg);
}

void ScrollButtonBar::resizeEvent(class QResizeEvent *) {
  relayout();
}

void ScrollButtonBar::relayout() {
  int N = buttons.size();
  if (N==0)
    return;
  
  QVector<int> widths;
  int neededwidth = 2*MARGIN - SPACING;
  for (QVector<Button*>::iterator i=buttons.begin(); i!=buttons.end(); ++i) {
    Button *b = *i;
    QFontMetrics fm(b->font());
    QRect bb(fm.boundingRect(b->text()));
    int w = bb.width();
    w+=2*PADDING;
    if (w<MINWIDTH)
      w=MINWIDTH;
    widths.push_back(w);
    neededwidth += SPACING + w;
  }
  int x0_easy = MARGIN; // to be used if we have enough space
  int x0_hard = width()-MARGIN - neededwidth; // to be used if not
  int x0 = (x0_hard<x0_easy) ? x0_hard : x0_easy;
  int h = height();

  for (int i=0; i<N; i++) {
    buttons[i]->setGeometry(x0,0,widths[i],h);
    x0 += widths[i] + SPACING;
  }
}

void ScrollButtonBar::recvClick(QString id, QString txt) {
  int idx = id.toInt(0);
  dbg("emitting clicked, idx=%i txt=%s path=%s",
      idx,qPrintable(txt),qPrintable(getAsPath(idx)));
  emit clicked(idx, txt);
}

void ScrollButtonBar::recvTextChange(QString, QString) {
  relayout();
}

void ScrollButtonBar::setAsPath(QString path) {
  QStringList lst = path.split("/",QString::SkipEmptyParts);
  for (QStringList::iterator i=lst.begin(); i!=lst.end(); ++i)
    *i += "/";
  lst.push_front("/");
  setAllLabels(lst);
}

QString ScrollButtonBar::getAsPath(int idx) {
  QString r="";
  for (int i=1; i<=idx; i++) {
    QString t = buttons[i]->text();
    r+="/";
    r+=t;
  }
  r.replace("//","/");
  if (r.endsWith("/"))
    r = r.left(r.size()-1);
  if (r.isEmpty())
#ifdef vsdWIN32
    r="c:/";
#else
    r="/";
#endif
  return r;
}
