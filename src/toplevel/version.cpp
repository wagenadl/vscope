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

#ifndef gitREVNO
#define gitREVNO "?"
#endif
#ifndef gitDATE
#define gitDATE "?"
#endif
#ifndef gitVERSION
#define gitVERSION "?"
#endif
#ifndef gitYEAR
#define gitYEAR "?"
#endif

#define buildDATE __DATE__ ", " __TIME__

QString versionBanner(QString cls) {
  QString txt = gitVERSION;
  txt += cls;

  txt += "<p>(C) Daniel A. Wagenaar 2008&ndash;" gitYEAR;
  QString gd = gitDATE;
  gd.replace("..", ", ");
  gd.replace(".", " ");
  txt += "<p>Last commit: " + gd + "<br>";
  txt += "Build date: " buildDATE;
  txt += "<p>For more info: daw@caltech.edu";

  return txt;
}
