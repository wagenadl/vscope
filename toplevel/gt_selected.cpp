// gt_selected.cpp

#include "gt_slots.h"
#include <QApplication>
#include <toplevel/globals.h>
#include <gfx/roiimage.h>
#include <QStringList>
#include <toplevel/vscopegui.h>
#include <gui/xmlbutton.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <acq/focus.h>
#include <toplevel/mgauto.h>
#include <toplevel/vsdtraces.h>
#include <toplevel/panelhistory.h>

void gt_slots::selected(QString p) {
  try {
    QStringList l = p.split("/");
    QString tail=l.last();
    dbg("selected %s\n",qPrintable(p));
    if (p.startsWith("panel")) 
      Globals::panelHistory->oldSelection(p);
    else if (p=="maintenance/focus/viewAll" && Globals::focus)
      Globals::focus->setViewMode(Focus::ViewAll);
    else if (p=="maintenance/focus/viewA" && Globals::focus)
      Globals::focus->setViewMode(Focus::ViewA);
    else if (p=="maintenance/focus/viewB" && Globals::focus)
      Globals::focus->setViewMode(Focus::ViewB);
    else if (p=="maintenance/focus/viewZoom" && Globals::focus) 
      Globals::focus->setViewMode(Focus::ViewZoom);
    else if (p=="maintenance/focus/viewDiff" && Globals::focus) 
      Globals::focus->setViewMode(Focus::ViewDiff);
    else if (p=="analysis/zoom") {
      Globals::coumarinw->setModeZoom();
      Globals::oxonolw->setModeZoom();
    } else if (p=="analysis/roiBlob") {
      Globals::coumarinw->setModeBlobROI();
      Globals::oxonolw->setModeBlobROI();
    } else if (p=="analysis/roiSelect") {
      Globals::coumarinw->setModeSelectROI();
      Globals::oxonolw->setModeSelectROI();
    } else if (p=="analysis/roiAdd") {
      Globals::coumarinw->setModeAddROI();
      Globals::oxonolw->setModeAddROI();
    } else if (p=="analysis/roiDelete") {
      Globals::coumarinw->setModeDelROI();
      Globals::oxonolw->setModeDelROI();
    } else if (p=="analysis/roiMove") {
      Globals::coumarinw->setModeMoveROI();
      Globals::oxonolw->setModeMoveROI();
    } else if (p=="analysis/roiResize") {
      Globals::coumarinw->setModeResizeROI();
      Globals::oxonolw->setModeResizeROI();
    } else if (p=="analysis/roiRotate") {
      Globals::coumarinw->setModeRotateROI();
      Globals::oxonolw->setModeRotateROI();
    } else if (p=="analysis/roiRotSize") {
      Globals::coumarinw->setModeRotSizeROI();
      Globals::oxonolw->setModeRotSizeROI();
    }
  } catch (Exception const &e) {
    report(e,"select button");
  }
}  
void gt_slots::deselected(QString p) {
  try {
    dbg("deselected %s\n",qPrintable(p));
  } catch (Exception const &e) {
    report(e,"deselect button");
  }
}
