// gfx/exptfiledialog.h - This file is part of VScope.
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

// exptfiledialog.h

#ifndef EXPTFILEDIALOG_H

#define EXPTFILEDIALOG_H

#include <gfx/filedialog.h>

class ExptFileDialog: public FileDialog {
  /*:C ExptFileDialog
   *:D A refinement of FileDialog that puts additional information in the
       buttons that represent directories containing experiments, and
       in buttons that represent trials. Also, it suppresses "-rois.xml"
       files.
  */
  Q_OBJECT;
public:
  ExptFileDialog(QWidget *parent);
};

#endif
