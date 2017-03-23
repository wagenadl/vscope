// gui/guiradiogroup.h - This file is part of VScope.
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

// guiradiogroup.h

#ifndef GUIRADIOGROUP_H

#define GUIRADIOGROUP_H

#include <QObject>
#include <QDomElement>
#include <QString>
#include <QStringList>
#include <QSet>

class guiPage;
class RadioGroup;
class PageBuildGeom;

class guiRadioGroup: public QObject {
  Q_OBJECT;
public:
  guiRadioGroup(bool ungroup, guiPage *parent);
  // "ungroup" means visually grouped but not radio grouped
  virtual ~guiRadioGroup();
  void build(PageBuildGeom &g, QDomElement doc);
  QStringList childIDs() const;
  QString id() const { return myid; }
  guiPage *parentPage() { return page; }
  bool mayResize();
public slots:
  void selectDefaultButton();
  virtual void add(class Button *b);
  virtual void rebuild();
private:
  QString myid;
  guiPage *page;
  RadioGroup *rg;
  QString dflt;
  QSet<class AutoButtons *> autoButtons;
  QSet<QString> fixedids;
  bool ungroup;
};

#endif
