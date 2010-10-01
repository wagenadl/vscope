// liveephys.cpp

#include "liveephys.h"
#include <gfx/multigraph.h>
#include <gfx/linegraph.h>
#include <QBitArray>
#include <xml/paramtree.h>
#include <xml/enumerator.h>
#include <xml/connections.h>
#include <daq/analogin.h>
#include <base/analogdata.h>
#include <math.h>
#include <base/minmax.h>
#include <base/dbg.h>
#include <base/exception.h>
#include "vco.h"
#include <gui/xmlpage.h>
#include <gui/xmlbutton.h>
#include <acq/contacq.h>

static bool channelAvailable(QString cid, QString ctyp) {
  if (ctyp=="ai")
    return Enumerator::find("AICHAN")->has(cid);
  else if (ctyp=="ao")
    return Enumerator::find("AOCHAN")->has(cid);
  else if (ctyp=="di" || ctyp=="do")
    return Enumerator::find("DIGILINES")->has(cid);
  else
    return false;
}  

LiveEPhys::LiveEPhys(QWidget *parent, QDomElement conf): QFrame(parent) {
  ic = new MultiGraph(this);
  ec = new MultiGraph(this);
  isActive = false;
  ain = 0;
  data = 0;
  vco = 0;

  contacq = 0;

  for (QDomElement elt=conf.firstChildElement("multigraph");
       !elt.isNull(); elt=elt.nextSiblingElement("multigraph")) {
    QString id = elt.attribute("id");
    QStringList *me = (id=="liveEPhysLeft") 
      ? &potchs_left
      : (id=="liveEPhysRight")
      ?  &potchs_right
      : 0;
    if (me) {
      for (QDomElement c=elt.firstChildElement("graph");
	   !c.isNull(); c=c.nextSiblingElement("graph")) 
	if (channelAvailable(c.attribute("ch"), c.attribute("typ"))) 
	  me->append(c.attribute("ch"));
    }
  }

  vcoidx = -1;
  vcoid = "off"; // this should probably be "" as a default.
}

LiveEPhys::~LiveEPhys() {
  if (isActive) {
    if (ain) {
      try {
	ain->abort();
      } catch (...) {
      }
    }
  }
  if (ain)
    delete ain;
  if (data)
    delete data;
  try {
    for (QMap<int,LineGraph *>::iterator i=aigraphs.begin();
	 i!=aigraphs.end(); ++i)
      i.value()->removeAllTraces();
    for (QMap<int,TraceInfo *>::iterator i=aitraces.begin();
	 i!=aitraces.end(); ++i)
      delete i.value();
  } catch (...) {
    fprintf(stderr,"LiveEPhys::~LiveEPhys: caught exception.\n");
  }
}

bool LiveEPhys::isChannelTiny(QString id) const {
  if (id=="60HzRef")
    return true;
  return false;
}

void LiveEPhys::resizeEvent(class QResizeEvent *) {
  ic->setGeometry(0,0,width()/2,height());
  ec->setGeometry(width()/2,0,width()/2,height());
}

void LiveEPhys::contAcqAvailable(int ascans, int /*dscans*/) {
  if (!data || ascans<=0 || !contacq || !contacq->adata())
    return;

  int nspace = data->getNumScans() - dataoffset;
  int nnow = mini(ascans, nspace);

  AnalogData *src = contacq->adata();
  int nchans = data->getNumChannels();

  memcpy((void*)(data->allData()+nchans*dataoffset),
	 (void*)(src->allData()),
	 nnow*nchans*sizeof(double));
  feedVCO(dataoffset,nnow);
  dataoffset+=nnow;
  addedData();
}

void LiveEPhys::dataAvailable() {
  //  dbg("liveephys(%p):dataav ain=%p data=%p\n",this,ain,data);
  if (!ain || !data)
    return;
  int nscans = data->getNumScans();

  if (dataoffset==0) { // this is our chance to catch up
    while (ain->countScansAvailable()>=2*nscans) {
      ain->read(data);
      t0+=nscans*dt_s;
    }
  }

  int av;
  while ((av = ain->countScansAvailable())>0) {
    int now = ain->read(data,dataoffset,av);
    feedVCO(dataoffset,now);
    dataoffset+=now;
    if (dataoffset>=nscans || now==0)
      break;
  }
  addedData();
}

void LiveEPhys::addedData() {
  int nscans = data->getNumScans();
  if (dataoffset<nscans)
    return;
  dataoffset=0;

  for (QMap<int,TraceInfo*>::iterator i=aitraces.begin();
       i!=aitraces.end(); ++i) {
    int cno = i.key();
    TraceInfo *tr=i.value();
    TraceInfo::DataPtr dp;
    dp.dp_double = data->channelData(cno);
    //dbg("  dataav cno=%i dp=%p",cno,dp.dp_none);
    tr->setData(t0,dt_s, dp, data->getNumScans(),data->getNumChannels());
  }
  if (first) {
    autoRange();
    first = false;
  } else {
    for (QMap<int,LineGraph*>::iterator i=aigraphs.begin();
	 i!=aigraphs.end(); ++i) {
      i.value()->autoSetXRange();
      i.value()->update(); // or update(), but that doesn't seem to work fast?
    }
  }
  t0+=nscans*dt_s;
  //  dbg("  dataav done ain=%p data=%p\n",ain,data);
}

void LiveEPhys::autoRange() {
  for (QMap<int,LineGraph*>::iterator i=aigraphs.begin();
       i!=aigraphs.end(); ++i) {
    i.value()->autoSetXRange();
    i.value()->autoSetYRange(0,2);
  }
}

void LiveEPhys::activate(ParamTree *p) {
  if (isActive)
    throw Exception("LiveEPhys","Attempt to activate while already active");

  ptree = p;
  contacq = 0;

    dbg("liveephys::activate p=%p\n",p);
  try {
    ain = new AnalogIn();
  } catch (Exception e) {
    return;
  }
    dbg("liveephys: ain created");
  connect(ain,SIGNAL(dataAvailable(AnalogIn*,int)),
	  this,SLOT(dataAvailable()));
  
  addChannels(ic, potchs_left);
  addChannels(ec, potchs_right);
  //  dbg("liveephys::channels added\n");
  ain->setTriggering(false);
  ain->setAcqLength(0);
  //  dbg("liveephys: triggering off, acq=cont\n");
  newTimebase();
  //  dbg("liveephys: timebase set\n");
  ain->commit();
  //  dbg("liveephys: committed\n");
  t0=0;
  dataoffset=0;
  ain->start();
  //  dbg("liveephys: started\n");

 
  isActive = true;
  //  dbg("liveephys activated\n");
  show();

  activateVCO();
}

void LiveEPhys::activateVCO() {
  if (vcoid=="off") {
    vcoidx = -1;
  } else {
    Enumerator *chnames = Enumerator::find("AICHAN");
    int cno = chnames->lookup(vcoid);
    vcoidx = data->whereIsChannel(cno);
    if (vcoidx>=0) {
      vcoscale = Connections::findAI(vcoid).scale;
      vco = new VCO();
      vco->setVoltage(0);
      vco->activate(ptree);
    }
  }
}

void LiveEPhys::activate(ParamTree *p, ContAcq *ca) {
  if (isActive)
    throw Exception("LiveEPhys","Attempt to activate while already active");

  ptree = p;
  contacq = ca;

  if (ain) {
    delete ain;
    ain=0;
  }
  
  connect(contacq,SIGNAL(dataSaved(int,int)),
	  this,SLOT(contAcqAvailable(int,int)));

  addChannels(ic, potchs_left);
  addChannels(ec, potchs_right);

  newTimebase();

  t0=0;
  dataoffset=0;

  isActive = true;
  show();

  activateVCO();
}

void LiveEPhys::addChannels(MultiGraph *cc, QStringList const &list) {
  QBitArray chmask = ptree->find("maintenance/liveEphys/aiChannels").toBitArray();
  Enumerator *chnames = Enumerator::find("AICHAN");
  dbg("liveephys addchannels to %p n=%i\n",cc,list.size());
  for (int l=0; l<list.size(); l++) {
    QString cid = list[l];
    if (!chnames->has(cid))
      continue; // skip digital channels
    int cno = chnames->lookup(cid);
    dbg("addchannels l=%i cno=%i cid='%s' test=%i\n",
    	l,cno,qPrintable(cid),chmask.testBit(cno));
    if (chmask.testBit(cno)) {
      LineGraph *lg = new LineGraph(cc); // cc becomes owner
      TraceInfo *tr = new TraceInfo(TraceInfo::dataDouble); // aitraces becomes owner
      dbg("  addchannels cid=%s cno=%i lg=%p tr=%p",qPrintable(cid),cno,lg,tr);
      addChannel(cno, cid);
      cc->addGraph(cid, lg, isChannelTiny(cid));
      lg->addTrace(cid, tr);
      Connections::AIChannel const &ch(Connections::findAI(cid));
      lg->setTraceLabel(cid,ch.alias);
      tr->setScaleFactor(ch.scale);
      lg->setYLabel(ch.unit);
      aitraces[cno] = tr;
      aigraphs[cno] = lg;
    }
  }
  first = true;
}

void LiveEPhys::addChannel(int cno, QString cid) {
  if (!ain)
    return;
  
  Connections::AIChannel const &ch(Connections::findAI(cid));
  ain->addChannel(cno);
  ain->setRange(cno,ch.range);
  if (ch.ground=="NRSE")
    ain->setGround(cno, AnalogIn::NRSE);
  else if (ch.ground=="RSE")
    ain->setGround(cno, AnalogIn::RSE);
  else if (ch.ground=="DIFF")
    ain->setGround(cno, AnalogIn::DIFF);
  else
    throw Exception("LiveEPhys",
		    "Unknown grounding scheme '" + ch.ground +"'",
		    "addChannel");
}

void LiveEPhys::deactivate() {
  //  dbg("LiveEPhys(%p)::deactivate");
  if (!isActive) {
    fprintf(stderr,"LiveEPhys: Deactivated while not active\n");
    return;
  }

  disconnect(contacq,SIGNAL(dataSaved(int,int)),
	     this,SLOT(contAcqAvailable(int,int)));

  if (vco) {
    vco->deactivate();
    delete vco;
    vco = 0;
  }

  isActive = false;
  hide();
  //  dbg("  deactivate: hidden");

  if (ain) {
    ain->stop();
    //    dbg("  deactivate: ain stopped");
    ain->uncommit();
    //    dbg("  deactivate: ain uncommitted");
  }

  for (QMap<int,LineGraph*>::iterator i=aigraphs.begin();
       i!=aigraphs.end(); ++i) {
    i.value()->removeAllTraces();
    //    dbg("  deactivate: remove traces %i",i.key());
  }
  aigraphs.clear();
  for (QMap<int,TraceInfo*>::iterator i=aitraces.begin();
       i!=aitraces.end(); ++i) {
    delete i.value();
    //    dbg("  deactivate: delete trace %i",i.key());
  }
  aitraces.clear();
  //  dbg("  deactivate: traces cleared");

  if (ain) {
    delete ain;
    //dbg("  deactivate: ain deleted");
    ain=0;
    //dbg("  deactivate: ain set to 0");
  }

  ic->deleteAll();
  //  dbg("  deactivate: ic deleteall");
  ec->deleteAll();
  //  dbg("  deactivate: ec deleteall");

  if (data) {
    delete data;
    //    dbg("  deactivate: data deleted");
    data = 0;
    //    dbg("  deactivate: data set to zero");
  }
}

void LiveEPhys::newDIChannels() {
}

void LiveEPhys::newAIChannels() {
  reactivate();
}

void LiveEPhys::reactivate() {
  if (!isActive)
    return;
  
  deactivate();
  if (contacq)
    activate(ptree, contacq);
  else
    activate(ptree);
}

void LiveEPhys::setVCOChannel(QString cid) {
  vcoid = cid;
  reactivate();
}

void LiveEPhys::newTimebase() {
  if (isActive && ain)
    ain->abort();

  double acqfreq_hz = contacq
    ? contacq->getAcqFreq_Hz()
    : ptree->find("acqEphys/acqFreq").toDouble();
  double timebase_s = ptree->find("maintenance/liveEphys/timeBase").toDouble()/1e3;
  int nscans = int(timebase_s*acqfreq_hz);
  int nchans;
  if (ain) {
    nchans = ain->getNumChannels();
  } else if (contacq) {
    AnalogData const *src = contacq->adata();
    if (src)
      nchans = src->getNumChannels();
    else
      throw Exception("LiveEPhys","No contacq src. This should not happen. (1)");
  } else {
    throw Exception("LiveEPhys","No contacq src. This should not happen. (2)");
  }
  
  dataoffset=0;
  dt_s = 1/acqfreq_hz;
  if (ain) {
    int pollscans = 1<<floori(log2(nscans));
    if (pollscans>128)
      pollscans = 128;
    ain->setFrequency(acqfreq_hz);
    ain->setPollPeriod(pollscans);
  }
  dbg("Liveephys::settimebase acqfreqhz=%g timebases=%g nscans=%i dts=%g\n",
      acqfreq_hz, timebase_s, nscans, dt_s);

  TraceInfo::DataPtr dp; dp.dp_none = 0;
  for (QMap<int,TraceInfo *>::iterator i=aitraces.begin();
       i!=aitraces.end(); ++i)
    i.value()->setData(0,dt_s, dp,0);
  
  if (data)
    delete data;
  data = new AnalogData(nscans, nchans);
  if (contacq) {
    AnalogData const *src = contacq->adata();
    if (src) 
      for (int idx=0; idx<nchans; idx++) 
	data->defineChannel(idx, src->getChannelAtIndex(idx));
    else 
      throw Exception("LiveEPhys","No contacq src. This should not happen. (3)");
  } else {
    for (int idx=0; idx<nchans; idx++)
      data->defineChannel(idx,ain->getChannelAt(idx));
  }
  if (isActive && ain) {
    ain->commit();
    ain->start();
  for (QMap<int,LineGraph*>::iterator i=aigraphs.begin();
       i!=aigraphs.end(); ++i) 
    i.value()->autoSetXRange();
  }
}

void LiveEPhys::feedVCO(int offset, int length) {
  if (!vco)
    return;
  if (vcoidx<0)
    return;

  double sumv=0;
  int dn = data->getNumChannels();
  double *d = data->allData() + vcoidx + offset*dn;
  for (int k=0; k<length; k++) {
    sumv += *d;
    d+=dn;
  }

  vco->setVoltage(sumv/length * vcoscale, length/ain->getFrequency());
}


void LiveEPhys::hideConfig(xmlPage &cfgpage) {
  cfgpage.findButton(QStringList("aiChannels")).setEnabled(false);

  // note that, in order to hide properly, we should also close the
  // subpages if they happen to be open. Not today.
}

void LiveEPhys::unhideConfig(xmlPage &cfgpage) {
  cfgpage.findButton(QStringList("aiChannels")).setEnabled(true);
}
