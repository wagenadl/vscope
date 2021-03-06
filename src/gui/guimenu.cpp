// gui/guimenu.cpp - This file is part of VScope.
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
#include <gfx/radiogroup.h>
#include <base/dbg.h>

guiMenu::guiMenu(class QWidget *parent,
		 class ParamTree *ptree,
		 QString id,
		 class guiRoot *master,
		 class QRect const &geom):
  guiPage(parent, ptree, id, master, geom) {
  autoItems = 0;
  overridden = false;
  itemgroup = new RadioGroup(this);
}

guiMenu::~guiMenu() {
}

void guiMenu::setNonExclusive() {
  if (itemgroup)
    delete itemgroup;
  itemgroup = 0;
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

static QString itemID(QDomElement doc) {
  if (doc.hasAttribute("id"))
    return doc.attribute("id");
  else if (doc.hasAttribute("custom"))
    return "custom-"+doc.attribute("custom");
  else if (doc.hasAttribute("value"))
    return doc.attribute("value");
  else
    return "";
}
  

guiButton *guiMenu::addItem(PageBuildGeom &g, QDomElement doc) {
  QString id = itemID(doc);
  if (id.isEmpty()) 
    throw Exception("guiPage", "Empty item ID in page " + path());
  
  guiButton *b = createItem(id);
  Param *p = ptree ? ptree->leafp() : 0;
  if (p && p->isImmune())
    b->makeROImmune();
  buttons[id] = b;
  buttons[id]->setup(doc);
  if (itemgroup)
    itemgroup->add(b);

  if (doc.hasAttribute("custom"))
    g.last(doc.attribute("custom").toInt());
  b->setGeometry(g.bbox());
  g.right();
  b->show();

  return b;
}

guiItem *guiMenu::createItem(QString id) {
  return new guiMenuItem(this, id, master);
}

void guiMenu::prepForOpening() {
  Param *pp = ptree ? ptree->leafp() : 0;
  if (!pp)
    throw Exception("guiMenu", "openSelf failed because paramtree has no leaf");

  //Dbg() << "prepforopening:" << path() << "autoi="<<autoItems
  //      << " over="<<overridden
  //      << " ids="<<(autoItems?autoItems->allIDs().join(",") : QString("-"));
  if (autoItems && !overridden)
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
  autoItems->setup(g, doc, ptree);
}

bool guiMenu::mayResize() {
  return autoItems==0 || true; // !autoItems->isDynamic();
}

void guiMenu::updateAuto() {
  // Dbg() << "guimenu:" << path() << ": updateauto";
  overridden = false;
  if (autoItems)
    autoItems->rebuild();
}

void guiMenu::overrideAuto(QStringList ids) {
  //  Dbg() << "guimenu:" << path() << ": overrideauto: " << ids.join(",");
  overridden = true;
  if (autoItems)
    autoItems->override(ids);
}
