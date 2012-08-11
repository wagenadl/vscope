// guiradiogroup.cpp

#include "guiradiogroup.h"
#include "guipage.h"
#include <gfx/radiogroup.h>
#include <xml/enumerator.h>
#include <xml/attribute.h>
#include <base/exception.h>
#include "autobuttons.h"

guiRadioGroup::guiRadioGroup(guiPage *parent):
  QObject(parent),
  page(parent) {
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
    if (tag=="button") {
      guiButton *b = page->addButton(g, e);
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
  foreach (AutoButtons *a, autoButtons)
    if (a->isDynamic())
      return false;
  return true;
}
