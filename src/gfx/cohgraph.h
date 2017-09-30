// gfx/cohgraph.h - This file is part of VScope.
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

// cohgraph.h

#ifndef COHGRAPH_H

#define COHGRAPH_H

#include <base/roidata.h>
#include <gfx/radialgraph.h>
#include <math/cvec.h>
#include <QSet>
#include <QMap>
#include <QPointF>
#include <base/ccdtiming.h>

class CohGraph: public RadialGraph {
  Q_OBJECT;
  /*:C CohGraph
   */
public:
  CohGraph(class CohData *dat, QWidget *parent=0);
  virtual ~CohGraph();
  virtual void showEvent(class QShowEvent *);
  virtual void paintEvent(class QPaintEvent *);
  /*:F paintEvent
   *:D Redraws the current image and any ROIs
   */
public slots:
  void updateData();
  void setRefTrace(QString achn);
  void setRefStim(class StimulusDef const &s, bool pulse_not_train);
  void setRefDigi(QString digiline);
  void setRefFreq(double fref_hz);
  void updateSelection(int id);
protected:
  void mousePressEvent(class QMouseEvent *);
signals:
  void newSelection(int id);
private:
  void perhapsRefresh();
private:
  class CohData *data;
  bool owndata;
  int selectedID;
  QMap<int, QPointF> labelxy;
private: // these will not be implemented
  CohGraph(CohGraph const &other);
  CohGraph &operator=(CohGraph const &other);
};

#endif
