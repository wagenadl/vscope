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

#define PAR_OUTRATE "acqEphys/acqFreq"
// Alternative: "stimEphys/outrate"

MGAuto::Channel::Channel(QString cid, QString ctyp, QString ctiny) {
  id = cid;
  label = id;
  if (ctyp=="ai") {
    typ=AI;
    chn = Enumerator::find("AICHAN")->lookup(id);
    label = Aliases::lookup(id);
    isOut = false;
    isDigi = false;
  } else if (ctyp=="ao") {
    typ=AO;
    chn = Enumerator::find("AOCHAN")->lookup(id);
    isOut = true;
    isDigi = false;
  } else if (ctyp=="di") {
    typ=DI;
    chn = Enumerator::find("DIGILINES")->lookup(id);
    label = Aliases::lookup(id);
    isOut = false;
    isDigi = true;
  } else if (ctyp=="do") {
    typ=DO;
    chn = Enumerator::find("DIGILINES")->lookup(id);
    label = Aliases::lookup(id);
    isOut = true;
    isDigi = true;
  } else
    throw Exception("MGAuto::Channel","Unknown type '"+ctyp+"'",
		    "constructor");

  tiny = ctiny=="yes" || ctiny=="true";
}

bool MGAuto::Channel::available(QString cid, QString ctyp) {
  if (ctyp=="ai")
    return Enumerator::find("AICHAN")->has(cid);
  else if (ctyp=="ao")
    return Enumerator::find("AOCHAN")->has(cid);
  else if (ctyp=="di" || ctyp=="do")
    return Enumerator::find("DIGILINES")->has(cid);
  else
    return false;
}  

MGAuto::MGAuto(QWidget *parent, QDomElement conf, QString myname):
  MultiGraph(parent) {
  bool ok=false;
  for (QDomElement elt=conf.firstChildElement("multigraph");
       !elt.isNull(); elt=elt.nextSiblingElement("multigraph")) {
    QString id = elt.attribute("id");
    if (id==myname) {
      for (QDomElement c=elt.firstChildElement("graph");
	   !c.isNull(); c=c.nextSiblingElement("graph")) 
	if (Channel::available(c.attribute("ch"), c.attribute("typ"))) 
	  pool.push_back(Channel(c.attribute("ch"), c.attribute("typ"),
				 c.attribute("tiny")));
      ok=true;
    }
  }
  if (!ok)
    throw Exception("MGAuto",
		    "No multigraph definition found for '"+myname+"' in xml",
		    "constructor");
  rebuild();
  connectZooms();
}

MGAuto::~MGAuto() {
  for (QMap<QString, TraceInfo*>::iterator i=traces.begin();
       i!=traces.end(); ++i)
    delete i.value();
}

void MGAuto::rebuild() {
  QSet<QString> newset;
  for (QList<Channel>::iterator i=pool.begin(); i!=pool.end(); ++i) {
    QString id = (*i).id;
    int chn = (*i).chn;
    switch ((*i).typ) {
    case Channel::AI:
      if (Globals::trove->trial().analogData() &&
	  Globals::trove->trial().analogData()->contains(chn)) {
	newset.insert(id);
      } else {
	;
      }
      break;
    case Channel::AO:
      if (Globals::ptree->find(QString("stimEphys/channel:%1/enable")
			       .arg(id)).toBool())
	newset.insert(id);
      break;
    case Channel::DI:
      newset.insert(id); // digital inputs are always enabled
      break;
    case Channel::DO:
      if (id.left(2)=="DO") {
	// numbered output channel
	if (Globals::ptree->find(QString("stimEphys/channel:%1/enable")
					 .arg(id)).toBool())
	  newset.insert(id);
      } else {
	// non-DOx channels
	if (Globals::trove->trial().digitalStimuli()) {
	  uint32_t mask = Globals::trove->trial().digitalStimuli()->getMask();
	  uint32_t one = 1;
	  if (mask & (one<<chn))
	    newset.insert(id);
	}
      }
    }
  }

  if (newset!=actual) {
    actual = newset;
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
  bool alltiny=true;
  for (QList<Channel>::iterator i=pool.begin(); i!=pool.end(); ++i)
    if (actual.contains((*i).id))
      if (!(*i).tiny)
	alltiny=false;
    
  // Build new graphs
  bool first=true;
  for (QList<Channel>::iterator i=pool.begin(); i!=pool.end(); ++i) {
    QString id = (*i).id;
    if (actual.contains(id)) {
      LineGraph *g = new LineGraph(this);
      if (!(*i).tiny || (alltiny && first) )
	g->setXLabel("(s)");
      else
	g->showXTickLabels(false);
      g->setTraceLabel(id,(*i).label);
      addGraph(id,g, (*i).tiny);
      first=false;
    }
  }  
}

void MGAuto::newtraces() {
  AnalogData const *aacq = Globals::trove->trial().analogData();
  DigitalData const *dacq = Globals::trove->trial().digitalData();
  AnalogData const *astim = Globals::trove->trial().analogStimuli();
  DigitalData const *dstim = Globals::trove->trial().digitalStimuli();
  double outrate_hz = Globals::ptree->find(PAR_OUTRATE).toDouble();
  for (QList<Channel>::iterator i=pool.begin(); i!=pool.end(); ++i) {
    QString id = (*i).id;
    int chn = (*i).chn;
    if (actual.contains(id)) {
      LineGraph *g = findp(id);
      TraceInfo *tr;
      if (traces.contains(id)) {
	tr = traces[id];
      } else {
	if ((*i).isDigi)
	  tr = new TraceInfo(TraceInfo::dataBinary,chn);
	else
	  tr = new TraceInfo(TraceInfo::dataDouble);
	traces[id] = tr;
      }
      TraceInfo::DataPtr dp;
      switch ((*i).typ) {
      case Channel::AI:
	if (aacq) {
	  dp.dp_double = aacq->channelData(chn);
	  tr->setData(0,1/Globals::ptree->find("acqEphys/acqFreq").toDouble(),
		      dp,
		      aacq->getNumScans(),
		      aacq->getNumChannels());
	  Connections::AIChannel const &aich =
	    Connections::findAI(id);
	  tr->setScaleFactor(aich.scale);
	  g->setYLabel("("+aich.unit+")");
	}
	break;
      case Channel::AO:
	if (astim) {
	  dp.dp_double = astim->channelData(chn);
	  tr->setData(0,1/outrate_hz,
		      dp,
		      astim->getNumScans(),
		      astim->getNumChannels());
	  g->setYLabel("(V)"); // AO always in volts for now
	}
	break;
      case Channel::DI:
	if (dacq) {
	  dp.dp_binary = dacq->allData();
	  tr->setData(0,1/Globals::ptree->find("acqEphys/acqFreq").toDouble(),
		      dp,
		      dacq->getNumScans());
	}
	break;
      case Channel::DO:
	if (dstim) {
	  dp.dp_binary = dstim->allData();
	  tr->setData(0,1/outrate_hz,
		      dp,
		      dstim->getNumScans());
	}
	break;
      }
      if (tr) {
	g->addTrace(id,tr);
	g->autoSetXRange();
	if ((*i).isDigi) {
	  g->setYRange(Range(-0.1,1.1));
	  g->setYTicks(100,100);
	  g->showYTickLabels(false);
	} else {
	  g->autoSetYRange(0.001,0.5);
	}
	traces[id] = tr;
      }
    }
  }
}
