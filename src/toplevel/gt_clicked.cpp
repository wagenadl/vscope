// toplevel/gt_clicked.cpp - This file is part of VScope.
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

// gt_slots.cpp

#include "gt_slots.h"
#include <QApplication>
#include <toplevel/globals.h>
#include <gfx/roiimages.h>
#include <QWidget>
#include <acq/datatrove.h>
#include <base/roiset.h>
#include <acq/liveephys.h>
#include <acq/focus.h>
#include <toplevel/acquire.h>
#include <toplevel/savedsettings.h>
#include <toplevel/exptlog.h>
#include <base/exception.h>
#include <gui/timebutton.h>
#include <gui/guibutton.h>
#include <toplevel/vscopegui.h>
#include <toplevel/scripts.h>
#include <toplevel/mainwindow.h>
#include <base/dbg.h>

static void dimple(QString p) {
  guiButton *b = Globals::gui->findpButton(p);
  if (b) {
  }
}

void gt_slots::doubleClicked(QString p) {
  try {
    dimple(p);
    Dbg() << "Double clicked: " << p;
    if (p=="analysis/roiDeleteAll") {
      Globals::trove->rois().clear();
    } else if (p=="savedSettings/delete") {
      Globals::savedSettings->deleteCurrent();
    }
  } catch (Exception const &e) {
    report(e,"button double click");
  }
}

void gt_slots::clicked(QString p) {
  try {
    dimple(p);
    dbg("clicked %s\n",qPrintable(p));
    if (p=="quit") {
      if (Globals::focus)
	Globals::focus->deactivate(true);
      if (Globals::liveephys)
	Globals::liveephys->deactivate(true);
      QApplication::quit();
    } else if (p=="minimize") {
      Globals::mainwindow->setWindowState(Qt::WindowMinimized);
    } else if (p=="analysis/zoomIn") {
      Globals::ccdw->first()->zoomIn();
      // others will be called through shareZoom
    } else if (p=="analysis/zoomOut") {
      Globals::ccdw->first()->zoomOut();
      // others will be called through shareZoom
    } else if (p=="analysis/roiDelete") {
      int id = Globals::ccdw->first()->currentROI();
      Globals::trove->rois().remove(id);
    } else if (p=="analysis/roiImport") {
      Globals::acquire->prepareImportROIs();
    } else if (p=="maintenance/liveEphys/autoRange") {
      if (Globals::liveephys)
        Globals::liveephys->autoRange();
    } else if (p=="maintenance/focus/autoRange") {
      if (Globals::focus)
        Globals::focus->autoRange();
    } else if (p=="acquisition/singleframe") {
      Globals::acquire->acqFrame();
    } else if (p=="acquisition/singletrial") {
      Globals::acquire->acqTrial();
    } else if (p=="acquisition/unlock") {
      Globals::acquire->unlock();
    } else if (p=="acquisition/load") {
      Globals::acquire->prepareLoad();
    } else if (p=="acquisition/log") {
      Globals::exptlog->prepareUserNote();
    } else if (p=="savedSettings/save") {
      Globals::savedSettings->prepareSave();
    } else if (p=="acquisition/exptelapsed") {
      Globals::exptelapsed->setFormat("Since Click: %1");
      Globals::exptelapsed->startCountUp();
      Globals::exptlog->addNote("Timer reset");
    } else if (p=="acquisition/trialelapsed") {
      Globals::trialelapsed->setFormat("Since Click: %1");
      Globals::trialelapsed->startCountUp();
      Globals::exptlog->addNote("Trial timer reset");
    } else if (p.startsWith("acquisition/camvals")) {
      p = Globals::gui->pathInstantiate(p);
      Dbg() << "path is " << p;
      int cidx = p.lastIndexOf(":");
      if (cidx>=0) {
	QString id = p.mid(cidx+1);
	int sidx = id.indexOf("/");
	if (sidx>=0)
	  id = id.left(sidx);
	QStringList bits = p.split("/");
	QString leaf = bits.takeLast();
	Globals::ccdw->get(id)->recolor(leaf);
      }
    } else if (p=="scripts/load") {
      Globals::scripts->prepareLoad();
    } else if (p=="scripts/save") {
      Globals::scripts->resave();
    } else if (p=="scripts/saveAs") {
      Globals::scripts->prepareSave();
    } else if (p=="scripts/check") {
      Globals::scripts->check();
    }
  } catch (Exception const &e) {
    report(e,"button click");
  }
}
