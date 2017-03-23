// gui/autobuttons.h - This file is part of VScope.
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

// autobuttons.h

#ifndef AUTOBUTTONS_H

#define AUTOBUTTONS_H

#include <gui/pagebuildgeom.h>
#include <gui/guipage.h>
#include <QString>
#include <QMap>
#include <QSet>
#include <xml/enumerator.h>
#include <gui/guiradiogroup.h>

class AutoButtons: public QObject {
  Q_OBJECT;
public:
  AutoButtons(guiPage *page, guiRadioGroup *group=0);
  void setup(PageBuildGeom &geom,
	     QDomElement doc);
  void rebuild(PageBuildGeom *g_out=0);
  QStringList selectIDs(QStringList inlist); // currently simply returns inlist
  QList<QString> childIDs() const; // keys of our buttons
private:
  guiPage *page;
  guiRadioGroup *group;
  QStringList ids;
  QMap<QString, guiButton *> buttons;
  QDomElement doc;
  QString pfx;
  PageBuildGeom initialGeom;
  Enumerator const *enumerator;
  bool isDyn;
};

#endif
