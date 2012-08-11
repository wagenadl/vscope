// guichecklist.cpp

#include "guichecklist.h"
#include "guichecklistitem.h"
#include <xml/paramtree.h>
#include <base/exception.h>
#include <base/istype.h>
#include <xml/enumerator.h>
#include <stdio.h>
#include "autoitems.h"
#include <base/dbg.h>

guiChecklist::guiChecklist(class QWidget *parent,
		 class ParamTree *ptree,
		 QString id,
		 class guiRoot *master,
		 class QRect const &geom):
  guiMenu(parent, ptree, id, master, geom) {
  setNonExclusive();
}

guiChecklist::~guiChecklist() {
}

guiItem *guiChecklist::createItem(QString id) {
  return new guiChecklistItem(this, id, master);
}

void guiChecklist::prepForOpening() {
  Param *pp = ptree->leafp();
  if (!pp || pp->getType()!="set") 
    throw Exception("guiCheckList",
		    "openSelf failed because I am not a leaf of type set");

  if (autoItems)
    autoItems->rebuild();
  
  QBitArray ba = pp->toBitArray();
  foreach (guiButton *b, buttons) {
    if (isType<guiItem>(b)) {
      Enumerator const *e = pp->getEnum();
      try {
	int idx = e->lookup(b->getValue());
	if (idx>=0)
	  b->setSelected(ba.testBit(idx));
	else
	  Dbg() << "guiCheckList: negative idx in " << b->id();
      } catch (Exception) {
	fprintf(stderr, "Exception ignored.\n");
      }	  
    }
  }
}

void guiChecklist::connectToParent(QDomElement doc) {
  guiMenu::connectToParent(doc);
  guiButton *b = parentPage()->buttonp(id());
  if (b)
    b->dropValueFromLabel();
}


