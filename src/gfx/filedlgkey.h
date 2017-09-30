// gfx/filedlgkey.h - This file is part of VScope.
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

// filedlgkey.h

#ifndef FILEDLGKEY_H

#define FILEDLGKEY_H

#include <QFrame>

class FileDlgKey: public QFrame {
  /*:C FileDlgKey
   *:D A version of FileDialog with a virtual keyboard
   */
  Q_OBJECT;
public:
  FileDlgKey(QWidget *parent, bool expts=false);
  /*:F constructor
   *:D If expts=true, we create an ExptFileDialog.
   */
  virtual ~FileDlgKey();
  void mustExist(bool);
  void requireConfirm(bool);
  QString getPath() const;
public slots:
  void goDir(QString path);
  void setExtn(QString extn);
  void setLabel(QString label);
signals:
  void changedDir(QString path);
  void chosenFile(QString fn);
  void canceled();
protected:
  virtual void resizeEvent(class QResizeEvent *);
private:
  class QLabel *hdr;
  class FileDialog *fd;
  class VirtKeyboard *kb;
};

#endif
