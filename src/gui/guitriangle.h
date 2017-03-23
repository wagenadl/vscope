// gui/guitriangle.h - This file is part of VScope.
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

// guitriangle.h

#ifndef GUITRIANGLE_H

#define GUITRIANGLE_H

#include <QString>
#include <QObject>
#include <QPolygon>
#include <QColor>

class guiTriangle: public QObject {
  Q_OBJECT;
public:
  guiTriangle(class guiPage *parent);
  void activate(QString id);
  void deactivate(QString id);
  bool isActive() const;
  QString currentId() const { return id; }
  void render();
  void recolor();
private:
  class guiButton *buttonp();
  class guiPage *subpagep();
private:
  class guiPage *parent;
  class guiButton *src;
  class guiPage *dst;
  QString id;
  QPolygon area;
  QPolygon darkLine;
  QPolygon lightLine;
  QColor areaColor;
  QColor darkColor;
  QColor lightColor;
};

#endif
