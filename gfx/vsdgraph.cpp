// vsdgraph.cpp

#include "vsdgraph.h"
#include <base/ccddata.h>
#include <base/dbg.h>

VSDGraph::VSDGraph(QWidget *parent): LineGraph(parent) {
  trcDonor = new TraceInfo(TraceInfo::dataDouble);
  trcAcceptor = new TraceInfo(TraceInfo::dataDouble);
  trcRatio = new TraceInfo(TraceInfo::dataDouble);

  addTrace("cc",trcDonor);
  addTrace("ox",trcAcceptor);
  addTrace("rat",trcRatio);
  setTraceLabel("cc","cc");
  setTraceLabel("ox","ox");
  setTraceLabel("rat","ratio");
  setTracePen("cc",QColor("#00ccff")); // blue-green
  setTracePen("ox",QColor("#ff8800")); // orange
  setTracePen("rat",QColor("#000000")); // black

  autoSetXRange();
  //  setAutoRepaint(false);
}

VSDGraph::~VSDGraph() {
  traces.clear(); // make sure the linegraph destructor won't mess with our data,
  delete trcRatio;
  delete trcAcceptor;
  delete trcDonor;
}

void VSDGraph::setROI(XYRRA el) {
  data.setROI(el);
  update();
}

void VSDGraph::setROI(class PolyBlob const *pb) {
  data.setROI(pb);
  update();
}

void VSDGraph::setData(CCDData const *donor,
		       CCDData const *acceptor) {
  data.setData(donor,acceptor);
  t0_ms = donor->getT0();
  dt_ms = donor->getDT();
  if (isXRangeAuto())
    setXRange(Range(t0_ms/1e3,t0_ms/1e3+dt_ms/1e3*data.getNFrames()),true);
  update();
}

void VSDGraph::setDebleach(ROIData::Debleach d) {
  data.setDebleach(d);
  update();
}

void VSDGraph::paintEvent(class QPaintEvent *e) {
  try {
  bool autoRP = getAutoRepaint();
  if (autoRP)
    setAutoRepaint(false);
  // Ensure that our data are up-to-date
  TraceInfo::DataPtr p;
  p.dp_double = data.dataDonor();
  trcDonor->setData(t0_ms/1e3, dt_ms/1e3, p, data.getNFrames());
  p.dp_double = data.dataAcceptor();
  trcAcceptor->setData(t0_ms/1e3, dt_ms/1e3, p, data.getNFrames());
  p.dp_double = data.dataRatio();
  trcRatio->setData(t0_ms/1e3, dt_ms/1e3, p, data.getNFrames());

  dbg("VSDGraph::paintEvent. t0=%g dt=%g n=%i",t0_ms,dt_ms,data.getNFrames());
  
  // Following is not really great, I think it may mess up zoom, but
  // where else can I reasonably do this?
  autoSetYRange(0.05,0.10);
  
  // Let LineGraph draw our stuff.
  LineGraph::paintEvent(e);
  if (autoRP)
    setAutoRepaint(true);
  } catch (...) {
    dbg("exception caught in vsdgraph::paintevent");
  }
}

