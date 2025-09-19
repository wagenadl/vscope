// toplevel/version.cpp - This file is part of VScope.
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

// version.cpp

#include "version.h"
#include "config.h"
#include <QStringList>

#define buildDATE __DATE__ ", " __TIME__

QString versionBanner(QString cls) {
  QString txt = VSCOPE_GITVERSION;
  QString gd = VSCOPE_GITDATE;
  QStringList bits = gd.split(".");
  QString year = (bits.size() >= 3) ? bits[2] : "?";
  txt += cls;
  txt += "<p>(C) Daniel A. Wagenaar 2008&ndash;";
  txt += year;
  gd.replace("..", ", ");
  gd.replace(".", " ");
  txt += "<p>Last commit: " + gd + "<br>";
  txt += "Build date: " buildDATE;
  txt += "<p>For more info: daw@caltech.edu";

  return txt;
}
