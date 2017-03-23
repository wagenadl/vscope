// pvp/pvpSystem.h - This file is part of VScope.
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

// pvpSystem.h

#ifndef PVPSYSTEM_H

#define PVPSYSTEM_H

#include <QString>
#include <pvp/pvpException.h>

class pvpSystem {
public:
  pvpSystem();
  static int countCameras() /*throw(pvpException)*/;
  static QString getCamName(int n) /*throw(pvpException)*/;
  static int getPVCAMVersion() /*throw(pvpException)*/;
  static void reportStatus() /*throw(pvpException)*/;
  static void initialize(); // called automatically
  static void closedown() /*throw(pvpException)*/;
private:
  private:
  static bool inited;
};

#endif
