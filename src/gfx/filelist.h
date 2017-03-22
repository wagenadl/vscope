// gfx/filelist.h - This file is part of VScope.
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

// filelist.h

#ifndef FILELIST_H

#define FILELIST_H

#include <QFrame>
#include <QString>
#include <QStringList>
#include <QColor>

/* A FileDialog consists of one or two FileChoosers: one for directories,
   and one for files. It also may contain a bar for direct typing of a
   name, OK and Cancel buttons, and a VirtKeyboard.
   A FileChooser consists of a FileList, "Left" and "Right" buttons to navigate
   that list, and an "Up" button if the chooser contains directories.
*/

class FileList: public QFrame {
  /*:C FileList
   *:D A simple array of file or directory names arranged in columns.
       This class does not provide scroll bars; that is left to the next
       level up, which is called FileChooser.
  */
  Q_OBJECT;
public:
  FileList(QWidget *parent);
  virtual ~FileList();
  void setColumnWidth(int pix);
  void setRowHeight(int pix);
  void setHeight(int pix);
  int countColumns() const { return ncols; }
  int countItems() const { return children.size(); }
  int columnWidth() const { return colwidth_pix; }
  int rowHeight() const { return rowheight_pix; }
  int countRows() { return nrows; }
  void populateFiles(class QDir const &src, QString extn, bool hideext=true);
  void populateDirs(class QDir const &src);
  void populate(class QDir const &src, QStringList lst);
  int indexOf(QString partialfn); // returns the index of the first entry matching partialfn, or -1
  void setBackground(QColor bg);
  class Button *buttonp(QString fn);
  /*:F buttonp
   *:D Returns a pointer to the button that represents the given filename,
       or null if not found.
  */
protected:
  virtual bool includeFileHook(class QDir const &dir, QString fn);
  /*:F includeFileHook
   *:D Hook for descendents to vet whether a certain file should be included
       in the list.
   *:A fn is relative to our directory.
   *:N The default implementation returns true unless fn starts with "_".
   */
  virtual bool includeDirHook(class QDir const &dir, QString fn);
  /*:F includeDirHook
   *:D Hook for descendents to vet whether a certain dir should be included
       in the list.
   *:A fn is relative to our directory.
   *:N The default implementation returns true unless fn starts with "_".
  */
  virtual QString infoHook(class QDir const &dir, QString fn);
  /*:F infoHook
   *:D Hook for descendents to supply additional information about a file.
   *:A fn is relative to our directory.
   *:N The default implementation returns the empty string.
   */
signals:
  void clickedItem(QString fn);
private:
  void relayout();
private slots:
  void buttonClicked(QString id, QString txt);
private:
  int colwidth_pix;
  int rowheight_pix;
  int height_pix;
  int width_pix; // automatically calculated
  int nrows;
  int ncols;
  QVector<class Button *> children;
  QColor bg;
};

#endif
