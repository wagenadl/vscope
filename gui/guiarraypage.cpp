// guiarraypage.cpp

#include "guiarraypage.h"
#include "guiradiogroup.h"
#include <base/exception.h>
#include "guiroot.h"
#include <base/dbg.h>

guiArrayPage::guiArrayPage(class QWidget *parent,
		 class ParamTree *ptree,
		 QString id,
		 class guiRoot *master,
		 class QRect const &geom):
  guiPage(parent, ptree, id, master, geom) {
  currentElement = "";
}

void guiArrayPage::connectToParent(QDomElement doc) {
  guiPage::connectToParent(doc);
  reconnect();
}

void guiArrayPage::reconnect() {
  guiPage *par = parentPage();
  if (!par)
    return;

  guiButton *penable = buttonp("enable");
  guiRadioGroup *pctrl = par->groupp(id());


  if (pctrl) {
    foreach (QString i, pctrl->childIDs()) {
      guiButton *b = par->buttonp(id() + ":" + i);
      if (!b) 
	continue; //throw Exception("guiArrayPage", "Button " + i + " not found in parent", path());

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

guiArrayPage::~guiArrayPage() {
}

void guiArrayPage::open(QString p) {
  int idx = p.lastIndexOf('/');
  QString elt = p.mid(idx+1);
  idx = elt.indexOf(":");
  if (idx<0)
    throw Exception("guiArrayPage", "Path does not specify element: " + p);
  QString ar = elt.left(idx);
  elt = elt.mid(idx+1);
  currentElement = elt;
  retree();
  open();
}

void guiArrayPage::retree() {
  setTree(0);
}

void guiArrayPage::open() {
  guiPage::open();
}

QString guiArrayPage::getCurrentElement() const {
  return currentElement;
}
