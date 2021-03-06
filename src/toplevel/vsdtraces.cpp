// toplevel/vsdtraces.cpp - This file is part of VScope.
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
  connect(&Globals::trove->trial(), SIGNAL(newData()),
	  SLOT(updateEPhysData()));
  Dbg(this) << "connected " << &Globals::trove->trial() << ":newData to my updateEphysData";

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
  //  Dbg() << "vsdtraces::setreftrace: " << id;
  refchn = id;
  refgraph->setTraceLabel("ref", Aliases::lookup(id));
  updateEPhysData();
}

void VSDTraces::setRefDigi(QString) {
  dbg("VSDTraces::setRefDigi: not yet implemented (but don't worry)");
}

void VSDTraces::setRefFreq(double) {
  dbg("VSDTraces::setRefFreq: not yet implemented (but don't worry)");
}

void VSDTraces::setRefStim(StimulusDef const &, bool) {
  dbg("VSDTraces::setRefStim: not yet implemented (but don't worry)");
}

void VSDTraces::updateEPhysData() {
  AnalogData const *adata = Globals::trove->trial().analogData();
  //if (!adata->contains(refchn)) {
  //  QString rf = adata->getChannelAtIndex(0);
  //  if (adata->contains(rf)) {
  //    setRefTrace(rf);
  //    return;
  //  }
  //}
  if (adata->contains(refchn)) {
    DataPtr dp(adata->channelData(refchn));
    int idx = adata->whereIsChannel(refchn);
    double fshz = Globals::trove->trial().paramTree()
      ->find("acqEphys/acqFreq").toDouble();
    reftrace->setData(0, 1/fshz,
		      dp,
		      adata->getNumScans(),
		      adata->getNumChannels());
    reftrace->setScaleFactor(adata->getScaleAtIndex(idx));
    reftrace->setOffset(adata->getOffsetAtIndex(idx));
    refgraph->setYLabel("("+adata->getUnitAtIndex(idx)+")");
  } else {
    DataPtr dp(adata->allData());
    reftrace->setData(0,1e4,dp,0,1);
    refgraph->setYLabel("(-)");
  }
  refgraph->autoSetYRange();
  Range xr = allgraph->computeXRange();
  xr.expand(refgraph->computeXRange());
  zoomRequest(xr);
}
