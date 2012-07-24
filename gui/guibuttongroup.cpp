// guibuttongroup.cpp

#include "guibuttongroup.h"
#include <xml/attribute.h>
#include <gfx/buttongrouper.h>
#include <stdio.h>
#include "autobuttons.h"
#include <base/exception.h>
#include <gui/guiitem.h>

guiButtonGroup::guiButtonGroup(guiPage *parent):
  QObject(parent), parent_(parent) {
}

guiButtonGroup::~guiButtonGroup() {
}

void guiButtonGroup::build(PageBuildGeom &g, QDomElement doc) {
  id = xmlAttribute(doc, "id", "guiButtonGroup", "Cannot read id");
  g.enterGroup(doc);

  for (QDomElement e=doc.firstChildElement(); !e.isNull();
       e=e.nextSiblingElement()) {
    QString tag = e.tagName();
    if (tag=="button") {
      guiButton *b = parent_->addButton(g, e);
      ButtonGrouper::add(b, this);
      b->makeRadio();
      b->setVisualType(b->getID().contains("panel") ?
		       Button::VTPanelOpen : Button::VTGrouped);
      childids.append(e.attribute("id"));
    } else if (tag=="break") {
      g.nextColumn(doc);
    } else if (tag=="auto") {
      AutoButtons *a = new AutoButtons(this);
      autoButtons.append(a);
      a->setup(g, e);
    } else {
      fprintf(stderr,"Warning: xmlPage: Unexpected tag <%s> in group.\n",
	      qPrintable(tag));
    }
  }
  g.leaveGroup();  

  if (doc.hasAttribute("default"))
    dfltButton = doc.attribute("default"); 
}

bool guiButtonGroup::mayResize() {
  foreach (AutoButtons *a, autoButtons)
    if (a->isDynamic())
      return false;
  return true;
}

void guiButtonGroup::selectDefaultButton() {
  if (dfltButton.isEmpty())
    return;
  guiButton *b = parent_->buttonp(dfltButton);
  if (b)
    b->setSelected(true);
}

void guiButtonGroup::add(QString id) {
  if (childids.contains(id))
    throw Exception("guiButtonGroup", "Already have id " + id);
  guiButton *b = parent_->buttonp(id);
  if (!b)
    throw Exception("guiButtonGroup", "Parent does not have button " + id, "add");
  if (!dynamic_cast<guiItem*>(b)) {
    /* How ugly! */
    ButtonGrouper::add(b, this);
    b->makeRadio();
    b->setVisualType(b->getID().contains("panel") ?
		     Button::VTPanelOpen : Button::VTGrouped);
  }
  childids.append(id);
}

bool guiButtonGroup::remove(QString id) {
  bool r = childids.removeOne(id);
  if (id==dfltButton)
    dfltButton = childids.empty() ? "" : childids[0];
  guiButton *b = parent_->buttonp(id);
  if (b)
    ButtonGrouper::remove(b);
  return r;
}

void guiButtonGroup::rebuildAuto() {
  foreach (AutoButtons *a, autoButtons)
    a->rebuild();
}
