// gui/guitabbedpage.cpp - This file is part of VScope.
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

// guitabbedpage.cpp

#include "guitabbedpage.h"
#include "guiradiogroup.h"
#include <base/exception.h>
#include "guiroot.h"
#include <base/dbg.h>

guiTabbedPage::guiTabbedPage(class QWidget *parent,
		 class ParamTree *ptree,
		 QString id,
		 class guiRoot *master,
		 class QRect const &geom):
  guiPage(parent, ptree, id, master, geom) {
  currentElement = "";
}

void guiTabbedPage::connectToParent(QDomElement doc) {
  guiPage::connectToParent(doc);
  reconnect();
}

#define uniqueConnect(src, sig, dst, slt) { disconnect(src, sig, dst, slt); connect(src, sig, dst, slt); }

void guiTabbedPage::reconnect() {
  guiPage *par = parentPage();

  if (!par)
    return;

  guiButton *penable = buttonp("enable");
  guiRadioGroup *pctrl = par->groupp(id());

  if (pctrl) {
    foreach (QString i, pctrl->childIDs()) {
      guiButton *b = par->buttonp(id() + ARRAYSEP + i);
      if (!b) 
	continue; //throw Exception("guiTabbedPage", "Button " + i + " not found in parent", path());

      uniqueConnect(b, SIGNAL(selected(QString,QString)),
		    this, SLOT(open(QString)));
      disconnect(b, SIGNAL(selected(QString,QString)),
		 master, SIGNAL(buttonSelected(QString,QString)));
      disconnect(b, SIGNAL(deselected(QString,QString)),
		 master, SIGNAL(buttonDeselected(QString,QString)));
      disconnect(b, SIGNAL(activated(QString,QString)),
		 master, SIGNAL(buttonClicked(QString,QString)));
      uniqueConnect(b, SIGNAL(selected(QString,QString)),
		    par, SLOT(addTriangle(QString)));
      uniqueConnect(b, SIGNAL(deselected(QString,QString)),
		    par, SLOT(removeTriangle(QString)));
      if (penable) {
	b->setVisualType(VT_ArrayCtrl);
        b->makeROImmune();
	uniqueConnect(b, SIGNAL(doubleClicked(QString,QString)),
		      penable, SLOT(toggleSelected()));
      }
      uniqueConnect(b, SIGNAL(doubleClicked(QString,QString)),
		    master, SIGNAL(buttonDoubleClicked(QString,QString)));
    }
  }

  if (penable) {
    uniqueConnect(penable, SIGNAL(selected(QString,QString)),
		  par, SLOT(childTabEnabled(QString)));
    uniqueConnect(penable, SIGNAL(deselected(QString,QString)),
		  par, SLOT(childTabEnabled(QString)))
  }
  prepForOpening();
}

guiTabbedPage::~guiTabbedPage() {
}

void guiTabbedPage::open(QString p) {
  int idx = p.lastIndexOf('/');
  QString elt = p.mid(idx+1);
  idx = elt.indexOf(ARRAYSEP);
  if (idx<0)
    throw Exception("guiTabbedPage", "Path does not specify element: " + p);
  QString ar = elt.left(idx);
  elt = elt.mid(idx+1);
  currentElement = elt;
  reTree(0);
  open();
}

void guiTabbedPage::open() {
  guiPage::open();
}

QString guiTabbedPage::getCurrentElement() const {
  return currentElement;
}
