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

  QSet<QString> ss = pp->toStrings();

  Dbg() << "prepforopening:" << path() << ":" << ptree << ":" << pp
        << ": " << QStringList(ss.toList()).join(", ");
  
  foreach (guiButton *b, buttons) 
    if (isType<guiItem>(b)) 
      b->setSelected(ss.contains(b->getValue()));
}

void guiChecklist::connectToParent(QDomElement doc) {
  guiMenu::connectToParent(doc);
  guiButton *b = parentPage()->buttonp(id());
  if (b)
    b->dropValueFromLabel();
}


