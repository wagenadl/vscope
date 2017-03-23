// gfx/vsdallgraph.h - This file is part of VScope.
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

// vsdallgraph.h

#ifndef VSDALLGRAPH_H

#define VSDALLGRAPH_H

#include <QMap>
#include <gfx/linegraph.h>
#include <base/xyrra.h>
#include <base/roidata.h>

class VSDAllGraph: public LineGraph {
  /*:C VSDAllGraph
   *:D A LineGraph derivative that shows (just) the ratio trace for
       a whole bunch of ROIs.
   *:N See also VSDGraph.
   */
  Q_OBJECT;
public:
  VSDAllGraph(class ROIData3Set *rs3d, QWidget *parent=0);
  virtual ~VSDAllGraph();
  class ROI3Data const *getData(int id);
  bool haveData(int id) const;
public slots:
  void updateSelection(int id);
  void updateData();
private slots:
  void updateROI(int id);
  void updateROIs();
protected:
  virtual void paintEvent(class QPaintEvent *);
private:
  void updateROIcore(int id);
  void newOffsets(); // recalculates the y offsets of each trace
private:
  class ROIData3Set *data; // we do not own this
  QMap<int, class TraceInfo *> traces;
  int selectedId;
  int halfMaxVisible; // when a trace is selected, we show it and up to 
                      // halfMaxVisible others before and after it.
private:
  VSDAllGraph(VSDAllGraph const &other);
  VSDAllGraph &operator=(VSDAllGraph const &other);
};

#endif
