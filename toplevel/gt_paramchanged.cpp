#include "gt_slots.h"
#include <QApplication>
#include <toplevel/globals.h>
#include <acq/liveephys.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <acq/trial.h>
#include <toplevel/mgauto.h>
#include <toplevel/vsdtraces.h>
#include <toplevel/acquire.h>
#include <xml/paramtree.h>
#include <base/roidata.h>
#include <gui/timebutton.h>
#include <gui/guibutton.h>
#include <gui/overridecursor.h>
#include <toplevel/exptlog.h>
#include <toplevel/vscopegui.h>
#include <toplevel/panelhistory.h>
#include <video/videolight.h>
#include <gui/videogui.h>
#include <gfx/roiimages.h>
#include <gfx/cohmaps.h>
#include <gfx/cohgraph.h>
#include <xml/enumerator.h>
#include <acq/datatrove.h>
#include <toplevel/scripts.h>
#include <gui/guipage.h>
#include <toplevel/savedsettings.h>
#include <base/roidata3set.h>
#include <base/enums.h>
#include <acq/focus.h>

static QString immediateCCDMaster(QString id) {
  /* Returns the master of camera ID, or "" if self. */
  QString pname = "acqCCD/camera:" + id + "/master";
  QString pval = Globals::ptree->find(pname).toString();
  Enumerator *cams = Enumerator::find("CAMERAS");
  if (cams->lookup(pval)>=0)
    return pval;
  else
    return "";
}

static QString ultimateCCDMaster(QString id) {
  /* Returns the (master of the master of the) master of camera ID,
     or "" if self, or "LOOP" if loop detected. */
  QString m = immediateCCDMaster(id);
  if (m=="")
    return "";
  QSet<QString> seen;
  seen.insert(id);
  while (true) {
    seen.insert(m);
    QString mm = immediateCCDMaster(m);
    if (mm=="")
      return m;
    if (seen.contains(mm))
      return "LOOP";
    m = mm;
  }
}  

static void ensureMasterOK(QString p) {
  QRegExp re("camera:([^/]*)/");
  int pos = re.indexIn(p);
  if (pos<0)
    throw Exception("gt_paramchanged", "ensureMasterOK confused by " + p);
  QString myid = re.cap(1);
  Dbg() << "Checking on master for " << myid;
  QString ultimate = ultimateCCDMaster(myid);
  if (ultimate=="LOOP") {
    QString master = immediateCCDMaster(myid);
    Globals::ptree->find("acqCCD/camera:" + master + "/master")
      .setInt(CAM_self);
  }
}
      
      
      
    

static void setRefTrace() {
  int typ = Globals::ptree->find("analysis/refType").toInt();
  Enumerator *e = Enumerator::find("REFTYPE");
  if (typ==e->lookup("Analog")) {
    QString chn = Globals::ptree->find("analysis/refTrace").toString();
    Globals::vsdtraces->setRefTrace(chn);
    Globals::cohmaps->setRefTrace(chn);
    Globals::cohgraph->setRefTrace(chn);
  } else if (typ==e->lookup("Digital")) {
    QString chn = Globals::ptree->find("analysis/refDigi").toString();
    Globals::vsdtraces->setRefDigi(chn);
    Globals::cohmaps->setRefDigi(chn);
    Globals::cohgraph->setRefDigi(chn);
  } else if (typ==e->lookup("Frequency")) {
    double frqhz = Globals::ptree->find("analysis/refFreq").toDouble();
    Globals::vsdtraces->setRefFreq(frqhz);
    Globals::cohmaps->setRefFreq(frqhz);
    Globals::cohgraph->setRefFreq(frqhz);
  } else {
    dbg("gt_paramchanged: Unknown reference type");
  }
}

void gt_slots::everythingChanged() {
  Globals::videogui->reset(Globals::gui, Globals::ptree);
  Globals::trial->prepare(Globals::ptree);
  Globals::mgintra->rebuild();
  Globals::mgextra->rebuild();
  Globals::mgstim->rebuild();
  setRefTrace();

  SHOWROIS sm = (SHOWROIS)Globals::ptree->find("analysis/showROIs").toInt();
  Globals::ccdw->showROIs(sm);
  Globals::cohmaps->setShowMode(sm);
  Globals::trove->roidata().
    setDebleach((DEBLEACH)Globals::ptree->find("analysis/debleach").toInt());
  Globals::acquire->newTrialPeriod();
  Globals::acquire->setContEphys();
  Globals::acquire->setAutoRun();
}

void gt_slots::paramchanged(QString p, QString val) {
  try {
    // Most parameters' side effects go in the following switch.

    if (p!="savedSettings/name") {
      Globals::savedSettings->anythingChanged();
    }
    
    if (p=="maintenance/liveEphys/timeBase") {
      if (Globals::liveephys)
        Globals::liveephys->newTimebase();
    } else if (p=="maintenance/liveEphys/aiChannels") {
      if (Globals::liveephys)
        Globals::liveephys->newAIChannels();
    } else if (p=="maintenance/liveEphys/diChannels") {
      if (Globals::liveephys)
        Globals::liveephys->newDIChannels();
    } else if (p=="maintenance/liveEphys/vcoSource") {
      if (Globals::liveephys)
        Globals::liveephys->setVCOChannel(val);
    } else if (p=="acqEphys/aiChannels") {
      Globals::trial->prepare(Globals::ptree);
      Globals::mgstim->rebuild();
      Globals::mgintra->rebuild();
      Globals::mgextra->rebuild();
    } else if (p=="analysis/refType" || p=="analysis/refTrace" ||
	       p=="analysis/refDigi" || p=="analysis/refFreq") {
      setRefTrace();
    } else if (p=="analysis/showROIs") {
      SHOWROIS sm = (SHOWROIS)Globals::ptree->find(p).toInt();
      Globals::ccdw->showROIs(sm);
      Globals::cohmaps->setShowMode(sm);
    } else if (p=="analysis/debleach") {
      Globals::trove->roidata().
	setDebleach((DEBLEACH)Globals::ptree->find(p).toInt());
    } else if (p=="acquisition/trialPeriod") {
      Globals::acquire->newTrialPeriod();
    } else if (p=="panelLeft") {
      Globals::panelHistory->newSelection("Left");
    } else if (p=="panelRight") {
      Globals::panelHistory->newSelection("Right");
    } else if (p=="stimVideo/enable") {
      OverrideCursor oc(Qt::WaitCursor);
      Globals::videogui->changeEnable(Globals::gui,Globals::ptree);
    } else if (p=="stimVideo/prog") {
      OverrideCursor oc(Qt::WaitCursor);
      Globals::videogui->changeProgram(Globals::gui,Globals::ptree);
    } else if (p.startsWith("stimVideo/par")) {
      OverrideCursor oc(Qt::WaitCursor);
      Globals::videogui->changeParam(Globals::gui,Globals::ptree,p.mid(10));
    } else if (QRegExp("acqCCD/camera:.*/master").exactMatch(p)) {
      ensureMasterOK(p);
    }

    if (p.startsWith("stim") || p.startsWith("acqCCD")) {
      Globals::trial->prepare(Globals::ptree);
      Globals::mgstim->rebuild();
      Globals::mgintra->rebuild();
      Globals::mgextra->rebuild();
    }
    
    // Parameters that should not be reported as a parameter change
    // get worked on here.
    if (p=="acquisition/exptname") {
      Globals::exptelapsed->setFormat("Since Start:\n%1");
      Globals::exptelapsed->startCountUp();
      Globals::exptlog->newExptName();
      if (!dbgfile)
	dbgfile = new DbgFile();
      dbgfile->setDir(Globals::filePath() + "/" + val);
      int lastmax = Acquire::maxTrial();
      Dbg() << "trial for " << val << " is " << lastmax;
      Globals::ptree->find("acquisition/trialno").setInt(lastmax);
      guiPage *pg = Globals::gui->findpPage("acquisition");
      if (pg && pg->isVisible())
	pg->open();
    } else if (p=="acquisition/contEphys") {
      Globals::acquire->setContEphys();
    } else if (p=="acquisition/dummy") {
      bool dummy = Globals::ptree->find(p).toBool();
      Globals::trove->setDummy(dummy);
      guiButton *b = Globals::gui->findpButton(p);
      b->setForeground(dummy ? QColor("#ff0000") : QColor("#000000"));
      b->setText(dummy ? "DUMMY" : "Dummy");
    } else if (p=="acquisition/autoRun") {
      if (val=="true")
        Globals::exptlog->addNote("Auto run: Enabled");
      else
        Globals::exptlog->addNote("Auto run: Disabled");
      Globals::acquire->setAutoRun();
    } else if (p=="stimVideo/lightOn") {
      VideoLight::set(Globals::ptree->find(p).toBool());
    } else if (p=="scripts/run") {
      Globals::scripts->setRunning(Globals::ptree->find(p).toBool());
    } else if (p.startsWith("panel")) {
      ;
    } else if (p.startsWith("maintenance/focus/cam")) {
      QString oldA = Globals::focus->getCamA();
      QString oldB = Globals::focus->getCamB();
      QString newA = Globals::ptree->find("maintenance/focus/camA").toString();
      QString newB = Globals::ptree->find("maintenance/focus/camB").toString();
      if (newA==newB) {
	if (p.endsWith("A"))
	  newB = oldA;
	else
	  newA = oldB;
	Dbg() << "cams was" << oldA<<oldB << " new " << newA<<newB;
        Globals::ptree->find("maintenance/focus/camA").set(newA);
        Globals::ptree->find("maintenance/focus/camB").set(newB);
	guiPage *pg = Globals::gui->findpPage("maintenance/focus");
	if (pg && pg->isVisible())
	  pg->open();
      }
      Globals::focus->setCams(newA, newB);
    } else if (p=="maintenance/focus/camB") {
      dbg("focus/camB");
      QString oldA = Globals::focus->getCamA();
      QString oldB = Globals::focus->getCamB();
      QString newB = Globals::ptree->find(p).toString();
      QString newA = oldA;
      if (newB==oldA) {
        newA = oldB;
        Globals::ptree->find("maintenance/focus/camA").set(newA);
        // may have to set the menu option as well
        // this doesn't work when camera is not present
      }
      Globals::focus->setCams(newA, newB);
    } else if (p=="maintenance/focus/mode") {
      Globals::focus->setViewMode(FOCUSMODE(Globals::ptree->find(p).toInt()));
    } else {
      // Create log entry
      QRegExp re("/([^/:]+):([^/]+)/");
      QString arr="";
      QString chn="";
      if (re.indexIn(p)>=0) {
	arr=re.cap(1);
	chn=re.cap(2);
      }
      QString val = Globals::ptree->find(p).toString();
      QString gp = Globals::gui->pathDeinstantiate(p);
      QStringList pathparts = gp.split("/");
      gp = "";
      QString lbl = "";
      while (!pathparts.isEmpty()) {
	if (!gp.isEmpty())
	  gp += "/";
	if (!lbl.isEmpty())
	  lbl += " ";
	QString pp = pathparts.takeFirst();
	gp += pp;
	if (pp==arr) {
	  lbl += chn;
	} else {
	  guiButton *b = Globals::gui->findpButton(gp);
	  if (b) {
	    QString t = b->text();
	    int idx = t.indexOf(':');
	    if (idx>=0)
	      t=t.left(idx);
	    t.replace("\n"," ");
	    lbl += t;
	  }
	}
      }
      if (lbl.contains(" Enabled") || lbl.contains(" Disabled")) {
	lbl.replace("Enabled","");
	lbl.replace("Disabled","");
	val = (val=="true") ? "Enabled" : "Disabled";
      }
      lbl.replace("...","");
      lbl=lbl.simplified();
      lbl.replace(" :",":");
      Globals::exptlog->changeSetting(lbl, val);
    }
  } catch (Exception const &e) {
    report(e,"parameter change");
  }
}

