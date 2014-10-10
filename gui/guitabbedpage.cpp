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

void guiTabbedPage::reconnect() {
  guiPage *par = parentPage();
  Dbg() << "guitabbedpage::reconnect " << this << par;

  if (!par)
    return;

  guiButton *penable = buttonp("enable");
  guiRadioGroup *pctrl = par->groupp(id());

  Dbg() << "  guitabbedpage::reconnect " << id() << pctrl;

  if (pctrl) {
    foreach (QString i, pctrl->childIDs()) {
      guiButton *b = par->buttonp(id() + ARRAYSEP + i);
      Dbg() << "   " << i << b;
      if (!b) 
	continue; //throw Exception("guiTabbedPage", "Button " + i + " not found in parent", path());

      connect(b, SIGNAL(selected(QString,QString)),this, SLOT(open(QString)));
      disconnect(b, SIGNAL(selected(QString,QString)),
		 master, SIGNAL(buttonSelected(QString,QString)));
      disconnect(b, SIGNAL(deselected(QString,QString)),
		 master, SIGNAL(buttonDeselected(QString,QString)));
      disconnect(b, SIGNAL(activated(QString,QString)),
		 master, SIGNAL(buttonClicked(QString,QString)));
      connect(b, SIGNAL(selected(QString,QString)),
	      par, SLOT(addTriangle(QString)));
      connect(b, SIGNAL(deselected(QString,QString)),
	      par, SLOT(removeTriangle(QString)));
      if (penable) {
	b->setVisualType(VT_ArrayCtrl);
        b->makeROImmune();
	connect(b, SIGNAL(doubleClicked(QString,QString)),
		penable, SLOT(toggleSelected()));
      }
      connect(b, SIGNAL(doubleClicked(QString,QString)),
	      master, SIGNAL(buttonDoubleClicked(QString,QString)));
    }
  }

  if (penable) {
    connect(penable, SIGNAL(selected(QString,QString)),
	    par, SLOT(childTabEnabled(QString)));
    connect(penable, SIGNAL(deselected(QString,QString)),
	    par, SLOT(childTabEnabled(QString)));
  }
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
