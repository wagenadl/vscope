// gui/guichecklist.cpp - This file is part of VScope.
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

  if (autoItems && !overridden)
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


