// xml/settingsfile.cpp - This file is part of VScope.
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

// settingsfile.cpp

#include "settingsfile.h"

#include <xml/connections.h>
#include <base/xml.h>

#include <QDateTime>

void SettingsFile::save(QString fn, ParamTree const *src) {
  XML xml(0,"vsdscopeSettings");
  src->write(xml.root());
  // Connections::writeXML(xml.root());
  QDomElement e=xml.append("date");
  e.setAttribute("modified",QDateTime::currentDateTime().toString());
  xml.write(fn);
}

void SettingsFile::load(QString fn, ParamTree *dst) {
  XML xml(fn);
  dst->read(xml.root());
  // Connections::readXML(xml.root());
} 
