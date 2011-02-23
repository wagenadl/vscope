// vsdallgraph.cpp

#include "vsdallgraph.h"
#include <base/roi3data.h>
#include <base/roiset3data.h>
#include <base/roidata.h>
#include <base/ccddata.h>
#include <base/dbg.h>
#include <gfx/traceinfo.h>
#include <QMap>
#include <base/base26.h>

const double TRACE_DY = 0.2; // That's the spacing between traces in percents

VSDAllGraph::VSDAllGraph(ROISet3Data *data, QWidget *parent):
  LineGraph(parent), data(data) {
  autoSetXRange();
}

VSDAllGraph::~VSDAllGraph() {
  for (QMap<int, TraceInfo *>::iterator i=traces.begin();
       i!=traces.end(); ++i) 
    delete i.value();
}

void VSDAllGraph::setData(CCDData const *donor,
			  CCDData const *acceptor) {
  data->setData(donor,acceptor);

  if (isXRangeAuto())
    setXRange(Range(data->getT0_ms()/1e3,
		    data->getT0_ms()/1e3
		    +data->getDT_ms()/1e3*donor->getNFrames()),true);
  update();
}

ROI3Data *VSDAllGraph::getData(int id) {
  return data->getData(id);
}

bool VSDAllGraph::haveData(int id) const {
  return data->haveData(id);
}

void VSDAllGraph::setDebleach(ROIData::Debleach d) {
  data->setDebleach(d);
  update();
}

void VSDAllGraph::changeROI(int id) {
  if (data->haveData(id)) {
    if (!traces.contains(id)) {
      traces[id] = new TraceInfo(TraceInfo::dataDouble);
      QString sid = num2az(id);
      addTrace(sid,traces[id]);
      setTraceLabel(sid,sid);
      setTracePen(sid,QColor("#000000"));
    }
  } else {
    if (traces.contains(id))
      delete traces[id];
    traces.remove(id);
  }
  newOffsets();
  update();
}

void VSDAllGraph::clearROIs() {
  // This probably won't last
  for (QMap<int,TraceInfo *>::iterator i=traces.begin();
       i!=traces.end(); ++i) {
    int id = i.key();
    removeTrace(num2az(id));
    delete i.value();
  }
  traces.clear();
  update();
}

void VSDAllGraph::newOffsets() {
  double offset=TRACE_DY*traces.size();
  foreach (TraceInfo *tr, traces.values()) 
    tr->setOffset(offset-=TRACE_DY);
}

void VSDAllGraph::selectROI(int id) {
  if (id!=selectedId) {
    setTracePen(num2az(selectedId),QColor("#000000"));
    selectedId = id;
    setTracePen(num2az(selectedId),QColor("#ff0000"));
  }
  update();
}

void VSDAllGraph::paintEvent(class QPaintEvent *e) {
  bool autoRP = getAutoRepaint();
  if (autoRP)
    setAutoRepaint(false);
  // Ensure that our data are up-to-date
  TraceInfo::DataPtr p;
  for (QMap<int, TraceInfo *>::iterator tri=traces.begin();
       tri!=traces.end(); ++tri) {
    int id = tri.key();
    TraceInfo *trc = tri.value();
    ROI3Data *dat = data->getData(id);
    if (!dat)
      continue;
    p.dp_double = dat->dataRatio();
    trc->setData(data->getT0_ms()/1e3, data->getDT_ms()/1e3,
		 p, dat->getNFrames());

    //    dbg("VSDAllGraph::paintEvent. t0=%g dt=%g n=%i",
    //	t0_ms,dt_ms,i.value()->getNFrames());
  }
  
  // Following is not really great, I think it may mess up zoom, but
  // where else can I reasonably do this?
  autoSetYRange(0.05,0.10);
  
  // Let LineGraph draw our stuff.
  LineGraph::paintEvent(e);
  if (autoRP)
    setAutoRepaint(true);
}

