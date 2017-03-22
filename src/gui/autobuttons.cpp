// gui/autobuttons.cpp - This file is part of VScope.
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

// autobuttons.cpp

#include "autobuttons.h"
#include "guichecklist.h"
#include <base/exception.h>
#include <base/dbg.h>
#include <xml/attribute.h>

AutoButtons::AutoButtons(guiPage *page, guiRadioGroup *group):
  QObject(page), page(page), group(group),
  initialGeom(page) {
  if (!page)
    throw Exception("AutoButtons", "Missing a page");
}

void AutoButtons::setup(PageBuildGeom &geom,
		      QDomElement doc_) {
  initialGeom = geom;
  doc = doc_;
  pfx = doc.hasAttribute("id")
    ? doc.attribute("id")
    : group
    ? group->id()
    : "";
  initialGeom.go(doc);
  enumerator = Enumerator::find(doc.attribute("enum"));
  if (!enumerator)
    throw Exception("AutoButtons", "No enumerator");
  rebuild(&geom);
}

QStringList AutoButtons::selectIDs(QStringList inlist) {
  QStringList outlist;
  foreach (QString id, inlist) {
    // filter based on our xml doc
    outlist.append(id);
  }
  return outlist;
}

void AutoButtons::rebuild(PageBuildGeom *g_out) {
  QStringList newids = selectIDs(enumerator->getNonnegativeTags());
  if (newids == ids)
    return;

  foreach (QString id, buttons.keys()) 
    page->deleteButton(pfx + ":" + id);

  buttons.clear();

  ids = newids;

  PageBuildGeom g = initialGeom;
  QDomDocument xml;
  QString hd1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone='yes'?>";
  QString hd2 = "<!DOCTYPE vscopeAuto>";
  QString tag = (doc.hasAttribute("immune") && doc.attribute("immune").toInt())
    ? "immune" : "button";
  foreach (QString id, ids) {
    QString fullid = pfx + ":" + id;
    // let's build an item
    xml.setContent(hd1 + "\n" + hd2 + "\n"
		   + "<" + tag
		   + " id=\"" + fullid + "\""
		   + " value=\"" + id + "\""
		   + "/>\n");
    QDomElement e = xml.documentElement();
    buttons[id] = page->addButton(g, e);
    g.up(); g.right();
    if (group)
      group->add(buttons[id]);
    buttons[id]->show();
  }
  
  if (g_out)
    *g_out = g;
}
 
QList<QString> AutoButtons::childIDs() const {
  return buttons.keys();
}
