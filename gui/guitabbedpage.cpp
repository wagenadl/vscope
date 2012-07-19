// guitabbedpage.cpp

#include "guitabbedpage.h"
#include "guibuttongroup.h"
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
  
  guiPage *par = dynamic_cast<guiPage *>(parent());
  if (!par)
    return;

  guiButton *penable = buttonp("enable");
  guiButtonGroup *pgroup = par->groupp(id());

  Dbg() << "guiTabbedPage id=" << id() << " penable=" << penable << " pgroup=" << pgroup;

  if (pgroup) {
    foreach (QString i, pgroup->childIDs()) {
      guiButton *b = par->buttonp(i);
      if (!b) 
	throw Exception("guiTabbedPage", "Button not found in parent");

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
      b->setVisualType(Button::VTArrayCtrl);
      if (penable)
	connect(b, SIGNAL(doubleClicked(QString,QString)),
		penable, SLOT(toggleSelected()));
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
  idx = elt.indexOf('*');
  QString ar = (idx>=0) ? elt.left(idx) : "";
  elt = elt.mid(idx+1);
  currentElement = elt;

  reTree(0);
  guiPage::open();
}

QString guiTabbedPage::getCurrentElement() const {
  return currentElement;
}
