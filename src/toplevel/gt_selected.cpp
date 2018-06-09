// toplevel/gt_selected.cpp - This file is part of VScope.
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
  Dbg() << "Selected " << p;
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
    else if (p=="analysis/roiMoveAll") 
      Globals::ccdw->setMode(ROIImage::CM_MoveAllROIs);
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
void gt_slots::deselected(QString) {
  try {
  } catch (Exception const &e) {
    report(e,"deselect button");
  }
}
