// gfx/radiogroup.h - This file is part of VScope.
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

// radiogroup.h

#ifndef RADIOGROUP_H

#define RADIOGROUP_H

#include <QObject>
#include <QPointer>
#include <QMap>
#include <gfx/button.h>

class RadioGroup: public QObject {
  Q_OBJECT;
public:
  RadioGroup(QObject *parent=0);
  virtual ~RadioGroup();
public slots:
  void add(Button *b);
  void remove(Button *b);
public:
  Button *selected();
private slots:
  void select();
  void deselect();
private:
  QMap< Button*, QPointer<Button> > buttons;
  QPointer<Button> selection;
private:
  static QMap<RadioGroup *, QPointer<RadioGroup> > allgroups;
};

#endif
