// gt_selected.cpp

#include "gt_slots.h"
#include <QApplication>
#include <toplevel/globals.h>
#include <gfx/roiimages.h>
#include <QStringList>
#include <toplevel/vscopegui.h>
#include <gui/guibutton.h>
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
    if (p.startsWith("panel")) 
      Globals::panelHistory->oldSelection(p);
    else if (p=="analysis/zoom") 
      Globals::ccdw->setMode(ROIImage::CM_Zoom);
    else if (p=="analysis/roiBlob") 
      Globals::ccdw->setMode(ROIImage::CM_BlobROI);
    else if (p=="analysis/roiSelect") 
      Globals::ccdw->setMode(ROIImage::CM_SelectROI);
    else if (p=="analysis/roiAdd") 
      Globals::ccdw->setMode(ROIImage::CM_AddROI);
    else if (p=="analysis/roiDelete") 
      Globals::ccdw->setMode(ROIImage::CM_DelROI);
    else if (p=="analysis/roiMove") 
      Globals::ccdw->setMode(ROIImage::CM_MoveROI);
    else if (p=="analysis/roiResize") 
      Globals::ccdw->setMode(ROIImage::CM_ResizeROI);
    else if (p=="analysis/roiRotate") 
      Globals::ccdw->setMode(ROIImage::CM_RotateROI);
    else if (p=="analysis/roiRotSize") 
      Globals::ccdw->setMode(ROIImage::CM_RotSizeROI);
  } catch (Exception const &e) {
    report(e,"select button");
  }
}  
void gt_slots::deselected(QString p) {
  try {
  } catch (Exception const &e) {
    report(e,"deselect button");
  }
}
