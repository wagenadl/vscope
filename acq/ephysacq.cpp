// ephysacq.cpp

#include "ephysacq.h"

#include <xml/enumerator.h>
#include <xml/paramtree.h>
#include <xml/connections.h>
#include <base/dbg.h>
#include <base/minmax.h>
#include <base/exception.h>
#include <base/analogdata.h>
#include <daq/analogin.h>
#include <base/digitaldata.h>
#include <daq/digitalin.h>
#include <QTimer>
#include <QMutexLocker>

#define EPHYSACQ_CONTACQ_CHUNKSIZE 8192
#define EPHYSACQ_CONTACQ_CHUNKS_IN_BUFFER 1

static void makeFakeAData(AnalogData *adata) {
  if (!adata)
    return;
  KeyGuard guard(*adata);
  int C = adata->getNumChannels();
  int N = adata->getNumScans();
  for (int c=0; c<C; c++) {
    QString chid = adata->getChannelAtIndex(c);
    double *data = adata->channelData(guard.key(), chid);
    for (int n=0; n<N; n++) {
      *data = (n%(100+60*c)) / 1000.;
      data += C;
    }
  }
}

static void makeFakeDData(DigitalData *ddata) {
  if (!ddata)
    return;
  KeyGuard guard(*ddata);
  int N = ddata->getNumScans();
  DigitalData::DataType mask = ddata->getMask();
  Dbg() << "makeFakeDData mask=" << QString::number(mask,16);
  DigitalData::DataType *data = ddata->allData(guard.key());
  DigitalData::DataType filler = 0;
  DigitalData::DataType step = 0x11111111;
  for (int n=0; n<N; n++) {
    if (n%100==0)
      filler += step;
    *data++ = filler & mask;
  }
}



EPhysAcq::EPhysAcq() {
  ain = 0;
  din = 0;
  adata = 0;
  ddata = 0;
  active = false;
  prep = false;
  timer = 0;
  dataAvFcn = 0;
}

EPhysAcq::~EPhysAcq() {
  if (active) {
    fprintf(stderr,"EPhysAcq destructed while active. Armageddon imminent.\n");
    try {
      abort();
    } catch(...) {
      fprintf(stderr,"Exception caught by EPhysAcq destructor.\n");
    }
  }
  if (ain)
    delete ain;
  if (din)
    delete din;
}

bool EPhysAcq::prepare(ParamTree const *ptree) {
  if (!adata)
    throw Exception("EPhysAcq","No analog buffer defined","prepare");
  if (!ddata)
    throw Exception("EPhysAcq","No digital buffer defined","prepare");

  trialtime_ms = ptree->find("acqEphys/acqTime").toDouble();
  double samprate_hz = ptree->find("acqEphys/acqFreq").toDouble();
  int nscans = roundi(trialtime_ms*samprate_hz/1000);

  bool contEphys = ptree->find("acquisition/contEphys").toBool();

  QStringList aic = ptree->find("acqEphys/aiChannels").toStringList();
  QMap<AnalogIn::Channel, QString> chmap;
  foreach (QString s, aic) 
    chmap[AnalogIn::Channel(Connections::findAI(s).line)] = s;
  int nchans = aic.size();

  if (contEphys)
    nscans =  EPHYSACQ_CONTACQ_CHUNKSIZE * EPHYSACQ_CONTACQ_CHUNKS_IN_BUFFER;
  adata->reshape(nscans, nchans);
  int idx=0;
  foreach (QString id, chmap.values()) {
    Connections::AIChannel const &aic = Connections::findAI(id);
    adata->defineChannel(idx++, id, aic.scale, aic.unit);
  }
  adata->setSamplingFrequency(samprate_hz);

  ddata->reshape(nscans);
  QStringList dch = Connections::digiInputLines();
  Enumerator *digilines = Enumerator::find("DIGILINES");
  ddata->clearMask();
  foreach (QString id, dch)
    ddata->defineLine(digilines->lookup(id), id);
  prep = true;

  bool haveDevice = createDAQ(ptree);
  if (!haveDevice)
    return false;
  
  ain->clearChannels();
  ain->setFrequency(samprate_hz);
  ain->setAcqLength(contEphys ? 0 : nscans);
  ain->setPollPeriod(contEphys ? EPHYSACQ_CONTACQ_CHUNKSIZE : 0);
  
  foreach (QString id, chmap.values()) {
    Connections::AIChannel const &aic = Connections::findAI(id);
    AnalogIn::Channel i(aic.line);
    ain->addChannel(i);
    ain->setRange(i,aic.range);
    if (aic.ground=="RSE")
      ain->setGround(i,AnalogIn::RSE);
    else if (aic.ground=="NRSE")
      ain->setGround(i,AnalogIn::NRSE);
    else if (aic.ground=="DIFF")
      ain->setGround(i,AnalogIn::DIFF);
    else
      throw Exception("EPhysAcq",
		      "Unknown grounding scheme '" + aic.ground
		      + "' for '" + aic.id + "'", "prepare");
  }
  
  din->setAcqLength(contEphys ? 0 : nscans);
  din->setPollPeriod(contEphys ? EPHYSACQ_CONTACQ_CHUNKSIZE : 0);
  din->setChannelMask(0);
  for (QStringList::iterator i=dch.begin(); i!=dch.end(); ++i) {
    // dbg("  digiinput: '%s'",qPrintable(*i));
    din->addChannel(digilines->lookup(*i));
  }
  return true;
}

bool EPhysAcq::createDAQ(ParamTree const *) {
  // perhaps we should use the ptree to figure out what device to use?
  QString devid = "";
  if (!DAQDevice::find(devid).ok())
    return false;

  if (!ain)
 {
    ain = new AnalogIn();
    connect(ain,SIGNAL(acquisitionEnded(AnalogIn*,bool)),
  	    this,SLOT(ainEnded()));
    connect(ain,SIGNAL(dataAvailable(AnalogIn *, int)),
	    this,SLOT(ainAvailable()));
  }

  if (!din) {
    din = new DigitalIn(ain);
    connect(din,SIGNAL(dataAvailable(DigitalIn *, int)),
	    this,SLOT(dinAvailable()));
  }
  return true;
}

void EPhysAcq::start() {
  if (!prep)
    throw Exception("EPhysAcq","Cannot start: not prepared");
  if (active)
    throw Exception("EPhysAcq","Cannot start: already active");
  active=true;
  if (ain || din) {
    //dbg("ephysacq:start");
    if (!ain || !din)
      throw Exception("EPhysAcq","Cannot start: someone ate my ain/din");
    ain->commit();
    ain->start();
    //dbg("ephysacq:started");
  } else {
    //dbg("ephysacq:fakestart");
    if (!timer) {
      timer = new QTimer(this);
      connect(timer,SIGNAL(timeout()), this,SLOT(ainEnded()));
    }

    makeFakeAData(adata);
    makeFakeDData(ddata);
    
    timer->setSingleShot(true);
    timer->setInterval(int(trialtime_ms));
    timer->start();
    //dbg("ephysacq:fakestarted ival=%g ms",trialtime_ms);
  }
}

void EPhysAcq::abort() {
  if (ain) {
    ain->abort();
    ain->uncommit();
  }
  active=false;
}

void EPhysAcq::ainEnded() {
  active=false;
  //dbg("EPhysAcq::ainEnded");
  if (ain) // actually, this is guaranteed, isn't it?
    ain->read(adata);
  if (din) {
    //dbg("ephysacq:ainended din=%p n=%i",din,din->countScansSoFar());
    //int n=
    din->read(ddata);
    //dbg("  nread = %i",n);
  }
  if (ain) {
    ain->stop(); // is this needed?
    ain->uncommit();
    //dbg("EPhysAcq::ain: ain stopped");
  }
  emit ended();
}

void EPhysAcq::setBuffer(AnalogData *adata0, DigitalData *ddata0) {
  adata = adata0;
  ddata = ddata0;
}

void EPhysAcq::setDataAvFcn(void (*fcn)(int analogscans, int digitalscans, void *aux),
			    void *aux) {
  QMutexLocker l(&dataAvLock);
  dataAvFcn = fcn;
  dataAvAux = aux;
}

void EPhysAcq::ainAvailable() {
  int n = ain->read(adata);
  //  dbg("ephysacq:ainAvailable %i",n);
  {
    QMutexLocker l(&dataAvLock);
    if (dataAvFcn)
      dataAvFcn(n,0,dataAvAux);
  }
  if (n<EPHYSACQ_CONTACQ_CHUNKSIZE*EPHYSACQ_CONTACQ_CHUNKS_IN_BUFFER)
    adata->setNumScans(EPHYSACQ_CONTACQ_CHUNKSIZE*EPHYSACQ_CONTACQ_CHUNKS_IN_BUFFER);
}

void EPhysAcq::dinAvailable() {
  int n = din->read(ddata);
  //dbg("ephysacq:dinAvailable %i",n);
  {
    QMutexLocker l(&dataAvLock);
    if (dataAvFcn)
      dataAvFcn(0,n,dataAvAux);
  }
  if (n<EPHYSACQ_CONTACQ_CHUNKSIZE*EPHYSACQ_CONTACQ_CHUNKS_IN_BUFFER)
    ddata->setNumScans(EPHYSACQ_CONTACQ_CHUNKSIZE*EPHYSACQ_CONTACQ_CHUNKS_IN_BUFFER);
}
