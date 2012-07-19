// guichecklist.cpp

#include "guichecklist.h"
#include "guichecklistitem.h"

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

guiItem *guiChecklist::createItem(QString id) {
  return new guiChecklistItem(this, id, master);
}
