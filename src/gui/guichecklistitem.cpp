// gui/guichecklistitem.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

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

