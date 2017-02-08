// trial.cpp

#include "trial.h"

#include <base/analogdata.h>
#include <base/digitaldata.h>
#include <base/ccddata.h>

#include <acq/ephysacq.h>
#include <acq/ccdacq.h>
#include <acq/ephysout.h>

#include <xml/enumerator.h>
#include <xml/connections.h>
#include <xml/paramtree.h>
#include <base/xml.h>
#include <base/base26.h>

#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QThread>
#include <QSet>
#include <xml/enumerator.h>
#include <base/dbg.h>

Trial::Trial(TrialData *d): dat(d) {
  ephysacq = 0;
  ccdacq = 0;
  ephysout = 0;

  ephysacq = new EPhysAcq();
  ephysacq->setBuffer(dat->analogData(),dat->digitalData());
  ephysout = new EPhysOut();
  ephysout->setBuffer(dat->analogStimuli(),dat->digitalStimuli());

  ccdacq = new CCDAcq();

  connect(dat, SIGNAL(newCameras()), SLOT(updateCameras()));
  updateCameras();

  prep=false;
  active=false;
  outcomplete = acqcomplete = ccdcomplete = false;

  connect(ephysacq, SIGNAL(ended()), this, SLOT(ephysacqComplete()));
  connect(ephysout, SIGNAL(ended()), this, SLOT(ephysoutComplete()));
}

Trial::~Trial() {
  if (active) {
    fprintf(stderr,"Trial destructed while active. Armageddon imminent.\n");
    try {
      abort();
    } catch(...) {
      fprintf(stderr,
	      "Trial destructor caught exception. Armageddon imminent.\n");
    }
  }

  if (ccdacq)
    delete ccdacq;
  if (ephysacq)
    delete ephysacq;
  if (ephysout)
    delete ephysout;

  if (dat)
    delete dat; // eventually, this will not be here any more
}

void Trial::updateCameras() {
  QSet<QString> cams;
  foreach (QString id, dat->cameras())
    cams.insert(id);
  foreach (QString camid, Connections::allCams()) 
    ccdacq->setDest(camid, cams.contains(camid) ? dat->ccdData(camid) : 0);
}  

void Trial::reconstructStim(ParamTree const *ptree) {
  ephysout->prepare(ptree, dat->allTiming());
}

void Trial::prepare(ParamTree const *ptree) {
  if (active)
    throw Exception("Trial","Cannot prepare for new trial while active");

  dat->useThisPTree(ptree);
  
  if (dat->isCCD()) {
    bool ccdok = ccdacq->prepare(ptree, dat->allTiming());
    if (!ccdok) {
#if CCDACQ_ACQUIRE_EVEN_WITHOUT_CAMERA
      dbg("trial:prepare: no camera. will simulate");
#else
      dbg("trial:prepare: trouble configuring cameras.");
#endif
    }
  }
  ephysout->setMaster(dat->hasContEPhys() ? 0 : ephysacq);
  ephysout->prepare(ptree, dat->allTiming());
  if (dat->isEPhys())  {
    bool ephysok = ephysacq->prepare(ptree);
    if (!ephysok) {
#if EPHYSACQ_TOLERATE_MISSING_EPHYS
      dbg("trial:prepare: No ephys device, will simulate");
#else
      throw Exception("Trial", "Trouble configuring ephys");
#endif
    }
  }

  outcomplete = acqcomplete = ccdcomplete = false;
  prep = true;
}

void Trial::prepareSnapshot(ParamTree const *ptree) {
  if (active)
    throw Exception("Trial","Cannot prepare for new trial while active");

  dat->useThisPTree(ptree);
  dat->prepareSnapshot();
  //bool ccdok =
  ccdacq->prepare(ptree, dat->allTiming());
  ephysout->setMaster(0);
  ephysout->prepareSnap(ptree, dat->allTiming());
  outcomplete = acqcomplete = ccdcomplete = false;
  prep = true;
}

void Trial::start() {
  if (!prep || !dat->isPrepared())
    throw Exception("Trial","Cannot start: not prepared");
  QDomElement info = dat->getXML()->find("info");
  QDateTime now(QDateTime::currentDateTime());
  info.setAttribute("date",now.toString("yyMMdd"));
  info.setAttribute("time",now.toString("hhmmss"));
  
  outcomplete = acqcomplete = ccdcomplete = false;
  ephysout->commit();
  if (dat->isCCD()) {
    ccdacq->start();
  }
  ephysout->start(); // this won't do anything if there is a master
  if (dat->isEPhys()) {
    ephysacq->start(); // This starts ephysout synchronously through the "master" system.
  } else {
    acqcomplete = true;
  }
  active = true;
}

void Trial::abort() {
  ccdacq->abort();
  ephysacq->abort();
  ephysout->abort();
  active = false;
  outcomplete = acqcomplete = ccdcomplete = false;
}

void Trial::ephysoutComplete() {
  outcomplete = true;
  if (acqcomplete)
    allEPhysComplete();
}

void Trial::ephysacqComplete() {
  acqcomplete = true;
  if (outcomplete)
    allEPhysComplete();
}

class Sleeper: public QThread {
public:
  static void msleep(unsigned long msecs) {
    QThread::msleep(msecs);
  }
};

void Trial::allEPhysComplete() {
  bool ccdc = true;
  if (dat->isCCD()) {
    double maxwait_ms = 5000;
    while (!ccdacq->hasEnded()) {
      dbg("Trial complete, ccdacq not yet ended");
      if (maxwait_ms<=0) {
        Warning() << "CCD Acquisition timed out";
        ccdc = false;
        break;
      }
      Sleeper::msleep(200);
      maxwait_ms -= 200;
    }
    if (ccdc && !ccdacq->wasSuccessful()) {
      Warning() << "CCD Acquisition failed";
      ccdc = false;
    }
  }
  ccdcomplete = ccdc;
  active = false;
  Dbg() << "Trial done" << dat->isCCD() << ccdcomplete;
  if (dat->isCCD()) {
    if (ccdcomplete) {
      ccdacq->finish();
      dat->refineCCDTiming();
    } else {
      ccdacq->abort();
    }
  }
  Dbg() << "Emitting ended";

  QDomElement info = dat->getXML()->find("info");
  QDateTime now(QDateTime::currentDateTime());
  info.setAttribute("enddate",now.toString("yyMMdd"));
  info.setAttribute("endtime",now.toString("hhmmss"));
  emit ended(dat->exptName(), dat->trialID());
}

  
