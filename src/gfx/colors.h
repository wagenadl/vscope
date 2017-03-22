// gfx/colors.h - This file is part of VScope.
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

// colors.h

#ifndef COLORS_H

#define COLORS_H
#include <QString>
#include <QDomElement>
#include <QMap>
#include <QColor>

class Colors {
public:
  static void add(QDomElement elt);
  /*:F read
   *:D elt can either be of type "colors" or contain an element of
       type "colors".
  */
  static QColor find(QString name, QString dflt);
  static QColor find(QString name); // throws exception if not found
  static bool has(QString name);
private:
  Colors();
  ~Colors();
  QMap<QString, QColor> map;
  void readxml(QDomElement elt);
private:
  static Colors *colors();
};

#endif
