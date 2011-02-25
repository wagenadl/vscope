// vsdgraph.cpp

#include "vsdgraph.h"
#include <base/ccddata.h>
#include <base/dbg.h>
#include <xml/connections.h>
#include <gfx/colors.h>
#include <base/roidata3set.h>

VSDGraph::VSDGraph(ROIData3Set *rs3d, QWidget *parent): LineGraph(parent) {
  data = rs3d;
  trcDonor = trcAcceptor = trcRatio = 0;
  selid = 0;
  trcDonor = new TraceInfo();
  trcAcceptor = new TraceInfo();
  trcRatio = new TraceInfo();
  autoSetXRange();
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

void VSDGraph::updateROI(int id) {
  if (id==selid)
    updateROIs();
}

void VSDGraph::updateROIs() {
  double t0_s = data->getT0_ms()/1e3;
  double dt_s = data->getDT_ms()/1e3;
  int nfr = data->getNFrames();
  if (isXRangeAuto())
    setXRange(Range(t0_s,t0_s+dt_s*nfr),true);
  update();
}

void VSDGraph::updateSelection(int id) {
  if (id==selid)
    return;

  if (data->haveData(selid)) {
    CamPair const &cams = data->getCam(selid);
    addTrace("donor",trcDonor);
    setTraceLabel("donor",cams.donor);
    setTracePen("donor", Colors::find("CCD"+cams.donor, "blue"));
    if (cams.acceptor.isEmpty()) {
      removeTrace("acceptor");
      removeTrace("ratio");
    } else {
      addTrace("acceptor",trcAcceptor);
      setTraceLabel("acceptor",cams.acceptor);
      setTracePen("acceptor",Colors::find("CCD"+cams.acceptor, "red"));
      addTrace("ratio",trcRatio);
      setTraceLabel("ratio","Ratio");
      setTracePen("ratio",QColor("black"));
    }
  } else {
    removeTrace("donor");
    removeTrace("acceptor");
    removeTrace("ratio");
  }
  updateROIs();
}  

void VSDGraph::paintEvent(class QPaintEvent *e) {
  try {
    bool autoRP = getAutoRepaint();
    if (autoRP)
      setAutoRepaint(false);

    double t0_s = data->getT0_ms()/1e3;
    double dt_s = data->getDT_ms()/1e3;
    
    if (data->haveData(selid)) {
      ROI3Data *d3 = data->getData(selid);
      int nfr = d3->getNFrames();
      trcDonor->setData(t0_s, dt_s, DataPtr(d3->dataDonor()), nfr);
      if (d3->dataAcceptor()) {
	trcAcceptor->setData(t0_s, dt_s, DataPtr(d3->dataAcceptor()), nfr);
	trcRatio->setData(t0_s, dt_s, DataPtr(d3->dataRatio()), nfr);
      }
    }
    
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

