// gfx/vsdgraph.h - This file is part of VScope.
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

// vsdgraph.h

#ifndef VSDGRAPH_H

#define VSDGRAPH_H

#include <gfx/linegraph.h>
#include <base/xyrra.h>
#include <base/roidata.h>
#include <base/roi3data.h>

class VSDGraph: public LineGraph {
  /*:C VSDGraph
   *:D A LineGraph derivative that shows donor, acceptor, and ratio traces
       for a single VSD ROI.
  */
  Q_OBJECT;
public:
  VSDGraph(class ROIData3Set *rs3d, QWidget *parent);
  virtual ~VSDGraph();
public slots:
  void updateSelection(int id);
private slots:
  void updateROIs();
  void updateROI(int id);
protected:
  virtual void paintEvent(class QPaintEvent *);
private:
  ROIData3Set *data; // we don't own this
private:
  class TraceInfo *trcDonor, *trcAcceptor, *trcRatio;
  int selid;
};

#endif
