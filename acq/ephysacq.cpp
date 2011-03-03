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

  int nchans = 0;
  QBitArray ba = ptree->find("acqEphys/aiChannels").toBitArray();
  for (int i=0; i<ba.size(); i++)
    if (ba.testBit(i))
      nchans++;
  
  adata->reshape(contEphys ? EPHYSACQ_CONTACQ_CHUNKSIZE*EPHYSACQ_CONTACQ_CHUNKS_IN_BUFFER : nscans,nchans);
  int idx=0;
  for (int i=0; i<ba.size(); ++i)
    if (ba.testBit(i))
      adata->defineChannel(idx++,i);
  adata->setSamplingFrequency(samprate_hz);

  ddata->reshape(contEphys
		 ? EPHYSACQ_CONTACQ_CHUNKSIZE*EPHYSACQ_CONTACQ_CHUNKS_IN_BUFFER
		 : nscans);
  prep = true;

  bool haveDevice = createDAQ(ptree);
  if (!haveDevice)
    return false;
  
  ain->clearChannels();
  ain->setFrequency(samprate_hz);
  ain->setAcqLength(contEphys ? 0 : nscans);
  ain->setPollPeriod(contEphys ? EPHYSACQ_CONTACQ_CHUNKSIZE : 0);
  
  Enumerator *aichan = Enumerator::find("AICHAN");
  for (int i=0; i<ba.size(); ++i) {
    if (ba.testBit(i)) {
      Connections::AIChannel const &aic =
	Connections::findAI(aichan->reverseLookup(i));
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
  }
  
  din->setAcqLength(contEphys ? 0 : nscans);
  din->setPollPeriod(contEphys ? EPHYSACQ_CONTACQ_CHUNKSIZE : 0);
  QStringList dch = Connections::digiInputLines();
  // dbg("ephysacq: digiinputs:");
  Enumerator *digilines = Enumerator::find("DIGILINES");
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
