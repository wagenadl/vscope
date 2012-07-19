// guiItem.cpp

#include "guiitem.h"

guiItem::guiItem(QWidget *parnt, QString id, guiRoot *mastr):
  guiButton(parnt, id, mastr) {
}

void guiItem::setup(QDomElement doc) {
  guiButton::setup(doc);
}

guiItem::~guiItem() {
}

