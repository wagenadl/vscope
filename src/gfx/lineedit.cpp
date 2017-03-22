// gfx/lineedit.cpp - This file is part of VScope.
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

// lineedit.cpp

#include <gfx/lineedit.h>
#include <QKeyEvent>

LineEdit::LineEdit(QWidget *parent): QLineEdit(parent) {
}

LineEdit::~LineEdit() {
}

void LineEdit::keyPressEvent(QKeyEvent *ke) {
  //fprintf(stderr,"lineedit:keypressevent %i\n",ke->key());
  ctrlheld = ke->modifiers() & Qt::ControlModifier;
  switch (ke->key()) { 
  case Qt::Key_Escape: 
    emit escapePressed();
    return; 
  case Qt::Key_Enter:
  case Qt::Key_Return: 
    emit returnPressed();
    return; 
  default:
    QLineEdit::keyPressEvent(ke);
  }
}
