// guichecklistitem.cpp

#include "guichecklistitem.h"
#include "guipage.h"
#include <base/exception.h>

guiChecklistItem::guiChecklistItem(QWidget *parnt, QString id, guiRoot *mastr):
  guiItem(parnt, id, mastr) {
  makeToggle();
}

guiChecklistItem::~guiChecklistItem() {
}

void guiChecklistItem::connectUp(QDomElement) {
  guiPage *grandparent = parent->parentPage();
  if (!grandparent)
    throw Exception("ChecklistItem", "I don't have a grandparent");

  connect(this, SIGNAL(selected(QString,QString)),
	  grandparent, SLOT(childItemSelected(QString,QString)));
  connect(this, SIGNAL(deselected(QString,QString)),
	  grandparent, SLOT(childItemDeselected(QString,QString)));
}

