// toplevel/gt_slots.cpp - This file is part of VScope.
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

// gt_slots.cpp

#include "gt_slots.h"
#include <QApplication>
#include <gui/guiroot.h>
#include <base/dbg.h>
#include <gui/guiexc.h>
#include <toplevel/globals.h>

gt_slots::gt_slots(guiRoot *master): guiroot(master) {
  QObject::connect(master, SIGNAL(buttonClicked(QString,QString)),
		   this, SLOT(clicked(QString)));
  QObject::connect(master, SIGNAL(buttonDoubleClicked(QString,QString)),
		   this, SLOT(doubleClicked(QString)));
  QObject::connect(master, SIGNAL(buttonSelected(QString,QString)),
		   this, SLOT(selected(QString)));
  QObject::connect(master, SIGNAL(buttonDeselected(QString,QString)),
		   this, SLOT(deselected(QString)));
  QObject::connect(master, SIGNAL(paramChanged(QString,QString)),
		   this, SLOT(paramchanged(QString,QString)));
  QObject::connect(master, SIGNAL(customValueChanged(QString,int,QString)),
		   this, SLOT(customchanged(QString,int,QString)));
  QObject::connect(master, SIGNAL(pageOpened(QString,QWidget*)),
		   this, SLOT(pgopen(QString,QWidget*)));
  QObject::connect(master, SIGNAL(pageClosed(QString,QWidget*)),
		   this, SLOT(pgclose(QString,QWidget*)));
}

gt_slots::~gt_slots() {
}

void gt_slots::customchanged(QString p, int cno, QString val) {
  dbg("customchanged %s (%i) -> %s\n",qPrintable(p),cno,qPrintable(val));
}

void gt_slots::report(Exception const &e, QString act) {
  GUIExc::report(e,act);
}

ParamTree *gt_slots::ptree() {
  return &guiroot->paramTree();
}
