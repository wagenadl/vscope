// gfx/exptdirlist.h - This file is part of VScope.
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

// exptdirlist.h

#ifndef EXPTDIRLIST_H

#define EXPTDIRLIST_H

#include <gfx/filelist.h>
class ExptDirList: public FileList {
  /*:C ExptDirList
   *:D A refinement of FileList that provides additional info for those
       directories that contain experiments.
   *:N We use the "log.txt" file to determine whether a directory contains
       an experiment; the presence of "Trial nnn: xxxx" lines in that file
       is our heuristic.
  */
  Q_OBJECT;
public:
  ExptDirList(QWidget *parent);
protected:
  virtual QString infoHook(class QDir const &dir, QString fn);
};

#endif
