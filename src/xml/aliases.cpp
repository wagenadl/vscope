// xml/aliases.cpp - This file is part of VScope.
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

// aliases.cpp

#include "aliases.h"
#include <base/dbg.h>
#include <base/exception.h>

Aliases &Aliases::a() {
  static Aliases aa;
  return aa;
}

void Aliases::add(QDomElement doc) {
  a().readxml(doc);
}

Aliases::Aliases() {
}

void Aliases::readxml(QDomElement doc) {
  if (doc.tagName()!="aliases") {
    doc = doc.firstChildElement("aliases");
  }
  if (doc.isNull()) 
    throw Exception("Aliases","No <aliases> element");

  for (QDomElement e=doc.firstChildElement("alias"); !e.isNull();
       e=e.nextSiblingElement("alias")) {
    QString id = e.attribute("id");
    QString lbl = e.attribute("label");
    if (id.endsWith("*"))
      wildcarded[id.left(id.size()-1)] = lbl;
    else
      map[id] = lbl;
  }
}

bool Aliases::has(QString id) {
  if (a().map.contains(id))
    return true;
  foreach (QString k, a().wildcarded.keys()) 
    if (id.startsWith(k))
      return true;
  return false;
}

QString Aliases::lookup(QString id) {
  if (a().map.contains(id))
    return a().map[id];
  foreach (QString k, a().wildcarded.keys()) 
    if (id.startsWith(k))
      return a().wildcarded[k] + id.mid(k.size());
  return id;
}
