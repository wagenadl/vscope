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
}

VSDAllGraph::~VSDAllGraph() {
  foreach (TraceInfo *tr, traces.values())
    delete tr;
}

void VSDAllGraph::updateData() {
  if (isXRangeAuto())
    setXRange(Range(data->getT0_ms()/1e3,
		    data->getT0_ms()/1e3
		    + data->getDT_ms()/1e3*data->getNFrames()),
	      true);
  update();
}

ROI3Data *VSDAllGraph::getData(int id) {
  return data->getData(id);
}

bool VSDAllGraph::haveData(int id) const {
  return data->haveData(id);
}

void VSDAllGraph::updateROIs() {
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
  if (id!=selectedId) {
    if (selectedId)
      setTracePen(num2az(selectedId),QColor("#000000"));
    selectedId = traces.contains(id) ? id : 0;
    if (selectedId)
      setTracePen(num2az(selectedId),QColor("#ff0000"));
  }
  update();
}

void VSDAllGraph::paintEvent(class QPaintEvent *e) {
  bool autoRP = getAutoRepaint();
  if (autoRP)
    setAutoRepaint(false);

  foreach (int id, traces.keys()) {
    TraceInfo *trc = traces[id];
    if (!data->haveData(id))
      continue; // this should not happen
    ROI3Data *dat = data->getData(id);
    trc->setData(data->getT0_ms()/1e3, data->getDT_ms()/1e3,
		 DataPtr(dat->dataRatio()), dat->getNFrames());
  }
  
  // Following is not really great, I think it may mess up zoom, but
  // where else can I reasonably do this?
  autoSetYRange(0.05,0.10);
  
  // Let LineGraph draw our stuff.
  LineGraph::paintEvent(e);
  if (autoRP)
    setAutoRepaint(true);
}

