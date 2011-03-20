// vsdtraces.cpp

#include "vsdtraces.h"

#include <gfx/vsdgraph.h>
#include <gfx/vsdallgraph.h>
#include <toplevel/globals.h>
#include <gfx/ccdimage.h>
#include <acq/trial.h>
#include <base/dbg.h>
#include <xml/paramtree.h>
#include <base/xml.h>
#include <toplevel/exptlog.h>
#include <QDir>

#include <base/analogdata.h>
#include <xml/enumerator.h>
#include <base/ccddata.h>
#include <acq/datatrove.h>
#include <xml/connections.h>
#include <xml/aliases.h>

VSDTraces::VSDTraces(QWidget *parent): MultiGraph(parent) {
  selected = new VSDGraph(&Globals::trove->roidata(), this);
  selid=0;
  addGraph("sel",selected);

  refgraph = new LineGraph(this);
  addGraph("ref",refgraph);
  reftrace = new TraceInfo();
  refgraph->addTrace("ref",reftrace);
  refgraph->autoSetXRange();
  refgraph->autoSetYRange();
  connect(&Globals::trove->trial(), SIGNAL(newData()), SLOT(updateEPhysData()));

  allgraph = new VSDAllGraph(&Globals::trove->roidata(), this);
  addGraph("all",allgraph);
  setGraphHeight("sel",100); // pretty arbitrary...
  setGraphHeight("ref",100); // pretty arbitrary... was 50
  connectZooms();
}

VSDTraces::~VSDTraces() {
}

void VSDTraces::updateSelection(int id) {
  selected->updateSelection(id);
  allgraph->updateSelection(id);
}

void VSDTraces::setRefTrace(QString id) {
  Dbg() << "vsdtraces::setreftrace: " << id;
  refchn = id;
  refgraph->setTraceLabel("ref", Aliases::lookup(id));
  updateEPhysData();
}

void VSDTraces::setRefDigi(int) {
  dbg("VSDTraces::setRefDigi: not yet implemented");
}

void VSDTraces::setRefFreq(double) {
  dbg("VSDTraces::setRefFreq: not yet implemented");
}

void VSDTraces::updateEPhysData() {
  Dbg() << "VSDTraces::updateEphysData";
  AnalogData const *adata = Globals::trove->trial().analogData();
  //if (!adata->contains(refchn)) {
  //  QString rf = adata->getChannelAtIndex(0);
  //  if (adata->contains(rf)) {
  //    setRefTrace(rf);
  //    return;
  //  }
  //}
  DataPtr dp(adata->contains(refchn)
	     ? adata->channelData(refchn)
	     : adata->allData());
  reftrace->setData(0,1/Globals::ptree->find("acqEphys/acqFreq").toDouble(),
		    dp,
		    adata->getNumScans(),
		    adata->getNumChannels());
  Connections::AIChannel const *aich = Connections::findpAI(refchn);
  if (aich) {
    reftrace->setScaleFactor(aich->scale);
    refgraph->setYLabel("("+aich->unit+")");
  } else {
    reftrace->setScaleFactor(1);
    refgraph->setYLabel("(a.u.)");
  }
  refgraph->autoSetYRange();
  Range xr = allgraph->computeXRange();
  xr.expand(refgraph->computeXRange());
  //dbg("  vsdtraces: xrange = [%g:%g]",xr.min,xr.max);
  zoomRequest(xr);
}
