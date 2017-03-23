// toplevel/gt_slots.h - This file is part of VScope.
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

// gt_slots.h

#ifndef GT_SLOTS_H

#define GT_SLOTS_H

#include <QObject>
#include <QString>

class gt_slots: public QObject {
  Q_OBJECT;
public:
  gt_slots(class guiRoot *master);
  virtual ~gt_slots();
public slots:
  void clicked(QString p);
  void doubleClicked(QString p);
  void selected(QString p);
  void deselected(QString p);
  void paramchanged(QString p, QString val);
  void customchanged(QString p, int cno, QString val);
  void pgopen(QString p, QWidget *w);
  void pgclose(QString p, QWidget *w);
  void everythingChanged();
protected:
  void report(class Exception const &e, QString act);
  class ParamTree *ptree();
  void ensureMasterOK(QString);
  void setRefTrace();
protected:
  guiRoot *guiroot;
};

#endif
