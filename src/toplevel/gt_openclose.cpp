// toplevel/gt_openclose.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    if (p=="maintenance/checkDaq")
      checkdaq(w);
    else if (p=="maintenance/checkCam")
      checkcam(w);
    else if (p=="maintenance/checkMEA")
      checkmea(w);
    else if (p=="maintenance")
      Globals::gui->findButton("maintenance/focus")
        .setEnabled(!Connections::allCams().isEmpty());
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
      Globals::gui->findButton("acquisition/singleframe")
        .setEnabled(!Connections::allCams().isEmpty());
      Globals::gui->findButton("acquisition/singletrial")
        .setEnabled(!Globals::ptree->find("acqEphys/aiChannels")
                    .toStrings().isEmpty());
      Globals::walltime->show();
      Globals::exptelapsed->show();
      Globals::trialelapsed->show();
    }
  } catch (Exception const &e) {
    report(e,"open page");
  }
}

void gt_slots::pgclose(QString p, QWidget *) {
  try {
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
