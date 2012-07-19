// guimenu.cpp

#include "guimenu.h"
#include "guimenuitem.h"

guiMenu::guiMenu(class QWidget *parent,
		 class ParamTree *ptree,
		 QString id,
		 class guiRoot *master,
		 class QRect const &geom):
  guiPage(parent, ptree, id, master, geom) {
}

void guiMenu::setup(QDomElement doc) {
  guiPage::setup(doc);
}

guiMenu::~guiMenu() {
}

Button::VisualType guiMenu::visualTypeForParentButton() const {
  return Button::VTVarOpen;
}

void guiMenu::connectToParent(QDomElement doc) {
  guiPage::connectToParent(doc);
  guiButton *b = parentPage()->buttonp(id());
  if (b)
    b->ensureValueInLabel();
}

guiItem *guiMenu::createItem(QString id) {
  return new guiMenuItem(this, id, master);
}
