// gfx/cohmaps.h - This file is part of VScope.
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

// cohmaps.h

#ifndef COHMAPS_H

#define COHMAPS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QRect>
#include <QSignalMapper>

#include <base/enums.h>
#include <gfx/roiimage.h>
#include <gfx/ccdimages.h>
#include <gfx/coherence.h>

class CohMaps: public CCDImages {
  Q_OBJECT;
public:
  CohMaps(QRect canvas);
  virtual ~CohMaps();
  Coherence *get(QString id);
  Coherence *first();
  void add(QString id, Coherence *img);
  void adjustedRange(QString camid, uint16_t const *data, int X, int Y);
  void newImage(QString camid, uint16_t const *data, int X, int Y,
                Transform const &t);
public slots:
  void setRefTrace(QString achn);
  void setRefStim(class StimulusDef const &s, bool pulse_not_train);
  void setRefDigi(QString digiline);
  void setRefFreq(double fref_hz);
  void setShowMode(SHOWROIS sm);
  void updateSelection(int);
signals:
  void newSelection(int);
private slots:
  void shareSelection(QString);
protected:
  QList<Coherence *> images();
protected:
  QSignalMapper *sm;
};

#endif
