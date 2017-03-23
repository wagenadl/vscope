// toplevel/savedsettings.h - This file is part of VScope.
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

// savedsettings.h

#ifndef SAVEDSETTINGS_H

#define SAVEDSETTINGS_H

#include <gfx/filechooser.h>

class SavedSettings: public FileChooser {
  Q_OBJECT;
public:
  SavedSettings(QWidget *parent);
  virtual ~SavedSettings();
public slots:
  void loadSettings(QString fn);
  void saveSettings(QString fn);
  void prepareSave();
  void deleteCurrent();
  void anythingChanged();
protected:
  virtual void showEvent(class QShowEvent *);
private:
  void visualdeselect();
  void visualselect(QString fn);
signals:
  void selected(QString fn);
private:
  class FileDlgKey *savedlg;
  class QFrame *saveframe;
  QString lastsel;
};

#endif
