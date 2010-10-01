// gt_openclose.cpp

#include "gt_slots.h"
#include <QApplication>
#include <toplevel/vscopegui.h>
#include <gui/xmlbutton.h>
#include <gui/xmlpage.h>
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

extern void checkdaq(QWidget *);
extern void checkcam(QWidget *);
extern void checkmea(QWidget *);
extern void checkvid(QWidget *);

void hideTop() {
  Globals::leftplace->hide();
  Globals::rightplace->hide();
  xmlPage &root = Globals::gui->rootPage();
  QList<xmlButton*> left=root.getGroup("showLeft");
  for (QList<xmlButton*>::iterator i=left.begin(); i!=left.end(); ++i)
    (*i)->setEnabled(false);
  QList<xmlButton*> right=root.getGroup("showRight");
  for (QList<xmlButton*>::iterator i=right.begin(); i!=right.end(); ++i)
    (*i)->setEnabled(false);
}

void showTop() {
  Globals::leftplace->show();
  Globals::rightplace->show();
  xmlPage &root = Globals::gui->rootPage();
  QList<xmlButton*> left=root.getGroup("showLeft");
  for (QList<xmlButton*>::iterator i=left.begin(); i!=left.end(); ++i)
    (*i)->setEnabled(true);
  QList<xmlButton*> right=root.getGroup("showRight");
  for (QList<xmlButton*>::iterator i=right.begin(); i!=right.end(); ++i)
    (*i)->setEnabled(true);
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
      if (Globals::focus)
        Globals::focus->activate();
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
