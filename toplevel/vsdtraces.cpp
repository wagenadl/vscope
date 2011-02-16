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
#include <base/roiset.h>
#include <base/analogdata.h>
#include <xml/enumerator.h>
#include <base/ccddata.h>
#include <acq/datatrove.h>
#include <xml/connections.h>

VSDTraces::VSDTraces(QWidget *parent): MultiGraph(parent), timing(0) {
  roiset=0;
  lastAnalog=0;
  lastDigital=0;
  
  selected = new VSDGraph(this);
  selid=0;
  addGraph("sel",selected);

  refgraph = new LineGraph(this);
  addGraph("ref",refgraph);
  reftrace = new TraceInfo(TraceInfo::dataDouble);
  refgraph->addTrace("ref",reftrace);
  refchn = 0;

  allgraph = new VSDAllGraph(&Globals::trove->roidata(), this);
  addGraph("all",allgraph);
  setGraphHeight("sel",100); // pretty arbitrary...
  setGraphHeight("ref",50); // pretty arbitrary...
  connectZooms();
}

VSDTraces::~VSDTraces() {
}

void VSDTraces::selectROI(int id) {
  //  dbg("vsdtraces:selectroi(%i) was %i",id,selid);
  if (id!=selid) {
    selid = id;
    if (selid && roiset) {
      if (roiset->isXYRRA(selid)) 
	selected->setROI(roiset->get(selid));
      else if (roiset->isPoly(selid)) 
	selected->setROI(&roiset->getp(selid));
    } else {
      // Ought we clear our display? Not sure how to do that nicely.
    }
    allgraph->selectROI(selid);
  }
}

void VSDTraces::newROIs(ROISet const *newrois, bool dontTell) {
  roiset = newrois;
  if (roiset->isXYRRA(selid)) 
    selected->setROI(roiset->get(selid));
  else if (roiset->isPoly(selid))
    selected->setROI(&roiset->getp(selid));
  allgraph->clearROIs();
  QSet<int> const &ids = roiset->ids();
  for (QSet<int>::const_iterator i=ids.begin(); i!=ids.end(); i++) {
    int id = *i;
    if (roiset->isXYRRA(id))
      allgraph->setROI(id,roiset->get(id));
    else if (roiset->isPoly(id))
      allgraph->setROI(id,&roiset->getp(id));
  }
  //  saveROIs(); ???
  if (!dontTell) {
    dbg("vsdtraces:newrois: emitting datachanged");
    emit dataChanged();
  }
}

void VSDTraces::editROI(int id) {
  if (roiset) {
    if (roiset->isXYRRA(id)) {
      allgraph->setROI(id, roiset->get(id));
      if (id==selid)
	selected->setROI(roiset->get(id));
    } else if (roiset->isPoly(id)) {
      allgraph->setROI(id, &roiset->getp(id));
      if (id==selid)
	selected->setROI(&roiset->getp(id));
    }
  }
  dbg("vsdtraces:editroi: emitting datachanged");
  emit dataChanged();
  emit roisChanged();
}

void VSDTraces::deleteROI(int id) {
  allgraph->removeROI(id);
  // if (id==selid)
  //   ; // what should we do? not sure.
  dbg("vsdtraces:deleteroi: emitting datachanged");
  emit dataChanged();
  emit roisChanged();
}

void VSDTraces::newCCDData(bool dontTell) {
  dbg("vsdtraces: newccddata");
  QString id_donor = Connections::leaderCamera();
  QString id_acceptor = Connections::findCam(id_donor).partnerid;
  bool has_partner = !id_acceptor.isEmpty();
  Dbg() << "donor: " << id_donor << "; acceptor: " << id_acceptor << ".";
  CCDData const *don = Globals::trove->trial().ccdData(id_donor);
  CCDData const *acc = has_partner
    ? Globals::trove->trial().ccdData(id_acceptor)
    : 0;
  Dbg() << "don="<<don<<" acc="<<acc;
  Dbg() << "nfr="<<don->getNFrames();
  timing.setFrames(don->getNFrames());
  Dbg() <<"hello";
  timing.setTiming(don->getT0(), don->getDT());
  Dbg() <<"world";
  Dbg() << "VSDTraces:: don nfr=" << timing.nframes()
	<< " t0=" << timing.t0_ms() << " dt=" << timing.dt_ms();
  selected->setData(don, acc);
  allgraph->setData(don, acc);
  if (!dontTell) {
    dbg("vsdtraces:newccddata: emitting datachanged");
    emit dataChanged();
  }
}

class ROI3Data *VSDTraces::getData(int id) {
  return allgraph->getData(id);
}

bool VSDTraces::haveData(int id) const {
  return allgraph->haveData(id);
}

void VSDTraces::setDebleach(ROIData::Debleach d) {
  selected->setDebleach(d);
  allgraph->setDebleach(d);
  dbg("vsdtraces:setdebleach %i: emitting datachanged",int(d));
  emit dataChanged();
}

void VSDTraces::setRefTrace(int rf) {
  dbg("vsdtraces::setreftrace: %i",rf);
  refchn = rf;
  QString lbl = Enumerator::find("AICHAN")->reverseLookup(rf);
  refgraph->setTraceLabel("ref",lbl);
  if (Globals::trial)
    newEPhys();
}

void VSDTraces::setRefDigi(int) {
  dbg("VSDTraces::setRefDigi: not yet implemented");
}

void VSDTraces::setRefFreq(double) {
  dbg("VSDTraces::setRefFreq: not yet implemented");
}

void VSDTraces::newEPhys(bool dontTell) {
  dbg("vsdtraces: newephys");
  lastAnalog = Globals::trove->trial().analogData();
  lastDigital = Globals::trove->trial().digitalData(); // we don't use this, but Coherence does
  if (!lastAnalog->contains(refchn)) {
    int rf = lastAnalog->getChannelAtIndex(0);
    if (lastAnalog->contains(rf)) {
      setRefTrace(rf);
      return;
    }
  }
  TraceInfo::DataPtr dp;
  dp.dp_double = lastAnalog->contains(refchn)
    ? lastAnalog->channelData(refchn)
    : lastAnalog->allData();
  dbg("vsdtraces::newephys. refchn=%i contained=%i. data=%p donttell=%i",
      refchn,lastAnalog->contains(refchn), dp.dp_double, dontTell);
  timing.setRate(Globals::ptree->find("acqEphys/acqFreq").toDouble());
  reftrace->setData(0,1/timing.fs_hz(),
		    dp,
		    lastAnalog->getNumScans(),
		    lastAnalog->getNumChannels());
  Range xr = allgraph->getXRange();
  dbg("  xrange = [%g-%g]",xr.min,xr.max);
  refgraph->setXRange(xr);
  refgraph->autoSetYRange();
  refgraph->update();

  if (!dontTell) {
    dbg("vsdtraces:newephys: emitting datachanged");
    emit dataChanged();
  }
}
