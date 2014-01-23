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

  connect(data, SIGNAL(newDatum(int)), SLOT(updateROI(int)));
  connect(data, SIGNAL(newAll()), SLOT(updateROIs()));
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
  //Dbg() << "VSDGraph("<<this<<"): updateROI("<<id<<")"; 
  if (id==selid)
    updateROIs();
}

void VSDGraph::updateROIs() {
  //Dbg() << "VSDGraph("<<this<<"): updateROIs()";
  if (data->haveData(selid)) {
    if (isXRangeAuto()) {
      //Dbg() << " autoranging";
      setXRange(data->getData(selid)->timeRange(), true);
    }
    update();
  }
}

void VSDGraph::updateSelection(int id) {
  //Dbg() << "VSDGraph("<<this<<"): updateSelection("<<id<<") was:"<<selid;
  selid = id;
  if (data->haveData(selid)) {
    CamPair const &cams = data->getCam(selid);
    ROI3Data const *d3 = data->getData(selid);
    bool plotDonor = d3->getDonorNFrames();
    bool plotAcceptor = d3->getAcceptorNFrames();
    bool plotRatio = d3->getRatioNFrames() && plotDonor && plotAcceptor;
    if (plotDonor) {
      addTrace("donor", trcDonor);
      setTraceLabel("donor", cams.donor);
      setTracePen("donor", Colors::find("CCD"+cams.donor, "blue"));
    } else {
      removeTrace("donor");
    }
    if (plotAcceptor) {
      addTrace("acceptor", trcAcceptor);
      setTraceLabel("acceptor", cams.acceptor);
      setTracePen("acceptor", Colors::find("CCD"+cams.acceptor, "red"));
    } else {
      removeTrace("acceptor");
    }
    if (plotRatio) {
      addTrace("ratio", trcRatio);
      setTraceLabel("ratio", "Ratio");
      setTracePen("ratio", QColor("black"));
    } else {
      removeTrace("ratio");
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
      ROI3Data const *d3 = data->getData(selid);
      bool plotDonor = d3->getDonorNFrames();
      bool plotAcceptor = d3->getAcceptorNFrames();
      bool plotRatio = d3->getRatioNFrames() && plotDonor && plotAcceptor;
      if (plotDonor)
	trcDonor->setData(d3->getDonorT0ms()/1e3
			  + d3->getDonorDurms()/2e3,
			  d3->getDonorDTms()/1e3,
			  DataPtr(d3->dataDonor()),
			  d3->getDonorNFrames());
      if (plotAcceptor)
	trcAcceptor->setData(d3->getAcceptorT0ms()/1e3
			     + d3->getAcceptorDurms()/2e3,
			     d3->getAcceptorDTms()/1e3,
			     DataPtr(d3->dataAcceptor()),
			     d3->getAcceptorNFrames());
      if (plotRatio)
	trcRatio->setData(d3->getRatioT0ms()/1e3
			  + d3->getRatioDurms()/2e3,
			  d3->getRatioDTms()/1e3,
			  DataPtr(d3->dataRatio()),
			  d3->getRatioNFrames());
    }
    
    // Following is not really great, I think it may mess up zoom, but
    // where else can I reasonably do this?
    autoSetYRange(0.0,0.10);
    
    // Let LineGraph draw our stuff.
    LineGraph::paintEvent(e);
    if (autoRP)
      setAutoRepaint(true);
  } catch (...) {
    dbg("exception caught in vsdgraph::paintevent");
  }
}

