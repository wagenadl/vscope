// liveephys.cpp

#include "liveephys.h"
#include <gfx/multigraph.h>
#include <gfx/linegraph.h>
#include <QBitArray>
#include <xml/paramtree.h>
#include <xml/enumerator.h>
#include <xml/connections.h>
#include <xml/aliases.h>
#include <daq/analogin.h>
#include <base/analogdata.h>
#include <math.h>
#include <base/minmax.h>
#include <base/dbg.h>
#include <base/exception.h>
#include "vco.h"
#include <gui/guipage.h>
#include <gui/guibutton.h>
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
  ptree = 0;
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
    foreach (LineGraph *lg, aigraphs.values())
      lg->removeAllTraces();
    foreach (TraceInfo *tr, aitraces.values())
      delete tr;
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

  KeyGuard guard(*data);

  int nspace = data->getNumScans() - dataoffset;
  int nnow = mini(ascans, nspace);

  AnalogData *src = contacq->adata();
  int nchans = data->getNumChannels();

  memcpy((void *)(data->allData(guard.key())+nchans*dataoffset),
	 (void const *)(src->allData()),
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

  foreach (QString cid, aitraces.keys()) {
    TraceInfo *tr = aitraces[cid];
    DataPtr dp(data->channelData(cid));
    //dbg("  dataav cno=%i dp=%p",cno,dp.dp_none);
    tr->setData(t0, dt_s, dp, data->getNumScans(), data->getNumChannels());
  }
  if (first) {
    autoRange();
    first = false;
  } else {
    foreach (LineGraph *lg, aigraphs.values()) {
      lg->autoSetXRange();
      lg->update();
    }
  }
  t0+=nscans*dt_s;
  //  dbg("  dataav done ain=%p data=%p\n",ain,data);
}

void LiveEPhys::autoRange() {
  foreach (LineGraph *lg, aigraphs.values()) {
    lg->autoSetXRange();
    lg->autoSetYRange(0,2);
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
  connect(ain, SIGNAL(dataAvailable(AnalogIn*, int)),
	  this, SLOT(dataAvailable()));

  channels.clear();
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
    vcoidx = data->whereIsChannel(vcoid);
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

  channels.clear();
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
  QStringList chmask = ptree->find("maintenance/liveEphys/aiChannels")
    .toStringList();
  QSet<QString> chset;
  foreach (QString c, chmask)
    chset.insert(c);
  dbg("liveephys addchannels to %p n=%i\n",cc,list.size());
  foreach (QString cid, list) {
    Connections::AIChannel const *aic = Connections::findpAI(cid);
    if (!aic)
      continue; // skip, e.g., digital channels
    if (chset.isEmpty())
      chset.insert(cid); // make sure we have at least one
    if (!chset.contains(cid))
      continue;
    LineGraph *lg = new LineGraph(cc); // cc becomes owner
    TraceInfo *tr = new TraceInfo(); // aitraces becomes owner
    addChannel(cid);
    cc->addGraph(cid, lg, isChannelTiny(cid));
    lg->addTrace(cid, tr);
    lg->setTraceLabel(cid,Aliases::lookup(cid));
    tr->setScaleFactor(aic->scale);
    lg->setYLabel(aic->unit);
    aitraces[cid] = tr;
    aigraphs[cid] = lg;
  }
  first = true;
}

void LiveEPhys::addChannel(QString cid) {
  Connections::AIChannel const &ch(Connections::findAI(cid));
  channels.append(&ch);
  
  if (!ain)
    return;

  AnalogIn::Channel cno(ch.line);
  ain->addChannel(cno);
  ain->setRange(cno, ch.range);
  ain->setGround(cno,
		 ch.ground=="NRSE" ? AnalogIn::NRSE
		 :ch.ground=="RSE" ? AnalogIn::RSE
		 :ch.ground=="DIFF" ? AnalogIn::DIFF
		 :AnalogIn::NRSE);
}

void LiveEPhys::deactivate(bool quietIfAlready) {
  //  dbg("LiveEPhys(%p)::deactivate");
  if (!isActive) {
    if (!quietIfAlready)
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

  foreach (LineGraph *lg, aigraphs) 
    lg->removeAllTraces();
  aigraphs.clear();
  foreach (TraceInfo *tr, aitraces) 
    delete tr;
  aitraces.clear();

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

  foreach (TraceInfo *tr, aitraces)
    tr->setData(0,dt_s, DataPtr(), 0);
  
  if (data)
    delete data;
  data = new AnalogData(nscans, nchans, acqfreq_hz);
  if (contacq) {
    AnalogData const *src = contacq->adata();
    if (src) 
      for (int idx=0; idx<nchans; idx++) 
	data->defineChannel(idx,
			    src->getChannelAtIndex(idx),
			    src->getScaleAtIndex(idx),
			    src->getUnitAtIndex(idx));
    else 
      throw Exception("LiveEPhys","No contacq src. This should not happen. (3)");
  } else {
    int idx = 0;
    foreach (Connections::AIChannel const *ach, channels) {
      data->defineChannel(idx++, ach->id, ach->scale, ach->unit);
    }
  }
  if (isActive && ain) {
    ain->commit();
    ain->start();
  }
  foreach (LineGraph *lg, aigraphs) 
    lg->autoSetXRange();
}

void LiveEPhys::feedVCO(int offset, int length) {
  if (!vco)
    return;
  if (vcoidx<0)
    return;

  double sumv=0;
  int dn = data->getNumChannels();
  double const *d = data->allData() + vcoidx + offset*dn;
  for (int k=0; k<length; k++) {
    sumv += *d;
    d+=dn;
  }

  vco->setVoltage(sumv/length * vcoscale, length/ain->getFrequency());
}


void LiveEPhys::hideConfig(guiPage &cfgpage) {
  cfgpage.findButton(QStringList("aiChannels")).setEnabled(false);

  // note that, in order to hide properly, we should also close the
  // subpages if they happen to be open. Not today.
}

void LiveEPhys::unhideConfig(guiPage &cfgpage) {
  cfgpage.findButton(QStringList("aiChannels")).setEnabled(true);
}
