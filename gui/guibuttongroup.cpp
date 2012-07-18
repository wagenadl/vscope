// guibuttongroup.cpp

#include "guibuttongroup.h"
#include <xml/attribute.h>
#include <gfx/buttongrouper.h>
#include <stdio.h>

guiButtonGroup::guiButtonGroup(xmlPage *parent):
  QObject(parent), parent(parent) {
}

guiButtonGroup::~guiButtonGroup() {
}

void guiButtonGroup::build(PageBuildGeom &g, QDomElement doc) {
  bool ok;
  id = xmlAttribute(doc, "id", "guiButtonGroup", "Cannot read id");
  int cols = doc.hasAttribute("cols") ? doc.attribute("cols").toInt(&ok) : 1;

  int col = 0;
  double startrow = g.nextrow;

  for (QDomElement e=doc.firstChildElement(); !e.isNull();
       e=e.nextSiblingElement()) {
    QString tag = e.tagName();
    if (tag=="button") {
      xmlButton *b = parent->addButton(g, e);
      if (cols>1)
	b->resize(b->width()/cols-((col==cols-1)
				   ? 2
				   : (g.master->intx*(cols-1))),
		  b->height());
      ButtonGrouper::add(b, this);
      b->makeRadio();
      b->setVisualType(b->getID().contains("panel") ?
		       Button::VTPanelOpen : Button::VTGrouped);
      childids.append(e.attribute("id"));
    } else if (tag=="break") {
      g.nextcol += 1./cols;
      col++;
      g.nextrow = startrow;
    } else {
      fprintf(stderr,"Warning: xmlPage: Unexpected tag <%s> in group.\n",
	      qPrintable(tag));
    }
  }
  g.nextrow = startrow;
  g.nextcol += 1./cols;

  if (doc.hasAttribute("default"))
    dfltButton = doc.attribute("default"); 
}

void guiButtonGroup::selectDefaultButton() {
  if (dfltButton.isEmpty())
    return;
  xmlButton *b = parent->buttonp(dfltButton);
  if (b)
    b->setSelected(true);
}
