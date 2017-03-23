// gui/guiradiogroup.cpp - This file is part of VScope.
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

// guiradiogroup.cpp

#include "guiradiogroup.h"
#include "guipage.h"
#include <gfx/radiogroup.h>
#include <xml/enumerator.h>
#include <xml/attribute.h>
#include <base/exception.h>
#include "autobuttons.h"
#include <base/dbg.h>

guiRadioGroup::guiRadioGroup(bool ungroup, guiPage *parent):
  QObject(parent),
  page(parent), ungroup(ungroup) {
  rg = new RadioGroup(this);
}

guiRadioGroup::~guiRadioGroup() {
}

void guiRadioGroup::add(Button *b) {
  rg->add(b);
}

void guiRadioGroup::build(PageBuildGeom &g0, QDomElement doc) {
  PageBuildGeom g(g0, doc);
  myid = doc.attribute("id");
  for (QDomElement e=doc.firstChildElement(); !e.isNull();
       e=e.nextSiblingElement()) {
    QString tag = e.tagName();
    if (tag=="button" || tag=="immune") {
      guiButton *b = page->addButton(g, e);
      if (!ungroup)
	rg->add(b); // this overrides visual type...
      fixedids.insert(b->id());
      if (e.hasAttribute("vt")) // ... so we may have to restore it
	b->setVisualType((VISUALTYPE)Enumerator::find("VISUALTYPE")
			 ->lookup(e.attribute("vt")));
    } else if (tag=="group") {
      build(g, e);
    } else if (tag=="auto") {
      AutoButtons *a = new AutoButtons(page, this);
      a->setup(g, e);
      autoButtons.insert(a);
    } else {
      throw Exception("guiRadioGroup", "Unexpected tag " + tag);
    }
  }
  if (doc.hasAttribute("default"))
    dflt = doc.attribute("default");
  g0.include(g.boundingBox());
}

void guiRadioGroup::selectDefaultButton() {
  guiButton *b = page->buttonp(dflt);
  if (b)
    b->setSelected(true);
}

QStringList guiRadioGroup::childIDs() const {
  QStringList ids;
  foreach (QString id, fixedids)
    ids.append(id);
  foreach (AutoButtons *a, autoButtons)
    foreach (QString id, a->childIDs())
      ids.append(id);
  return ids;
}

void guiRadioGroup::rebuild() {
  foreach (AutoButtons *a, autoButtons)
    a->rebuild();
}

bool guiRadioGroup::mayResize() {
  //foreach (AutoButtons *a, autoButtons)
  //  if (a->isDynamic())
  //    return false;
  return true;
}
