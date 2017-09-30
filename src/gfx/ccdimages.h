// gfx/ccdimages.h - This file is part of VScope.
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

// ccdimages.h

#ifndef CCDIMAGES_H

#define CCDIMAGES_H


#include <QString>
#include <QStringList>
#include <QMap>
#include <QRect>
#include <QSignalMapper>

#include <base/enums.h>
#include <gfx/ccdimage.h>

class CCDImages: public QObject {
  Q_OBJECT;
public:
  CCDImages(QRect canvas);
  virtual ~CCDImages();
  CCDImage *get(QString id);
  CCDImage *first();
  QStringList ids() const;
  void add(QString id, CCDImage *img);
  void del(QString id);
  bool has(QString id) const;
  QRect const &currentCanvas() const { return canvas; }
public slots:
  void setCanvas(QRect const &r);
  void updateZoom(QRect); // rectangle in global coords
signals:
  void newZoom(QRect); // rectangle in global coords
private slots:
  void shareZoom(QString);
protected:
  QSignalMapper *zm;
  QMap<QString, CCDImage *> imgs;
  QRect canvas;
};

#endif
