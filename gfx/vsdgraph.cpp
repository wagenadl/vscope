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

  connect(data, SIGNAL(changedOne(int)), SLOT(updateROI(int)));
  connect(data, SIGNAL(changedAll()), SLOT(updateROIs()));
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
  Dbg() << "VSDGraph("<<this<<"): updateROI("<<id<<")"; 
  if (id==selid)
    updateROIs();
}

void VSDGraph::updateROIs() {
  Dbg() << "VSDGraph("<<this<<"): updateROIs()";
  if (data->haveData(selid)) {
    if (isXRangeAuto()) {
      Dbg() << " autoranging";
      setXRange(data->getData(selid)->timeRange(), true);
    }
    update();
  }
}

void VSDGraph::updateSelection(int id) {
  Dbg() << "VSDGraph("<<this<<"): updateSelection("<<id<<") was:"<<selid;
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

    if (data->haveData(selid)) {
      ROI3Data *d3 = data->getData(selid);
      trcDonor->setData(d3->getDonorT0ms()/1e3,
			d3->getDonorDTms()/1e3,
			DataPtr(d3->dataDonor()),
			d3->getDonorNFrames());
      if (d3->dataAcceptor()) {
	trcAcceptor->setData(d3->getAcceptorT0ms()/1e3,
			     d3->getAcceptorDTms()/1e3,
			     DataPtr(d3->dataAcceptor()),
			     d3->getAcceptorNFrames());
      trcRatio->setData(d3->getRatioT0ms()/1e3,
			d3->getRatioDTms()/1e3,
			DataPtr(d3->dataRatio()),
			d3->getRatioNFrames());
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

