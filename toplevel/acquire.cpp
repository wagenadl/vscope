// acquire.cpp

#include "acquire.h"
#include <toplevel/globals.h>
#include <toplevel/vscopegui.h>
#include <toplevel/mgauto.h>
#include <xml/paramtree.h>
#include <acq/trial.h>
#include <acq/contacq.h>
#include <base/ccddata.h>
#include <base/analogdata.h>
#include <base/digitaldata.h>
#include <gfx/roiimages.h>
#include <base/dbg.h>
#include <toplevel/vsdtraces.h>
#include <toplevel/coherence.h>
#include <gfx/blackout.h>
#include <gfx/filedlgkey.h>
#include <gui/timebutton.h>
#include <gui/guibutton.h>
#include <gui/guiexc.h>
#include <toplevel/exptlog.h>
#include <base/roiset.h>
#include <xml/connections.h>
#include <video/videoprog.h>
#include <acq/datatrove.h>
#include <QDir>
#include <toplevel/mainwindow.h>
#include <base/roidata3set.h>
#include <toplevel/panelhistory.h>

Acquire::Acquire() {
  connect(Globals::trial,SIGNAL(ended(QString,QString)),
	  this,SLOT(trialDone()));
  loaddlg = 0;
  loadframe = 0;
  lastdir = "";
  blockout = false;
  connect(&autoRunner,SIGNAL(timeout()),this,SLOT(autoRunEvent()));
}

Acquire::~Acquire() {
}

void Acquire::incTrialNo() {
  Param &ptrial = Globals::ptree->find("acquisition/trialno");
  QString fpath = Globals::filePath();
  QString expt = Globals::ptree->find("acquisition/exptname").toString();
  int trialno = ptrial.toInt() + 1;
  while (QFile(QString("%1/%2/%3.xml").
	       arg(fpath).arg(expt).arg(trialno,int(3),int(10),QChar('0')))
	 .exists())
    trialno++;
  ptrial.setInt(trialno);
}

void Acquire::acqFrame() {
  bool dummy = Globals::ptree->find("acquisition/dummy").toBool();
  blockout = true;
  Globals::blackout->show();
  if (dummy) {
    Globals::blackout->setText("Starting *DUMMY* Frame Acquisition...");
  } else {
    Globals::blackout->setText("Starting Frame Acquisition...");
    Globals::trialelapsed->startCountUp(); // hmmm...
    Globals::exptelapsed->startCountUp(true);
    incTrialNo();
  }
  
  type = FRAME;
  Globals::trial->prepareSnapshot(Globals::ptree);
  if (!dummy) {
    Globals::exptlog->markSnap();
    Globals::contacq->markTrial(Globals::exptlog->trialID());
  }
  Globals::trial->start();
  if (dummy)
    Globals::blackout->setText("Acquiring *DUMMY* Single Frame...");
  else
    Globals::blackout->setText("Acquiring Single Frame...");
}

void Acquire::acqTrial() {
  bool dummy = Globals::ptree->find("acquisition/dummy").toBool();
  blockout = true;
  Globals::blackout->show();
  if (dummy) {
    Globals::blackout->setText("Starting *DUMMY* Trial Acquisition...");
  } else {
    Globals::blackout->setText("Starting Trial Acquisition...");
    Globals::trialelapsed->startCountUp();
    Globals::exptelapsed->startCountUp(true);
    incTrialNo();
  }
  
  type = TRIAL;
  Globals::trial->prepare(Globals::ptree);
  if (!dummy) {
    Globals::exptlog->markTrial();
    Globals::contacq->markTrial(ExptLog::trialID());
  }
  Globals::trial->start();
  if (dummy)
    Globals::blackout->setText("Acquiring *DUMMY* Trial...");
  else
    Globals::blackout->setText("Acquiring Trial...");
}


void Acquire::abort() {
  Globals::blackout->setText("Aborting Acquisition...");
  dbg("acquire::abort");
  // At this point, data is invalid. What do we do?
  TrialType t = type;
  type = NONE;
  switch (t) {
  case FRAME:
    abortFrame();
    break;
  case TRIAL:
    abortTrial();
    break;
  case NONE:
    break;
  }
  Globals::blackout->hide();
  blockout = false;
}

void Acquire::trialDone() {
  Globals::blackout->setText("Acquisition Complete");
  switch (type) {
  case FRAME:
    doneFrame();
    break;
  case TRIAL:
    doneTrial();
    break;
  case NONE:
    break;
  }
  Globals::blackout->hide();
  Globals::gui->open(); // make sure updated trial number visible.
  blockout = false;
}

void Acquire::abortFrame() {
  dbg("Acquire:frame abort. Not yet implemented.");
}

void Acquire::abortTrial() {
  dbg("Acquire:trial abort. Not yet implemented");
}

void Acquire::closeDialog() {
  dbg("Acquire:closeDialog: Not yet implemented");
}

void Acquire::saveData() {
  try {
    Globals::trove->write();
  } catch (Exception const &e) {
    GUIExc::report(e,"Acquire");
  }
}

void Acquire::redisplayCCD() {
  displayCCD();
}

void Acquire::displayCCD(bool writePixStatsToLog) {
  QVector<QString> camname;
  foreach (QString id, Connections::allCams())
    camname.push_back(id);
  int K = camname.size();
  
  QVector<ROIImage *> imgs;
  foreach (QString id, camname)
    imgs.push_back(Globals::ccdw->get(id));

  QVector<guiButton *> btn;
  foreach (QString id, camname)
    btn.push_back(&Globals::gui->findButton(QString("acquisition/camvals")
					    + ARRAYSEP + id));

  QVector<CCDData const *> src;
  foreach (QString id, camname)
    src.push_back(Globals::trove->trial().ccdData(id));

  QVector<Connections::CamCon const *> cams;
  foreach (QString id, camname)
    cams.push_back(Connections::findpCam(id));

  QString brightMsg = "Pixel values:";
  bool first = true;
  for (int k=0; k<K; k++) {
    if (!src[k] || src[k]->getNFrames()==0)
      continue;
    uint16_t const *dat = src[k]->frameData();
    int nser = src[k]->getSerPix();
    int npar = src[k]->getParPix();
    imgs[k]->adjustedRange(dat,nser,npar);
    imgs[k]->newImage(dat,nser,npar,
		      Globals::trove->trial().ccdPlacement(camname[k]));
    if (first) {
      Globals::coherence->adjustedRange(dat,nser,npar);
      Globals::coherence->newImage(dat,nser,npar,
				   Globals::trove->trial()
				   .ccdPlacement(camname[k]));
    first = false;
    }
    uint16_t min = 65535;
    uint16_t max = 0;
    uint64_t sum = 0;
    uint16_t const *ptr = dat;
    for (int p=0; p<npar; p++) {
      for (int s=0; s<nser; s++) {
  	uint16_t v = *ptr++;
  	if (v<min)
  	  min=v;
  	if (v>max)
  	  max=v;
  	sum+=v;
      }
    }
    double avg = double(sum)/nser/npar;
    QString s = QString("%1/%2/%3").arg(min).arg(int(avg)).arg(max);
    if (btn[k]->getFormat().indexOf(":")<0)
      btn[k]->setFormat(btn[k]->text() + ": %1");
    Dbg() << "btn["<<k<<"]: format := " << btn[k]->getFormat();
    btn[k]->setValue(s);
    Dbg() << "btn["<<k<<"]: format ::= " << btn[k]->getFormat();
    if (k)
      brightMsg += ";";
    brightMsg += " " + camname[k] + ": " + s;
  }
  if (writePixStatsToLog)
    Globals::exptlog->addNote(brightMsg);
}

void Acquire::displayEPhys() {
  Globals::mgstim->rebuild();
  Globals::mgintra->rebuild();
  Globals::mgextra->rebuild();
}

void Acquire::updateVSDTraces() {
  //  Globals::trove->roidata().updateData();
  //  Globals::vsdtraces->newEPhys();
}


void Acquire::doneFrame() {
  dbg("frame complete");
  saveData();
  displayCCD();
}

void Acquire::doneTrial() {
  dbg("trial complete.");
  saveData();
  displayCCD();
  displayEPhys();
  //  Globals::trove->roidata().updateData(); // temporary hack
  //  updateVSDTraces();
}

void Acquire::loadData(QString xmlfn) {
  QStringList path = xmlfn.split("/");
  QString trialbit = path.takeLast();
  QString exptbit = path.takeLast();
  QString dirbit = path.join("/");
  trialbit.replace(".xml","");
  trialbit.replace("-analog.dat","");
  trialbit.replace("-digital.dat","");
  trialbit.replace("-ccd.dat","");
  
  try {
    Globals::trove->read(dirbit,exptbit,trialbit);
  } catch (Exception const &e) {
    GUIExc::report(e,"load data");
  }
  Globals::trial->reconstructStim(Globals::ptree);

  Globals::ptree->find("acquisition/exptname").set(exptbit);
  Globals::ptree->find("acquisition/trialno").set(trialbit);
  Globals::ptree->find("acquisition/dummy").set("true");
  
  Globals::trove->roidata().
    setDebleach((DEBLEACH)Globals::ptree->find("analysis/debleach").toInt());
  displayCCD();
  displayEPhys();
  //  updateVSDTraces();
  Globals::panelHistory->relabelAll();
  Globals::gui->open(); // make sure updated trial number visible.
  
  if (loadframe)
      loadframe->hide();

}

void Acquire::prepareLoad() {
  if (!loaddlg) {
    dbg("prepareload: creating dialog");
    loadframe = new QFrame(Globals::mainwindow);
    loadframe->setGeometry(50,30,924,700);
    loadframe->setFrameStyle(QFrame::Raised|QFrame::Panel);
    loadframe->setLineWidth(2);
    loadframe->setAutoFillBackground(true);
    QPalette p = loadframe->palette();
    p.setColor(QPalette::Window,"#ddeeee");
    loadframe->setPalette(p);
    
    loaddlg = new FileDlgKey(loadframe, true);
    QRect r(loadframe->contentsRect());
    r.adjust(2,2,-2,-2);
    loaddlg->setGeometry(r);
    loaddlg->setLabel("Load Previously Acquired Data...");
    loaddlg->setExtn("xml");
    connect(loaddlg,SIGNAL(chosenFile(QString)),
	    this,SLOT(loadData(QString)));
    connect(loaddlg,SIGNAL(changedDir(QString)),
	    this,SLOT(chgDir(QString)));
    connect(loaddlg,SIGNAL(canceled()),
	    loadframe,SLOT(hide()));
  }

  dbg("lastdir = [%s]",qPrintable(lastdir));

  if (lastdir.isEmpty())
    lastdir = Globals::filePath();
  loaddlg->goDir(lastdir);
  loaddlg->show();
  loadframe->show();
}

void Acquire::chgDir(QString dir) {
  lastdir = dir;
}

void Acquire::setContEphys() {
  bool contephys = Globals::ptree->find("acquisition/contEphys").toBool();
  if (contephys) {
    if (Globals::contacq->isActive()) {
      Dbg() << "Acquire: continuous acquisition already active";
      return;
    }
    incTrialNo();
    Globals::contacq->prepare(Globals::ptree);
    Globals::exptlog->markContEphys();
    Globals::gui->open(); // make sure updated trial number visible.
    Globals::contacq->start();
  } else {
    if (!Globals::contacq->isActive()) {
      Dbg() << "Acquire: continuous acquisition already stopped";
      return;
    }
    Globals::contacq->stop();
    Globals::exptlog->markContEphysEnds();
  }
}

void Acquire::setAutoRun() {
  bool autorun = Globals::ptree->find("acquisition/autoRun").toBool();
  newTrialPeriod();
  if (autorun)
    acqTrial();
}

void Acquire::newTrialPeriod() {
  bool autorun = Globals::ptree->find("acquisition/autoRun").toBool();
  if (autorun) {
    double ival_ms = Globals::ptree->find("acquisition/trialPeriod")
      .toDouble();
    autoRunner.setInterval(int(ival_ms + 0.5)); // round to nearest ms
    autoRunner.setSingleShot(false);
    autoRunner.start();
  } else {
    autoRunner.stop();
  }
}

void Acquire::autoRunEvent() {
  if (blockout) {
    fprintf(stderr,"Warning: Auto run event ignored because a trial is ongoing.\n");
    Globals::exptlog->addNote("Warning: Auto run event ignored because a trial is ongoing.");
  } else {
    acqTrial();
  }
}

int Acquire::maxTrial() {
  QDir dir(Globals::filePath() + "/"
	   + Globals::ptree->find("acquisition/exptname").toString());
  if (!dir.exists())
    return 0;
  QStringList flt; flt << "*.xml";
  dir.setNameFilters(flt);
  dir.setSorting(QDir::Name);
  QStringList fls = dir.entryList();
  if (fls.isEmpty())
    return 0;
  QString tri = fls.last();
  tri = tri.left(3);
  bool ok;
  int n = tri.toInt(&ok);
  if (ok)
    return n;
  else
    return 0;
}
