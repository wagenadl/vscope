// gfx/roiimages.h - This file is part of VScope.
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

// roiimages.h

#ifndef ROIIMAGES_H

#define ROIIMAGES_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QRect>
#include <QSignalMapper>

#include <base/enums.h>
#include <gfx/roiimage.h>
#include <gfx/ccdimages.h>

class ROIImages: public CCDImages {
  Q_OBJECT;
public:
  ROIImages(QRect canvas);
  virtual ~ROIImages();
  ROIImage *get(QString id);
  ROIImage *first();
  void add(QString id, ROIImage *img);
public slots:
  void setMode(ROIImage::ClickMode cm);
  void showROIs(SHOWROIS sm);
  void setROIs(class ROISet *);
  void updateSelection(int);
signals:
  void newSelection(int);
private slots:
  void shareSelection(QString);
protected:
  QList<ROIImage *> images();
protected:
  QSignalMapper *sm;
  ROISet *rs;
};

#endif
