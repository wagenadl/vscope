// guiItem.cpp

#include "guiitem.h"

guiItem::guiItem(guiPage *parnt, QString id, guiRoot *mastr):
  guiButton(parnt, id, mastr) {
  makeItem();
  setVisualType(VT_VarValue);
}

guiItem::~guiItem() {
}

void guiItem::stylize(QDomElement) {
}
