// guiItem.cpp

#include "guiitem.h"

guiItem::guiItem(QWidget *parnt, QString id, guiRoot *mastr):
  guiButton(parnt, id, mastr) {
  makeItem();
  setVisualType(Button::VTVarValue);
}

guiItem::~guiItem() {
}

void guiItem::stylize(QDomElement) {
}
