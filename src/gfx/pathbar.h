// gfx/pathbar.h - This file is part of VScope.
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

// pathbar.h

#ifndef PATHBAR_H

#define PATHBAR_H

#include <QVector>
#include <QScrollArea>

class PathBar: public QScrollArea {
  Q_OBJECT;
public:
  PathBar(QWidget *parent);
  virtual ~PathBar();
  void setPath(QString path);
signals:
  void pathSelected(QString path);
protected:
  virtual void resizeEvent(class QResizeEvent *);
  virtual void paintEvent(class QPaintEvent *);
protected slots:
  void elementClicked(QString id);
private:
  void validateArea();
private:
  QWidget *area;
  class QHBoxLayout *layout;
  QVector<class Button *> elements;
  bool asizeValid;
};

#endif
