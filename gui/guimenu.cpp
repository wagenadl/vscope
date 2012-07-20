// guimenu.cpp

#include "guimenu.h"
#include "guimenuitem.h"
#include <xml/paramtree.h>
#include <base/exception.h>
#include "guiroot.h"
#include <base/istype.h>
#include <stdio.h>
#include <xml/enumerator.h>

guiMenu::guiMenu(class QWidget *parent,
		 class ParamTree *ptree,
		 QString id,
		 class guiRoot *master,
		 class QRect const &geom):
  guiPage(parent, ptree, id, master, geom) {
}

void guiMenu::setup(QDomElement doc) {
  guiPage::setup(doc);
}

guiMenu::~guiMenu() {
}

Button::VisualType guiMenu::visualTypeForParentButton() const {
  return Button::VTVarOpen;
}

void guiMenu::connectToParent(QDomElement doc) {
  guiPage::connectToParent(doc);
  guiButton *b = parentPage()->buttonp(id());
  if (b)
    b->ensureValueInLabel();
}

guiItem *guiMenu::createItem(QString id) {
  return new guiMenuItem(this, id, master);
}

void guiMenu::prepForOpening() {
  Param *pp = ptree->leafp();
  if (!pp)
    throw Exception("guiMenu", "openSelf failed because paramtree has no leaf");
  
  Param copy(*pp);
  bool valuefound = false;
  foreach (guiButton *b, buttons) {    
    if (b->isCustom()) {
      Param const *custpar = master->paramTree().
	findp(QString("custom/%1-%3")
	      .arg(pathDeinstantiate(myPath))
	      .arg(b->customNo()));
      if (custpar) {
	b->setValue(custpar->toString());
      } else {
	fprintf(stderr,"guiPage(%s): Warning: no value for custom %i\n",
		qPrintable(myPath), b->customNo());
      }
    }
    try {
      copy.set(b->getValue());
      bool match = copy==*pp;
      b->setSelected(match);
      if (match)
	valuefound=true;
      
      b->setEnabled(master->canSetParam(master->pathInstantiate(myPath),
					b->getValue()));
    } catch (Exception) {
      if (isType<guiItem>(b))
	b->setEnabled(false);
    }
  }
  if (!valuefound) {
    // try to locate a custom button
    foreach (guiButton *b, buttons) {
      if (b->isCustom()) {
	b->setValue(pp->toString());
	b->setSelected(true);
	break;
      }
    }
  }
}
  
