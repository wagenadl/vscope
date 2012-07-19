// guiItem.cpp

#include "guiitem.h"

guiItem::guiItem(QWidget *parnt, QString id, guiRoot *mastr):
  guiButton(parnt, id, mastr) {
  makeItem();
  setVisualType(Button::VTVarValue);
}

void guiItem::setup(QDomElement doc) {
  guiButton::setup(doc);
}

guiItem::~guiItem() {
}

void guiItem::connectUp(QDomElement) {
}

void guiItem::stylize(QDomElement) {
}
