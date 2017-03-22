// gfx/colors.cpp - This file is part of VScope.
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

// colors.cpp

#include "colors.h"
#include <base/dbg.h>
#include <base/exception.h>

Colors *Colors::colors() {
  static Colors *c = new Colors();
  return c;
}

Colors::Colors() {
}

Colors::~Colors() {
}

bool Colors::has(QString name) {
  return colors()->map.contains(name);
}

QColor Colors::find(QString name) {
  if (!has(name))
    throw Exception("Colors","No color named " + name);;
  return QColor(colors()->map[name]);
}

QColor Colors::find(QString name, QString dflt) {
  if (has(name))
    return find(name);
  else
    return QColor(dflt);
}

void Colors::add(QDomElement elt) {
  colors()->readxml(elt);
}

void Colors::readxml(QDomElement elt) {
  if (elt.tagName()!="colors") {
    elt = elt.firstChildElement("colors");
    if (elt.isNull()) {
      Dbg() << "Colors: No <colors> element found";
      return;
    }
  }
  for (QDomElement e=elt.firstChildElement("color");
       !e.isNull(); e=e.nextSiblingElement("color")) {
    QString id = e.attribute("id");
    QString val = e.attribute("value");
    if (id.isEmpty() || val.isEmpty())
      throw Exception("Colors",
		      "<color> element must have id and value attributes");
    map[id] = QColor(val);
  }
}

    
