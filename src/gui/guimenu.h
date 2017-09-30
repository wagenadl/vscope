// gui/guimenu.h - This file is part of VScope.
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

// guimenu.h

#ifndef GUIMENU_H

#define GUIMENU_H

#include <gui/guipage.h>

class guiMenu: public guiPage {
  Q_OBJECT;
public:
  guiMenu(class QWidget *parent,
	  class ParamTree *ptree,
	  QString id,
	  class guiRoot *master,
	  class QRect const &geom);
  virtual ~guiMenu();
  virtual class guiButton *addItem(PageBuildGeom &g, QDomElement elt);
public slots:
  virtual void updateAuto();
  virtual void overrideAuto(QStringList ids);
  void setNonExclusive();
protected:
  virtual VISUALTYPE visualTypeForParentButton() const;
  virtual void connectToParent(QDomElement doc);
  virtual guiItem *createItem(QString id); // simply constructs a guiMenuItem
  virtual void prepForOpening();
  virtual void addAuto(PageBuildGeom &g, QDomElement doc);
  virtual bool mayResize();
protected:
  class AutoItems *autoItems;
  class RadioGroup *itemgroup;
  bool overridden;
};

#endif
