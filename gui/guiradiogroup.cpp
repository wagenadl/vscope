// guiradiogroup.cpp

#include "guiradiogroup.h"
#include "guipage.h"
#include <gfx/radiogroup.h>
#include <xml/enumerator.h>
#include <base/exception.h>

guiRadioGroup::guiRadioGroup(guiPage *parent):
  QObject(parent),
  parent_(parent) {
  rg = new RadioGroup(this);
}

guiRadioGroup::~guiRadioGroup() {
}

void guiRadioGroup::build(PageBuildGeom &g, QDomElement doc) {
  for (QDomElement e=doc.firstChildElement(); !e.isNull();
       e=e.nextSiblingElement()) {
    QString tag = e.tagName();
    if (tag=="button") {
      ids.insert(e.attribute("id"));
      guiButton *b = parent_->addButton(g, e);
      rg->add(b); // this overrides visual type...
      if (e.hasAttribute("vt")) // ... so we may have to restore it
	b->setVisualType((VISUALTYPE)Enumerator::find("VISUALTYPE")
			 ->lookup(e.attribute("vt")));
    } else {
      throw Exception("guiRadioGroup", "Unexpected tag " + tag);
    }
  }
  if (doc.hasAttribute("default"))
    dflt = doc.attribute("default"); 
}

void guiRadioGroup::selectDefaultButton() {
  guiButton *b = parent_->buttonp(dflt);
  if (b)
    b->setSelected(true);
}

QStringList guiRadioGroup::childIDs() const {
  QStringList x;
  foreach (QString id, ids)
    x.append(id);
  return x;
}
