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
      foreach (ROIImage *img, Globals::ccdw.values())
	img->setModeZoom();
    } else if (p=="analysis/roiBlob") {
      foreach (ROIImage *img, Globals::ccdw.values())
	img->setModeBlobROI();
    } else if (p=="analysis/roiSelect") {
      foreach (ROIImage *img, Globals::ccdw.values())
	img->setModeSelectROI();
    } else if (p=="analysis/roiAdd") {
      foreach (ROIImage *img, Globals::ccdw.values())
	img->setModeAddROI();
    } else if (p=="analysis/roiDelete") {
      foreach (ROIImage *img, Globals::ccdw.values())
	img->setModeDelROI();
    } else if (p=="analysis/roiMove") {
      foreach (ROIImage *img, Globals::ccdw.values())
	img->setModeMoveROI();
    } else if (p=="analysis/roiResize") {
      foreach (ROIImage *img, Globals::ccdw.values())
	img->setModeResizeROI();
    } else if (p=="analysis/roiRotate") {
      foreach (ROIImage *img, Globals::ccdw.values())
	img->setModeRotateROI();
    } else if (p=="analysis/roiRotSize") {
      foreach (ROIImage *img, Globals::ccdw.values())
	img->setModeRotSizeROI();
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
