// guibuttongroup.cpp

#include "guibuttongroup.h"
#include <xml/attribute.h>
#include <gfx/buttongrouper.h>
#include <stdio.h>

guiButtonGroup::guiButtonGroup(guiPage *parent):
  QObject(parent), parent(parent) {
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
      guiButton *b = parent->addButton(g, e);
      ButtonGrouper::add(b, this);
      b->makeRadio();
      b->setVisualType(b->getID().contains("panel") ?
		       Button::VTPanelOpen : Button::VTGrouped);
      childids.append(e.attribute("id"));
    } else if (tag=="break") {
      g.nextColumn(doc);
    } else {
      fprintf(stderr,"Warning: xmlPage: Unexpected tag <%s> in group.\n",
	      qPrintable(tag));
    }
  }
  g.leaveGroup();  

  if (doc.hasAttribute("default"))
    dfltButton = doc.attribute("default"); 
}

void guiButtonGroup::selectDefaultButton() {
  if (dfltButton.isEmpty())
    return;
  guiButton *b = parent->buttonp(dfltButton);
  if (b)
    b->setSelected(true);
}
