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
#include <gui/xmlbutton.h>
#include <gui/overridecursor.h>
#include <toplevel/exptlog.h>
#include <toplevel/vscopegui.h>
#include <toplevel/panelhistory.h>
#include <video/videolight.h>
#include <gui/videogui.h>
#include <gfx/roiimage.h>
#include <toplevel/coherence.h>
#include <toplevel/cohgraph.h>
#include <xml/enumerator.h>
#include <acq/datatrove.h>
#include <toplevel/scripts.h>

static void setRefTrace() {
  int typ = Globals::ptree->find("analysis/refType").toInt();
  Enumerator *e = Enumerator::find("REFTYPE");
  if (typ==e->lookup("Analog")) {
    int chn = Globals::ptree->find("analysis/refTrace").toInt();
    Globals::vsdtraces->setRefTrace(chn);
    Globals::coherence->setRefTrace(chn);
    Globals::cohgraph->setRefTrace(chn);
  } else if (typ==e->lookup("Digital")) {
    int chn = Globals::ptree->find("analysis/refDigi").toInt();
    Globals::vsdtraces->setRefDigi(chn);
    Globals::coherence->setRefDigi(chn);
    Globals::cohgraph->setRefDigi(chn);
  } else if (typ==e->lookup("Frequency")) {
    double frqhz = Globals::ptree->find("analysis/refFreq").toDouble();
    Globals::vsdtraces->setRefFreq(frqhz);
    Globals::coherence->setRefFreq(frqhz);
    Globals::cohgraph->setRefFreq(frqhz);
  } else {
    dbg("gt_paramchanged: Unknown reference type");
  }
}

void gt_slots::everythingChanged() {
  Globals::videogui->changeEnable(Globals::gui, Globals::ptree);
  Globals::trial->prepare(Globals::ptree);
  Globals::mgintra->rebuild();
  Globals::mgextra->rebuild();
  Globals::mgstim->rebuild();
  setRefTrace();

  ROIImage::ShowMode sm =
    ROIImage::ShowMode(Globals::ptree->find("analysis/showROIs").toInt());
  Globals::coumarinw->showROIs(sm);
  Globals::oxonolw->showROIs(sm);
  Globals::coherence->setShowMode(sm);
  Globals::vsdtraces->
    setDebleach(ROIData::Debleach(Globals::ptree->find("analysis/debleach").toInt()));
  Globals::acquire->newTrialPeriod();
  Globals::acquire->setContEphys();
  Globals::acquire->setAutoRun();
}

void gt_slots::paramchanged(QString p, QString val) {
  try {
    dbg("param changed %s -> %s\n",qPrintable(p),qPrintable(val));
    // Most parameters' side effects go in the following switch.

    if (p!="savedSettings/_name") 
      Globals::ptree->find("savedSettings/_name").set("Modified");
    
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
      ROIImage::ShowMode sm =
	ROIImage::ShowMode(Globals::ptree->find(p).toInt());
      Globals::coumarinw->showROIs(sm);
      Globals::oxonolw->showROIs(sm);
      Globals::coherence->setShowMode(sm);
    } else if (p=="analysis/debleach") {
      Globals::vsdtraces->
	setDebleach(ROIData::Debleach(Globals::ptree->find(p).toInt()));
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
    }

    if (p.startsWith("stim") || p.startsWith("acqCCD")) {
      Globals::trial->prepare(Globals::ptree);
      Globals::mgstim->rebuild();
      Globals::mgintra->rebuild();
      Globals::mgextra->rebuild();
    }
    
    // Parameters that should not be reported as a parameter change
    // get worked on here.
    if (p=="acquisition/_exptname") {
      Globals::exptelapsed->setFormat("Since Start:\n%1");
      Globals::exptelapsed->startCountUp();
      Globals::exptlog->newExptName();
    } else if (p=="acquisition/contEphys") {
      Globals::acquire->setContEphys();
    } else if (p=="acquisition/_dummy") {
      bool dummy = Globals::ptree->find(p).toBool();
      xmlButton *b = Globals::gui->findpButton(p);
      b->setForeground(dummy ? QColor("#ff0000") : QColor("#000000"));
      b->setText(dummy ? "DUMMY" : "Dummy");
    } else if (p=="acquisition/autoRun") {
      if (val=="true")
        Globals::exptlog->addNote("Auto run: Enabled");
      else
        Globals::exptlog->addNote("Auto run: Disabled");
      Globals::acquire->setAutoRun();
    } else if (p=="stimVideo/_@lightOn") {
      VideoLight::set(Globals::ptree->find(p).toBool());
    } else if (p=="scripts/_run") {
      Globals::scripts->setRunning(Globals::ptree->find(p).toBool());
    } else if (p.startsWith("panel")) {
      ;
    } else {
      // Create log entry
      QRegExp re(":([^/]+)/");
      QString chn="";
      if (re.indexIn(p)>=0) 
	chn=re.cap(1);
      dbg("chn = [%s]",qPrintable(chn));
      QString val = Globals::ptree->find(p).toString();
      QString gp = Globals::gui->pathDeinstantiate(p);
      dbg("path = %s ----> %s",qPrintable(p), qPrintable(gp));
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
	xmlButton *b = Globals::gui->findpButton(gp);
	if (b) {
	  QString t = b->text();
	  int idx = t.indexOf(':');
	  if (idx>=0)
	    t=t.left(idx);
	  t.replace("\n"," ");
	  lbl += t;
	} else {
	  // lbl += pp;
	}
      }
      if (chn!="")
	lbl += " (" + chn + ")";
      if (lbl.contains(" Enabled") || lbl.contains(" Disabled")) {
	lbl.replace(" Enabled","");
	lbl.replace(" Disabled","");
	lbl.replace("Enabled","");
	lbl.replace("Disabled","");
	val = (val=="true") ? "Enabled" : "Disabled";
      }
      lbl.replace("...","");
      lbl.replace(QRegExp(" +")," ");
      lbl.replace(" :",":");
      Globals::exptlog->changeSetting(lbl + ": " + val);
      dbg("wrote '%s: %s' to log",qPrintable(lbl), qPrintable(val));
    }
  } catch (Exception const &e) {
    report(e,"parameter change");
  }
}

