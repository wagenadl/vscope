// gfx/radiogroup.cpp - This file is part of VScope.
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

// radiogroup.cpp

#include "radiogroup.h"

#include <base/dbg.h>

QMap<RadioGroup *, QPointer<RadioGroup> > RadioGroup::allgroups;

RadioGroup::RadioGroup(QObject *parent): QObject(parent) {
  allgroups[this] = QPointer<RadioGroup>(this);
}

RadioGroup::~RadioGroup() {
  allgroups.remove(this);
}

void RadioGroup::add(Button *b) {
  // ownership is exclusive, so remove from other groups
  foreach (QPointer<RadioGroup> const &rg, allgroups)
    if (rg!=this)
      if (!rg.isNull())
	rg->remove(b);

  buttons[b] = QPointer<Button>(b);

  b->makeRadio();
  b->setVisualType(VT_Grouped);

  connect(b, SIGNAL(selected(QString, QString)),
	  this, SLOT(select()));
  connect(b, SIGNAL(deselected(QString, QString)),
	  this, SLOT(deselect()));

}

void RadioGroup::remove(Button *b) {
  if (selection==b)
    selection=0;
  Button *p = buttons[b]; // reference through guard
  buttons.remove(b);
  if (p) {
    disconnect(p, SIGNAL(selected(QString, QString)),
	       this, SLOT(select()));
    disconnect(p, SIGNAL(deselected(QString, QString)),
	       this, SLOT(deselect()));
  }
}

void RadioGroup::select() {
  QObject *src = sender();
  if (selection!=src)
    if (!selection.isNull())
      selection->setSelected(false);
  selection = dynamic_cast<Button *>(src);
}

void RadioGroup::deselect() {
  QObject *src = sender();
  if (selection==src)
    selection = 0;
}
