// gt_openclose.cpp

#include "gt_slots.h"
#include <QApplication>
#include <toplevel/vscopegui.h>
#include <gui/guibutton.h>
#include <gui/guipage.h>
#include <toplevel/globals.h>
#include <acq/liveephys.h>
#include <acq/focus.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <toplevel/savedsettings.h>
#include <gui/timebutton.h>
#include <gui/videogui.h>
#include <gui/overridecursor.h>
#include <xml/paramtree.h>
#include <acq/datatrove.h>
#include <gui/guiradiogroup.h>

extern void checkdaq(QWidget *);
extern void checkcam(QWidget *);
extern void checkmea(QWidget *);
extern void checkvid(QWidget *);

void hideTop() {
  Globals::leftplace->hide();
  Globals::rightplace->hide();
  guiPage &root = Globals::gui->rootPage();
  foreach (QString id, root.groupp("showLeft")->childIDs())
    root.buttonp(id)->setEnabled(false);
  foreach (QString id, root.groupp("showRight")->childIDs())
    root.buttonp(id)->setEnabled(false);
}

void showTop() {
  Globals::leftplace->show();
  Globals::rightplace->show();
  guiPage &root = Globals::gui->rootPage();
  foreach (QString id, root.groupp("showLeft")->childIDs())
    root.buttonp(id)->setEnabled(true);
  foreach (QString id, root.groupp("showRight")->childIDs())
    root.buttonp(id)->setEnabled(true);
}

static bool ensureFocusCams() {
  QString idA = Globals::ptree->find("maintenance/focus/camA").toString();
  QString idB = Globals::ptree->find("maintenance/focus/camB").toString();
  Connections::CamCon const *camA = Connections::findpCam(idA);
  Connections::CamCon const *camB = Connections::findpCam(idB);
  Dbg() << "ensureFocusCams" << idA << ":" << idB;
  bool anyChg = false;
  if (!camA) {
    if (camB) {
      idA = camB->partnerid;
      camA = Connections::findpCam(idA);
    }
    if (!camA) {
      foreach (QString id, Connections::allCams()) {
	if (id!=idB) {
	  idA = id;
	  camA = Connections::findpCam(idA);
	  break;
	}
      }
    }
    Globals::ptree->find("maintenance/focus/camA").set(idA);
    if (Connections::findpCam(idA))
      anyChg = true;
  }
  if (!camB) {
    if (camA) {
      idB = camA->partnerid;
      camB = Connections::findpCam(idB);
    }
    if (!camB) {
      foreach (QString id, Connections::allCams()) {
	if (id!=idA) {
	  idB = id;
	  camB = Connections::findpCam(idB);
	  break;
	}
      }
    }
    Globals::ptree->find("maintenance/focus/camB").set(idB);
    if (Connections::findpCam(idB))
      anyChg = true;
  }
  return anyChg;
}

void gt_slots::pgopen(QString p, QWidget *w) {
  try {
    dbg("pgopen %s\n",qPrintable(p));
    if (p=="maintenance/checkDaq")
      checkdaq(w);
    else if (p=="maintenance/checkCam")
      checkcam(w);
    else if (p=="maintenance/checkMEA")
      checkmea(w);
    else if (p=="maintenance/checkVideo")
      checkvid(w);
    else if (p=="maintenance/focus") {
      hideTop();
      if (Globals::focus) {
	if (ensureFocusCams())
	  Globals::gui->findpPage(p)->open();

	Globals::focus->setCams(Globals::ptree->find(p+"/camA").toString(),
				Globals::ptree->find(p+"/camB").toString());
        Globals::focus->activate(true);
      }
    } else if (p=="maintenance/liveEphys") {
      hideTop();
      if (Globals::liveephys) {
	if (Globals::ptree->find("acquisition/contEphys").toBool()) {
	  // We are already acquiring, so live ephys must be slave.
	  // First, hide irrelevant buttons
	  Globals::liveephys->hideConfig(Globals::gui->findPage(p));
	  Globals::liveephys->activate(Globals::ptree,Globals::contacq);
	} else {
	  Globals::liveephys->activate(Globals::ptree);
	}
      }
    } else if (p=="analysis") {
      // Globals::gui->findButton("leftCam1").setSelected(true);
      // Globals::gui->findButton("rightCam2").setSelected(true);
    } else if (p=="savedSettings") {
      Globals::savedSettings->show();
    } else if (p=="acquisition") {
      Globals::walltime->show();
      Globals::exptelapsed->show();
      Globals::trialelapsed->show();
    } else if (p=="stimVideo") {
      OverrideCursor oc(Qt::WaitCursor);
      Globals::videogui->openPage(Globals::gui, Globals::ptree);
    }
  } catch (Exception const &e) {
    report(e,"open page");
  }
}

void gt_slots::pgclose(QString p, QWidget *) {
  try {
    dbg("pgclose %s\n",qPrintable(p));
    if (p=="maintenance/liveEphys") {
      if (Globals::liveephys) {
        Globals::liveephys->deactivate();
	Globals::liveephys->unhideConfig(Globals::gui->findPage(p));
      }	
      showTop();
    } else if (p=="maintenance/focus") {
      if (Globals::focus)
        Globals::focus->deactivate();
      showTop();
    } else if (p=="savedSettings") {
      Globals::savedSettings->hide();
    } else if (p=="acquisition") {
      Globals::walltime->hide();
      Globals::exptelapsed->hide();
      Globals::trialelapsed->hide();
    }
  } catch (Exception const &e) {
    report(e,"close page");
  }
}
