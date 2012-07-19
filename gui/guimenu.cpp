// guimenu.cpp

#include "guimenu.h"

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

