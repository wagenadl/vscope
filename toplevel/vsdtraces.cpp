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

VSDTraces::VSDTraces(QWidget *parent): MultiGraph(parent) {
  lastAnalog=0;
  lastDigital=0;
  
  selected = new VSDGraph(&Globals::trove->roidata(), this);
  selid=0;
  addGraph("sel",selected);

  refgraph = new LineGraph(this);
  addGraph("ref",refgraph);
  reftrace = new TraceInfo();
  refgraph->addTrace("ref",reftrace);
  refchn = 0;
  refgraph->autoSetXRange();
  refgraph->autoSetYRange();
  connect(&Globals::trove->trial(), SIGNAL(newData()), SLOT(updateEPhysData()));

  allgraph = new VSDAllGraph(&Globals::trove->roidata(), this);
  addGraph("all",allgraph);
  setGraphHeight("sel",100); // pretty arbitrary...
  setGraphHeight("ref",50); // pretty arbitrary...
  connectZooms();
}

VSDTraces::~VSDTraces() {
}

void VSDTraces::updateSelection(int id) {
  selected->updateSelection(id);
  allgraph->updateSelection(id);
}

void VSDTraces::setRefTrace(int rf) {
  dbg("vsdtraces::setreftrace: %i",rf);
  refchn = rf;
  QString lbl = Enumerator::find("AICHAN")->reverseLookup(rf);
  refgraph->setTraceLabel("ref",lbl);
  updateEPhysData();
}

void VSDTraces::setRefDigi(int) {
  dbg("VSDTraces::setRefDigi: not yet implemented");
}

void VSDTraces::setRefFreq(double) {
  dbg("VSDTraces::setRefFreq: not yet implemented");
}

void VSDTraces::updateEPhysData() {
  lastAnalog = Globals::trove->trial().analogData();
  lastDigital = Globals::trove->trial().digitalData(); // we don't use this, but Coherence does
  if (!lastAnalog->contains(refchn)) {
    int rf = lastAnalog->getChannelAtIndex(0);
    if (lastAnalog->contains(rf)) {
      setRefTrace(rf);
      return;
    }
  }
  DataPtr dp(lastAnalog->contains(refchn)
	     ? lastAnalog->channelData(refchn)
	     : lastAnalog->allData());
  dbg("vsdtraces::newephys. refchn=%i contained=%i. data=%p",
      refchn,lastAnalog->contains(refchn), dp.ptr.dp_double);
  reftrace->setData(0,1/Globals::ptree->find("acqEphys/acqFreq").toDouble(),
		    dp,
		    lastAnalog->getNumScans(),
		    lastAnalog->getNumChannels());
  Range xr = allgraph->getXRange();
  dbg("  xrange = [%g:%g]",xr.min,xr.max);
  refgraph->setXRange(xr);
  refgraph->autoSetYRange();
  refgraph->update();
}
