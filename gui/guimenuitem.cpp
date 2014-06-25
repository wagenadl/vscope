// guimenuitem.cpp

#include "guimenuitem.h"
#include "guipage.h"
#include <base/exception.h>

guiMenuItem::guiMenuItem(guiPage *parnt, QString id, guiRoot *mastr):
  guiItem(parnt, id, mastr) {
  makeRadio();
}

guiMenuItem::~guiMenuItem() {
}

void guiMenuItem::connectUp(QDomElement) {
  guiPage *grandparent = parent->parentPage();
  if (!grandparent)
    throw Exception("MenuItem", "I don't have a grandparent");

  connect(this, SIGNAL(selected(QString,QString)),
	  grandparent, SLOT(childItemSelected(QString,QString)));
  if (isCustom())
    connect(this, SIGNAL(customize(QString,int,QString)),
	    grandparent, SLOT(childItemCustomized(QString,int,QString)));
}

