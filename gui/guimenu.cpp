// guimenu.cpp

#include "guimenu.h"
#include "guimenuitem.h"
#include <xml/paramtree.h>
#include <base/exception.h>
#include "guiroot.h"
#include <base/istype.h>
#include <stdio.h>
#include <xml/enumerator.h>
#include "autoitems.h"

guiMenu::guiMenu(class QWidget *parent,
		 class ParamTree *ptree,
		 QString id,
		 class guiRoot *master,
		 class QRect const &geom):
  guiPage(parent, ptree, id, master, geom) {
  autoItems = 0;
}

guiMenu::~guiMenu() {
}

VISUALTYPE guiMenu::visualTypeForParentButton() const {
  return VT_VarOpen;
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
  Param *pp = ptree ? ptree->leafp() : 0;
  if (!pp)
    throw Exception("guiMenu", "openSelf failed because paramtree has no leaf");
  
  if (autoItems)
    autoItems->rebuild();

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

void guiMenu::addAuto(PageBuildGeom &g, QDomElement doc) {
  if (autoItems)
    throw Exception("guiMenu", "Can have only one <auto> item: " + path());

  autoItems = new AutoItems(this);
  autoItems->setup(g, doc);
}

bool guiMenu::mayResize() {
  return autoItems==0 || !autoItems->isDynamic();
}

void guiMenu::updateAuto() {
  if (autoItems)
    autoItems->rebuild();
}
