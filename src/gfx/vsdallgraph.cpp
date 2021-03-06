// gfx/vsdallgraph.cpp - This file is part of VScope.
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

// vsdallgraph.cpp

#include "vsdallgraph.h"
#include <base/roi3data.h>
#include <base/roidata3set.h>
#include <base/roidata.h>
#include <base/ccddata.h>
#include <base/dbg.h>
#include <gfx/traceinfo.h>
#include <QMap>
#include <base/base26.h>

const double TRACE_DY = 0.2; // That's the spacing between traces in percents

VSDAllGraph::VSDAllGraph(ROIData3Set *data, QWidget *parent):
  LineGraph(parent), data(data) {
  autoSetXRange();
  connect(data, SIGNAL(newDatum(int)), SLOT(updateROI(int)));
  connect(data, SIGNAL(newAll()), SLOT(updateROIs()));
  connect(data, SIGNAL(newAll()), SLOT(updateData()));
  halfMaxVisible = 8;
}

VSDAllGraph::~VSDAllGraph() {
  foreach (TraceInfo *tr, traces.values())
    delete tr;
}

void VSDAllGraph::updateData() {
  // Dbg() << "VSDAllGraph("<<this<<"): updateData() data="<<data;
  Range tt;
  if (!data)
    return;
  if (isXRangeAuto())
    autoSetXRange();
  //    Range tt;
  //    foreach (int id, data->allIDs()) 
  //      tt.expand(data->getData(id)->timeRange());
  //    setXRange(tt, true);
  update();
}

ROI3Data const *VSDAllGraph::getData(int id) {
  return data->getData(id);
}

bool VSDAllGraph::haveData(int id) const {
  return data->haveData(id);
}

void VSDAllGraph::updateROIs() {
  // Dbg() << "VSDAllGraph("<<this<<"): updateROIs()";
  QSet<int> ids = QSet<int>::fromList(data->allIDs());
  ids += QSet<int>::fromList(traces.keys());
  foreach (int id, ids) 
    updateROIcore(id);
  newOffsets();
  update();
}

void VSDAllGraph::updateROIcore(int id) {
  bool exists = data->haveData(id);
  bool existed = traces.contains(id);
  if (exists && !existed) {
    traces[id] = new TraceInfo();
    QString sid = num2az(id);
    addTrace(sid,traces[id]);
    setTraceLabel(sid,sid);
    setTracePen(sid,QColor("#000000"));
  } else if (existed && !exists) {
    removeTrace(num2az(id));
    delete traces[id];
    traces.remove(id);
  }
}  

void VSDAllGraph::updateROI(int id) {
  // Dbg() << "VSDAllGraph("<<this<<"): updateROI("<<id<<")";
  updateROIcore(id);
  newOffsets();
  update();
}

void VSDAllGraph::newOffsets() {
  double offset=TRACE_DY*traces.size();
  foreach (TraceInfo *tr, traces.values()) 
    tr->setOffset(offset-=TRACE_DY);
}

void VSDAllGraph::updateSelection(int id) {
  Dbg() << "VSDAllGraph("<<this<<"): updateSelection("<<id<<") was:"<<selectedId;
  if (selectedId)
    setTracePen(num2az(selectedId),QColor("#000000"));
  selectedId = traces.contains(id) ? id : 0;
  if (selectedId)
    setTracePen(num2az(selectedId),QColor("#ff0000"));
  if (selectedId) {
    foreach (int id, traces.keys()) {
      if (id>=selectedId-halfMaxVisible && id<=selectedId+halfMaxVisible)
	showTrace(num2az(id));
      else
	hideTrace(num2az(id));
    }
  } else {
    showAllTraces();
  }
  update();
}

void VSDAllGraph::paintEvent(class QPaintEvent *e) {
  bool autoRP = getAutoRepaint();
  if (autoRP)
    setAutoRepaint(false);

  Dbg() << "vsdallgraph::paintevent";
  foreach (int id, traces.keys()) {
    TraceInfo *trc = traces[id];
    if (!data->haveData(id)) {
      Dbg() << "  missing data for " << id;
      continue; // this should not happen
    }
    ROI3Data const *dat = data->getData(id);
    trc->setData(dat->getRatioT0ms()/1e3, dat->getRatioDTms()/1e3,
		 DataPtr(dat->dataRatio()), dat->getRatioNFrames());
  }
  
  // Following is not really great, I think it may mess up zoom, but
  // where else can I reasonably do this?
  autoSetYRange(0.0,0.10);
  
  // Let LineGraph draw our stuff.
  LineGraph::paintEvent(e);
  if (autoRP)
    setAutoRepaint(true);
}

