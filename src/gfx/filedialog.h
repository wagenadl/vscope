// gfx/filedialog.h - This file is part of VScope.
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

// filedialog.h

#ifndef FILEDIALOG_H

#define FILEDIALOG_H

#include <QFrame>
#include <QString>

class FileDialog: public QFrame {
  /*:C FileDialog
   *:D A file dialog for loading or saving
   */
  Q_OBJECT;
public:
  FileDialog(QWidget *parent,
	     class FileChooser *alt_dirs=0,
	     class FileChooser *alt_files=0);
  virtual ~FileDialog();
  void mustExist(bool);
  void requireConfirm(bool);
  QString getPath() const { return path; }
public slots:
  void goDir(QString path);
  void setExtn(QString extn);
signals:
  void changedDir(QString path);
  void chosenFile(QString fn);
  void canceled();
protected:
  void resizeEvent(class QResizeEvent *);
private slots:
  void selectedFile(QString fn);
  void selectedDir(QString fn);
  void selectedPath(int idx, QString fn);
  void clickedOK();
  void edited(QString nwtxt);
  void hitReturn();
  void synthKeyPress(QChar asc);
private:
  void relayout();
private:
  class ScrollButtonBar *pathbar;
  class FileChooser *dirs;
  class FileChooser *files;
  class Button *cancel;
  class Button *ok;
  class QLineEdit *editor;
  QString path;
  QString extn;
  bool confirmFlag;
  bool mustExistFlag;
  QString selFile;
};

#endif
