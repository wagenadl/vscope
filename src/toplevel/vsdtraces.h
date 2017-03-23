// toplevel/vsdtraces.h - This file is part of VScope.
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

// vsdtraces.h

#ifndef VSDTRACES_H

#define VSDTRACES_H

#include <gfx/multigraph.h>
#include <base/xyrra.h>
#include <base/roidata.h>
#include <QMap>
#include <gfx/vsdallgraph.h>
#include <base/ccdtiming.h>

class VSDTraces: public MultiGraph {
  /*:C VSDTraces
   *:D MultiGraph showing:
       (1) The cc, ox, and rat traces of the currently selected ROI
       (2) The rat traces of all ROIs (current highlighted in red?)
       (3) (?) Intracellular voltage [as a tiny]
       (4) (?) Stimuli [as a tiny]
   *:N For now, only (1, 2) are implemented.
  */
  Q_OBJECT;
public:
  VSDTraces(QWidget *parent);
  virtual ~VSDTraces();
public slots:
  void updateSelection(int id);
  void setRefTrace(QString chn);
  void setRefDigi(QString chn);
  void setRefFreq(double frqhz);
  void setRefStim(class StimulusDef const &s, bool pulse_not_train);
private slots:
  void updateEPhysData();
private:
  class VSDGraph *selected;
  class VSDAllGraph *allgraph;
  class LineGraph *refgraph;
  class TraceInfo *reftrace;
  int selid;
  QString refchn;
};

#endif
