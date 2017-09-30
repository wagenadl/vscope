// xml/settingsfile.h - This file is part of VScope.
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

// settingsfile.h

#ifndef SETTINGSFILE_H

#define SETTINGSFILE_H

#include <QString>
#include <xml/paramtree.h>

class SettingsFile {
public:
  static void save(QString fn, ParamTree const *src);
  static void load(QString fn, ParamTree *dst);
};

#endif
