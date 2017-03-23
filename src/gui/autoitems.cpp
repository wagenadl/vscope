// gui/autoitems.cpp - This file is part of VScope.
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

// autoitems.cpp

#include "autoitems.h"
#include "guichecklist.h"
#include <base/exception.h>
#include <base/dbg.h>
#include <xml/attribute.h>
#include "guiroot.h"

AutoItems::AutoItems(guiPage *parent):
  QObject(parent), initialGeom(parent) {
  immune = false;
}

void AutoItems::setup(PageBuildGeom &geom,
		      QDomElement doc_,
		      ParamTree *ptree) {
  initialGeom = geom;
  doc = doc_;
  immune = doc.attribute("immune").toInt() > 0;
  if (ptree && ptree->leafp() && ptree->leafp()->isImmune())
    immune = true;
  enumerator = Enumerator::find(doc.attribute("enum"));
  if (!enumerator)
    throw Exception("AutoItems", "No enumerator");
  rebuild(&geom);
}

QStringList AutoItems::selectIDs(QStringList inlist) {
  QStringList outlist;
  foreach (QString id, inlist) {
    // filter based on our xml doc
    QStringList exclude = doc.attribute("exclude").split(QRegExp("\\s+"));
    bool ok = true;
    foreach (QString s, exclude) {
      if (s == "*id") {
	guiPage *p = dynamic_cast<guiPage*>(parent());
	QString path = p->masterp()->pathInstantiate(p->path());
	int colon = path.lastIndexOf(":");
	if (colon>=0) {
	  path = path.mid(colon+1);
	  int slash = path.indexOf("/");
	  if (slash>=0)
	    path = path.left(slash);
	}
	s = path;
      }
      if (id==s)
	ok = false;
    }
    if (ok)
      outlist.append(id);
  }
  return outlist;
}

void AutoItems::rebuild(PageBuildGeom *g_out) {
  QStringList newids = selectIDs(enumerator->getNonnegativeTags());
  override(newids, g_out);
}

void AutoItems::override(QStringList newids, PageBuildGeom *g_out) {
  if (newids == ids)
    return;
  
  guiPage *p = dynamic_cast<guiPage *>(parent());
  if (!p)
    throw Exception("AutoItems", "Parent is not a page");
  foreach (QString id, items.keys()) {
    // Dbg() << "AI: deleting " << id;
    p->deleteButton(id);
  }

  items.clear();

  ids = newids;

  PageBuildGeom g = initialGeom;
  QDomDocument xml;
  QString hd1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone='yes'?>";
  QString hd2 = "<!DOCTYPE vscopeAuto>";
  foreach (QString id, ids) {
    if (!enumerator->has(id) || enumerator->lookup(id)>=0) {
      // let's build an item
      // Dbg() << "AI: building " << id;
      xml.setContent(hd1 + "\n" + hd2 + "\n"
		     + "<item value=\""
		     + id
		     + "\"/>\n");
      QDomElement e = xml.documentElement();
      items[id] = p->addItem(g, e);
      guiPage *pg = dynamic_cast<guiPage*>(parent());
      if (immune)
        items[id]->makeROImmune();
      if (pg && pg->isReadOnly())
        items[id]->setReadOnly(true);
    }
  }
  
  if (g_out)
    *g_out = g;
}
 
QStringList AutoItems::allIDs() const {
  return ids;
}
