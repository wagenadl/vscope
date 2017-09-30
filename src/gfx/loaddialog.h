// gfx/loaddialog.h - This file is part of VScope.
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

// loaddialog.h

#ifndef LOADDIALOG_H

#define LOADDIALOG_H

#include <QDialog>

class LoadDialog: public QDialog {
public:
  LoadDialog(QWidget *parent);
  virtual ~LoadDialog();
  static QString getLocalFilename(QString dir, QString filter, QString caption);
  static QString getAnywhereFilename(QString dir, QString filter, QString caption);
private:
  void populate(QString dir, QString filter, bool navigable);
  /* If navigable, present [Up] button and list of folders */
  void chdir(QString dir);
  QStringList getFiles(QString dir, QString filter);
  QStringList getFolders(QString dir);
private slots:
  void selectFile(QString fn);
  void selectFolder(QString fn);
  void doubleClickFile(QString fn);
  void doubleClickFolder(QString fn);
  void scrollFilesBack();
  void scrollFilesForward();
  void scrollFoldersBack();
  void scrollFoldersForward();
private:
  bool navigable;
  QString dir;
  QString filter;
  QString chosenfn;
  bool haschoice;
  Button *folderUp, *folderPrev, *folderNext;
  Button *filesPrev, *filesNext;
  Button *actCancel, *actOK;
  QWidget *scrollareaFolders, *scrollareaFiles;
};

#endif
