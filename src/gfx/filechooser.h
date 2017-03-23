// gfx/filechooser.h - This file is part of VScope.
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

// filechooser.h

#ifndef FILECHOOSER_H

#define FILECHOOSER_H

#include <QFrame>
#include <QDir>

class FileChooser: public QFrame {
  /*:C FileChooser
   *:D This is a frame around a FileList, with Left and Right buttons
       and perhaps an Up button.
       The geometry of the FileChooser is typically set by the parent;
       the FileChooser then picks an appropriate size for the FileList.
  */
  Q_OBJECT;
public:
  FileChooser(QWidget *parent, class FileList *alt_child=0);
  /*:F constructor
   *:D Optional second argument passes in a preconstructed file list.
       This is useful if you need a derived class as your file list, e.g.,
       to employ more complex file name inclusion rules or to show additional
       information below the file name.
   *:N This FileChooser becomes the owner of the alt_child, and will 
       ultimately delete it.
  */
  virtual ~FileChooser();
  void populateFiles(QDir const &src, QString extn, bool hideextn=true);
  void populateDirs(QDir const &src);
  int countColumns() const;
  int countItems() const;
  void setBackground(QColor bg);
  class Button *buttonp(QString fn);
  /*:F buttonp
   *:D Returns the button associated with the given filename, or null.
   */
signals:
  void clickedItem(QString fn);
  /*:S clickedItem
   *:D Emitted whenever the user clicks on an item.
   *:N Special case: the "Up" button generates a clickedItem with fn="..".
   */
public slots:
  void clickedLeft();
  void clickedRight();
  void clickedUp();
  void clickedFile(QString fn);
  void scroll(double col);
  void scrolling(double col);
  void scrollTo(QString partialfn="");
protected:
  void resizeEvent(class QResizeEvent *);
private:
  void relayout();
private:
  class FileList *list;
  class Button *buttonLeft, *buttonRight, *buttonUp;
  class HScrollBar *scrollBar;
  bool isDirs;
};

#endif
