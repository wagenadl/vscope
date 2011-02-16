// gt_slots.cpp

#include "gt_slots.h"
#include <QApplication>
#include <toplevel/globals.h>
#include <gfx/roiimage.h>
#include <QWidget>
#include <acq/liveephys.h>
#include <acq/focus.h>
#include <toplevel/acquire.h>
#include <toplevel/savedsettings.h>
#include <toplevel/exptlog.h>
#include <base/exception.h>
#include <gui/timebutton.h>
#include <gui/xmlbutton.h>
#include <toplevel/vscopegui.h>
#include <toplevel/scripts.h>

static void dimple(QString p) {
  xmlButton *b = Globals::gui->findpButton(p);
  if (b) {
  }
}

void gt_slots::clicked(QString p) {
  try {
    dimple(p);
    printf("clicked %s\n",qPrintable(p));
    if (p=="quit") {
      QApplication::quit();
    } else if (p=="minimize") {
      Globals::mainwindow->setWindowState(Qt::WindowMinimized);
    } else if (p=="analysis/zoomIn") {
      Globals::ccdw.values().first()->zoomIn();
      // others will be called through shareZoom
    } else if (p=="analysis/zoomOut") {
      Globals::ccdw.values().first()->zoomOut();
      // others will be called through shareZoom
    } else if (p=="analysis/roiDelete") {
      Globals::ccdw.values().first()->deleteROI();
      // others will receive acceptROIdelete.
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
    } else if (p=="acquisition/load") {
      Globals::acquire->prepareLoad();
    } else if (p=="acquisition/log") {
      Globals::exptlog->prepareUserNote();
    } else if (p=="savedSettings/save") {
      Globals::savedSettings->prepareSave();
    } else if (p=="acquisition/_exptelapsed") {
      Globals::exptelapsed->setFormat("Since Click:\n%1");
      Globals::exptelapsed->startCountUp();
      Globals::exptlog->addNote("Timer reset");
    } else if (p.startsWith("acquisition/_camvals")) {
      QStringList bits = p.split("/");
      QString leaf = bits.takeLast();
      QString id = bits.last().mid(8);
      Globals::ccdw[id]->recolor(leaf);
      Globals::acquire->redisplayCCD();
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
