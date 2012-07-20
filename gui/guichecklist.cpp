// guichecklist.cpp

#include "guichecklist.h"
#include "guichecklistitem.h"
#include <xml/paramtree.h>
#include <base/exception.h>
#include <base/istype.h>
#include <xml/enumerator.h>
#include <stdio.h>

guiChecklist::guiChecklist(class QWidget *parent,
		 class ParamTree *ptree,
		 QString id,
		 class guiRoot *master,
		 class QRect const &geom):
  guiPage(parent, ptree, id, master, geom) {
}

void guiChecklist::setup(QDomElement doc) {
  guiPage::setup(doc);
}

guiChecklist::~guiChecklist() {
}

Button::VisualType guiChecklist::visualTypeForParentButton() const {
  return Button::VTVarOpen;
}

guiItem *guiChecklist::createItem(QString id) {
  return new guiChecklistItem(this, id, master);
}

void guiChecklist::prepForOpening() {
  Param *pp = ptree->leafp();
  if (!pp || pp->getType()!="set") 
    throw Exception("guiCheckList",
		    "openSelf failed because I am not a leaf of type set");
  
  QBitArray ba = pp->toBitArray();
  foreach (guiButton *b, buttons) {
    if (isType<guiItem>(b)) {
      Enumerator const *e=pp->getEnum();
      try {
	int idx=e->lookup(b->getValue());
	b->setSelected(ba.testBit(idx));
      } catch (Exception) {
	fprintf(stderr, "Exception ignored.\n");
      }	  
    }
  }
}
