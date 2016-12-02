// mgauto.cpp

#include "mgauto.h"

#include <base/analogdata.h>
#include <base/digitaldata.h>
#include <gfx/linegraph.h>
#include <gfx/traceinfo.h>
#include <toplevel/globals.h>
#include <xml/enumerator.h>
#include <xml/connections.h>
#include <xml/paramtree.h>
#include <xml/param.h>
#include <xml/aliases.h>
#include <acq/datatrove.h>
#include <acq/trialdata.h>
#include <acq/ephysout.h>
#include <base/dbg.h>
#include <xml/attribute.h>
#include <xml/outrate.h>


MGAuto::MGAuto(QWidget *parent, QString myname):
  MultiGraph(parent), myname(myname) {
  rebuild();
  connectZooms();
}

MGAuto::~MGAuto() {
  for (QMap<QString, TraceInfo*>::iterator i=traces.begin();
       i!=traces.end(); ++i)
    delete i.value();
}

bool MGAuto::useInputChannel(QString id) const {
  if (id=="")
    return false;
  if (id.startsWith("E") && id.right(1)>="0" && id.right(1)<="9")
    return myname == "extra";
  else if (id.startsWith("Frame:"))
    return myname == "extra";
  else
    return myname == "intra";
}

bool MGAuto::useOutputChannel(QString id) const {
  if (id=="")
    return false;
  if (myname!="stim")
    return false;

  ParamTree *ptree = Globals::trove->trial().myParamTree();
  if (!ptree)
    ptree = Globals::ptree;
  
  if (id.contains("Vid"))
    return ptree->enabled("stimVideo");
  if (id.startsWith("Lamp:"))
    return ptree->enabled("acqCCD");
  if (id.startsWith("Shutter:") || id.startsWith("Trigger:")) {
    QStringList bits = id.split(":");
    QString camid = bits.last();
    return ptree->enabled("acqCCD/camera:" + camid);
  }
  return ptree->enabled("stimEphys/channel:" + id);
}

void MGAuto::rebuild() {
  QList<QString> newlist;
  QSet<QString> digi;
  QSet<QString> outp;
  AnalogData const *ad = Globals::trove->trial().analogData();
  if (ad) {
    for (int i=0; i<ad->getNumChannels(); i++) {
      QString id = ad->getChannelAtIndex(i);
      if (useInputChannel(id)) {
	newlist << id;
      }
    }
  }

  DigitalData const *dd = Globals::trove->trial().digitalData();
  if (dd) {
    for (int i=0; i<32; i++) {
      QString id = dd->lineID(i);
      if (useInputChannel(id)) {
	newlist << id;
	digi << id;
      }
    }
  }

  AnalogData const *as = Globals::trove->trial().analogStimuli();
  Dbg() << "mgauto as=" << as;
  if (as) {
    for (int i=0; i<as->getNumChannels(); i++) {
      QString id = as->getChannelAtIndex(i);
      Dbg() << "mgauto as:" << i << ": " << id << ": " << useOutputChannel(id);
      if (useOutputChannel(id)) {
	newlist << id;
	outp << id;
      }
    }
  }

  DigitalData const *ds = Globals::trove->trial().digitalStimuli();
  if (ds) {
    for (int i=0; i<32; i++) {
      QString id = ds->lineID(i);
      if (useOutputChannel(id)) {
	newlist << id;
	digi << id;
	outp << id;
      }
    }
  }
  
  Dbg() << "mgauto:" << myname << " newlist=" << QStringList(newlist).join(",")
	<< " digi=" << QStringList(digi.toList()).join(",")
	<< " outp=" << QStringList(outp.toList()).join(",")
	<< " actual=" << QStringList(actual).join(",");
  
  if (newlist!=actual) {
    actual = newlist;
    digiSet = digi;
    outputSet = outp;
    newgraphs();
  }
  
  newtraces();
  update();
}

void MGAuto::newgraphs() {
  // delete graphs
  deleteAll();

  // delete traces
  for (QMap<QString, TraceInfo*>::iterator i=traces.begin();
       i!=traces.end(); ++i)
    delete i.value();
  traces.clear();

  // find out if all tiny
  bool alltiny = digiSet.size() == actual.size();
    
  // Build new graphs
  bool first=true;
  foreach (QString id, actual) {
    LineGraph *g = new LineGraph(this);
    if (!digiSet.contains(id) || (alltiny && first) )
      g->setXLabel("(s)");
    else
      g->showXTickLabels(false);
    g->setTraceLabel(id, Aliases::lookup(id));
    addGraph(id, g, digiSet.contains(id));
    first=false;
  }
}

void MGAuto::newtraces() {
  AnalogData const *aacq = Globals::trove->trial().analogData();
  DigitalData const *dacq = Globals::trove->trial().digitalData();
  AnalogData const *astim = Globals::trove->trial().analogStimuli();
  DigitalData const *dstim = Globals::trove->trial().digitalStimuli();
  double outrate_hz = Globals::trove->trial().paramTree()
    ->find(PAR_OUTRATE).toDouble();
  double inrate_hz = Globals::trove->trial().paramTree()
    ->find("acqEphys/acqFreq").toDouble();

  foreach (QString id, actual) {
    LineGraph *g = findp(id);
    TraceInfo *tr = traces[id];
    if (!tr)	  
      traces[id] = tr = new TraceInfo();
    if (outputSet.contains(id)) {
      // output
      if (digiSet.contains(id)) {
	// digital output
	tr->setData(0, 1/outrate_hz,
		    DataPtr(dstim->allData(),dstim->findLine(id)),
		    dstim->getNumScans());
      } else {
	// analog output
	tr->setData(0, 1/outrate_hz,
		    DataPtr(astim->channelData(id)),
		    astim->getNumScans(),
		    astim->getNumChannels());
	tr->setScaleFactor(astim->getScaleAtIndex(astim->whereIsChannel(id)));
	tr->setOffset(astim->getOffsetAtIndex(astim->whereIsChannel(id)));
	g->setYLabel("("+astim->getUnitAtIndex(astim->whereIsChannel(id))+")");
      }
    } else {
      // input
      if (digiSet.contains(id)) {
	// digital input
	tr->setData(0,1/inrate_hz,
		    DataPtr(dacq->allData(),dacq->findLine(id)),
		    dacq->getNumScans());
      } else {
	// analog input
	tr->setData(0, 1/inrate_hz,
		    DataPtr(aacq->channelData(id)),
		    aacq->getNumScans(),
		    aacq->getNumChannels());
	tr->setScaleFactor(aacq->getScaleAtIndex(aacq->whereIsChannel(id)));
	tr->setOffset(aacq->getOffsetAtIndex(aacq->whereIsChannel(id)));
	g->setYLabel("("+aacq->getUnitAtIndex(aacq->whereIsChannel(id))+")");
      }
    }

    g->addTrace(id, tr);
    g->autoSetXRange();
    if (digiSet.contains(id)) {
      g->setYRange(Range(-0.1, 1.1));
      g->setYTicks(100, 100);
      g->showYTickLabels(false);
    } else {
      g->autoSetYRange(0.001, 0.5);
    }
  }
}
