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
#include <video/videoprog.h>

Trial::Trial(TrialData *d): dat(d) {
  ephysacq = 0;
  ccdacq = 0;
  ephysout = 0;

  ephysacq = new EPhysAcq();
  ephysacq->setBuffer(dat->analogData(),dat->digitalData());
  ephysout = new EPhysOut();
  ephysout->setBuffer(dat->analogStimuli(),dat->digitalStimuli());

  ccdacq = new CCDAcq();

  foreach (QString camid, Connections::allCams()) 
    ccdacq->setDest(camid, dat->ccdData(camid));;

  prep=false;
  active=false;

  connect(ephysacq,SIGNAL(ended()),this,SLOT(ephysacqComplete()));
  connect(ephysout,SIGNAL(ended()),this,SLOT(ephysoutComplete()));
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

QString Trial::prepare(ParamTree const *ptree) {
  if (active)
    throw Exception("Trial","Cannot prepare for new trial while active");

  QString trialid = dat->prepare(ptree);
  
  dbg("trial:prepare contephys=%i. ephysacq=%p",dat->hasContEPhys(),ephysacq);
  if (dat->isCCD())
    ccdacq->prepare(ptree, dat->timing());
  ephysout->setMaster(dat->hasContEPhys() ? 0 : ephysacq);
  ephysout->prepare(ptree, dat->timing());
  if (dat->isEPhys())
    ephysacq->prepare(ptree);
  outcomplete = acqcomplete = false;
  prep = true;
  return trialid;
}

QString Trial::prepareSnapshot(ParamTree const *ptree) {
  Dbg() << "Trial::prepareSnapshot";
  if (active)
    throw Exception("Trial","Cannot prepare for new trial while active");

  Dbg() << "Trial: preparing data";
  QString trialid = dat->prepareSnapshot(ptree);
  Dbg() << "Trial: preparing cameras";
  bool ccdok = ccdacq->prepare(ptree, dat->timing());
  Dbg() << "Trial: camera status: " << ccdok;
  ephysout->setMaster(0);
  ephysout->prepareSnap(ptree, dat->timing());
  outcomplete = acqcomplete = false;
  prep = true;
  return trialid;
}

void Trial::start() {
  Dbg() << "Trial::start";
  if (!prep || !dat->isPrepared())
    throw Exception("Trial","Cannot start: not prepared");
  QDomElement info = dat->getXML()->find("info");
  QDateTime now(QDateTime::currentDateTime());
  info.setAttribute("date",now.toString("yyMMdd"));
  info.setAttribute("time",now.toString("hhmmss"));
  
  outcomplete = acqcomplete = false;
  ephysout->commit();
  if (dat->isCCD()) {
    Dbg() << "Trial: Starting ccd";
    ccdacq->start();
  }
  Dbg() << "Trial: Starting ephysout";
  ephysout->start(); // this won't do anything if there is a master
  if (dat->isEPhys()) {
    Dbg() << "Trial: Starting ephysin";
    ephysacq->start(); // This starts ephysout synchronously through the "master" system.
  }
  active = true;
}

void Trial::abort() {
  ccdacq->abort();
  ephysacq->abort();
  ephysout->abort();
  active = false;
}

void Trial::ephysoutComplete() {
  outcomplete = true;
  if (acqcomplete || !dat->isEPhys())
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
  if (dat->isCCD()) {
    double maxwait_ms = 1000;
    while (!ccdacq->hasEnded()) {
      dbg("Trial complete, ccdacq not yet ended");
      if (maxwait_ms<=0)
	throw Exception("Trial","CCD Acquisition timed out","allComplete");
      Sleeper::msleep(200);
      maxwait_ms -= 200;
    }
    if (!ccdacq->wasSuccessful())
      throw Exception("Trial","CCD Acquisition failed","allComplete");
    dbg("trial complete, ccd ok");
  }
  active = false;
  QDomElement info = dat->getXML()->find("info");
  QDateTime now(QDateTime::currentDateTime());
  info.setAttribute("enddate",now.toString("yyMMdd"));
  info.setAttribute("endtime",now.toString("hhmmss"));
  completedEvent();
  emit ended(dat->exptName(),dat->trialID());
}

void Trial::completedEvent() {
  // meant for descendants
}

