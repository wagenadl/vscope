// xml/aliases.h - This file is part of VScope.
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

// aliases.h

#ifndef ALIASES_H

#define ALIASES_H

#include <QDomElement>
#include <QMap>
#include <QString>

class Aliases {
public:
  static void add(QDomElement def);
  static QString lookup(QString id);
  static bool has(QString id);
private:
  QMap<QString, QString> map;
  QMap<QString, QString> wildcarded;
  void readxml(QDomElement def);
private:
  Aliases();
  static Aliases &a();
};

#endif
