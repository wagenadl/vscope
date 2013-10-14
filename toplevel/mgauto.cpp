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

MGAuto::Channel::Channel(QDomElement c) {
  id = c.attribute("ch");
  QString ctyp = c.attribute("typ");
  QString ctiny = c.attribute("tiny");
  label = id;
  if (ctyp=="ai") {
    typ=AI;
    label = Aliases::lookup(id);
    isOut = false;
    isDigi = false;
  } else if (ctyp=="ao") {
    typ=AO;
    isOut = true;
    isDigi = false;
  } else if (ctyp=="di") {
    typ=DI;
    //chn = Enumerator::find("DIGILINES")->lookup(id);
    label = Aliases::lookup(id);
    isOut = false;
    isDigi = true;
  } else if (ctyp=="do") {
    typ=DO;
    //chn = Enumerator::find("DIGILINES")->lookup(id);
    label = Aliases::lookup(id);
    isOut = true;
    isDigi = true;
  } else
    throw Exception("MGAuto::Channel","Unknown type '"+ctyp+"'",
		    "constructor");

  tiny = ctiny=="yes" || ctiny=="true";

  if_enabled = xmlAttribute(c, "if_enabled", "");
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
	  pool.push_back(Channel(c));
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
    if (!Globals::ptree->enabled((*i).if_enabled))
      continue;
    
    switch ((*i).typ) {
    case Channel::AI:
      if (Globals::trove->trial().analogData() &&
	  Globals::trove->trial().analogData()->contains(id)) 
	newset.insert(id);
      break;
    case Channel::AO:
      if (Globals::trove->trial().analogStimuli() &&
	  Globals::trove->trial().analogStimuli()->contains(id)) 
	newset.insert(id);
      break;
    case Channel::DI:
      if (Globals::trove->trial().digitalData() &&
	  Globals::trove->trial().digitalData()->hasLine(id)) 
	newset.insert(id);
    case Channel::DO:
      if (Globals::trove->trial().digitalStimuli() &&
	  Globals::trove->trial().digitalStimuli()->hasLine(id)) 
	newset.insert(id);
      break;
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
  Dbg() << "MGAuto: outrate: " << outrate_hz;

  foreach (Channel c, pool) {
    if (actual.contains(c.id)) {
      LineGraph *g = findp(c.id);
      TraceInfo *tr = traces[c.id];
      if (!tr)	  
	traces[c.id] = tr = new TraceInfo();
      switch (c.typ) {
      case Channel::AI:
	if (aacq) {
	  tr->setData(0,1/Globals::ptree->find("acqEphys/acqFreq").toDouble(),
		      DataPtr(aacq->channelData(c.id)),
		      aacq->getNumScans(),
		      aacq->getNumChannels());
	  tr->setScaleFactor(aacq->getScaleAtIndex(aacq->whereIsChannel(c.id)));
	  g->setYLabel("("
		       +aacq->getUnitAtIndex(aacq->whereIsChannel(c.id))
		       +")");
	}
	break;
      case Channel::AO:
	if (astim) {
	  Dbg() << "MGAuto: AO:"<< c.id<<": " << astim->contains(c.id) << "="<<astim->whereIsChannel(c.id)<<" scale="<<astim->getScaleAtIndex(astim->whereIsChannel(c.id));
	  tr->setData(0,1/outrate_hz,
		      DataPtr(astim->channelData(c.id)),
		      astim->getNumScans(),
		      astim->getNumChannels());
	  tr->setScaleFactor(astim->getScaleAtIndex
			     (astim->whereIsChannel(c.id)));
	  g->setYLabel("("
		       +astim->getUnitAtIndex(astim->whereIsChannel(c.id))
		       +")");
	}
	break;
      case Channel::DI:
	if (dacq) {
	  tr->setData(0,1/Globals::ptree->find("acqEphys/acqFreq").toDouble(),
		      DataPtr(dacq->allData(),dacq->findLine(c.id)),
		      dacq->getNumScans());
	}
	break;
      case Channel::DO:
	if (dstim) {
	  tr->setData(0,1/outrate_hz,
		      DataPtr(dstim->allData(),dstim->findLine(c.id)),
		      dstim->getNumScans());
	}
	break;
      }
      
      g->addTrace(c.id, tr);
      g->autoSetXRange();
      if (c.isDigi) {
	g->setYRange(Range(-0.1,1.1));
	g->setYTicks(100,100);
	g->showYTickLabels(false);
      } else {
	g->autoSetYRange(0.001,0.5);
      }
    }
  }
}
