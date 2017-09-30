// gui/guiexc.h - This file is part of VScope.
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

// exception.h

#ifndef GUIEXCEPTION_H

#define GUIEXCEPTION_H

#include <QString>
#include <base/exception.h>

class GUIExc {
public:
  static void report(Exception const &e, QString reporter) throw();
  // report calls saveSettings
public:
  static void setSettingsDir(QString path);
  static void setParamTree(class ParamTree const *ptree);
  static void setArgs(int argc, char **argv);
  static void saveSettings();
private:
  static QString settingsdir;
  static class ParamTree const *ptree;
  static int argc;
  static char **argv;
};

#endif
