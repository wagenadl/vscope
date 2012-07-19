// guichecklist.cpp

#include "guichecklist.h"

guiChecklist::guiChecklist(class QWidget *parent,
		 class ParamTree *ptree,
		 QString id,
		 class guiRoot *master,
		 class QRect const &geom):
  guiPage(parent, ptree, id, master, geom) {
}

void guiChecklist::setup(QDomElement doc) {
  guiPage::setup(doc);
}

guiChecklist::~guiChecklist() {
}

Button::VisualType guiChecklist::visualTypeForParentButton() const {
  return Button::VTVarOpen;
}

guiButton *guiChecklist::addItem(PageBuildGeom &g, QDomElement elt) {
  guiButton *b = guiPage::addItem(g, elt);
  b->makeToggle();
  return b;
}
