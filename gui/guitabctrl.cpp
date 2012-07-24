// guitabctrl.cpp

#include "guitabctrl.h"
#include "guipage.h"
#include "pagebuildgeom.h"
#include <gfx/radiogroup.h>
#include "autobuttons.h"
#include <base/exception.h>

guiTabCtrl::guiTabCtrl(guiPage *parent):
  QObject(parent),
  parent_(parent) {
  rg = new RadioGroup(this);
}

guiTabCtrl::~guiTabCtrl() {
}

void guiTabCtrl::add(Button *b) {
  rg->add(b);
}

void guiTabCtrl::build(PageBuildGeom &g0, QDomElement doc) {
  id_ = doc.attribute("id");
  PageBuildGeom g(g0, doc);

  for (QDomElement e=doc.firstChildElement(); !e.isNull();
       e=e.nextSiblingElement()) {
    QString tag = e.tagName();
    if (tag=="button") {
      guiButton *b = parent_->addButton(g, e);
      g.up(); g.right();
      rg->add(b);
      fixedids.insert(b->id());
    } else if (tag=="auto") {
      AutoButtons *a = new AutoButtons(this);
      a->setup(g, e);
      autoButtons.insert(a);
    } else {
      throw Exception("guiTabCtrl", "Unexpected tag " + tag);
    }
  }
}

void guiTabCtrl::rebuild() {
  foreach (AutoButtons *a, autoButtons)
    a->rebuild();
}

bool guiTabCtrl::mayResize() {
  foreach (AutoButtons *a, autoButtons)
    if (a->isDynamic())
      return false;
  return true;
}

QStringList guiTabCtrl::childIDs() const {
  QStringList ids;
  foreach (QString id, fixedids)
    ids.append(id);
  foreach (AutoButtons *a, autoButtons)
    foreach (QString id, a->childIDs())
      ids.append(id);
  return ids;
}
