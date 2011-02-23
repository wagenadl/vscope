// vsdgraph.cpp

#include "vsdgraph.h"
#include <base/ccddata.h>
#include <base/dbg.h>
#include <xml/connections.h>
#include <gfx/colors.h>

VSDGraph::VSDGraph(QWidget *parent): LineGraph(parent) {
  trcDonor = trcAcceptor = trcRatio = 0;

  QString id_donor = Connections::leaderCamera();
  QString id_acceptor = Connections::findCam(id_donor).partnerid;
  bool has_partner = !id_acceptor.isEmpty();

  Dbg() << "VSDGraph: id_donor="<<id_donor;
  Dbg() << "VSDGraph: id_acceptor="<<id_acceptor;
  
  trcDonor = new TraceInfo(TraceInfo::dataDouble);
  
  addTrace(id_donor,trcDonor);
  setTraceLabel(id_donor,id_donor);
  setTracePen(id_donor, Colors::find("CCD"+id_donor,"black"));

  if (has_partner) {
    trcAcceptor = new TraceInfo(TraceInfo::dataDouble);
    addTrace(id_acceptor,trcAcceptor);
    setTraceLabel(id_acceptor,id_acceptor);
    setTracePen(id_acceptor,Colors::find("CCD"+id_acceptor,"black"));
    
    trcRatio = new TraceInfo(TraceInfo::dataDouble);
    addTrace("rat",trcRatio);
    setTraceLabel("rat","Ratio");
    setTracePen("rat",QColor("#000000")); // black
  }
  
  autoSetXRange();
  //  setAutoRepaint(false);
}

VSDGraph::~VSDGraph() {
  traces.clear(); // make sure the linegraph destructor won't mess with our data
  if (trcRatio)
    delete trcRatio;
  if (trcAcceptor)
    delete trcAcceptor;
  if (trcDonor)
    delete trcDonor;
}

void VSDGraph::setROI(ROICoords const *roi) {
  data.setROI(roi);
  update();
}
void VSDGraph::setData(CCDData const *donor,
		       CCDData const *acceptor) {
  data.setData(donor, acceptor);
  t0_ms = donor ? donor->getT0() : 0;
  dt_ms = donor ? donor->getDT() : 1;
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
    if (trcDonor)
      trcDonor->setData(t0_ms/1e3, dt_ms/1e3, p, data.getNFrames());
    p.dp_double = data.dataAcceptor();
    if (trcAcceptor)
      trcAcceptor->setData(t0_ms/1e3, dt_ms/1e3, p, data.getNFrames());
    p.dp_double = (data.dataDonor() && data.dataAcceptor())
      ? data.dataRatio() : 0;
    if (trcRatio)
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

