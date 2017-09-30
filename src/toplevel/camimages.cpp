// toplevel/camimages.cpp - This file is part of VScope.
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

// camimages.cpp

#include "camimages.h"
#include <xml/connections.h>
#include "globals.h"
#include <acq/datatrove.h>
#include <acq/trialdata.h>
#include <xml/paramtree.h>
#include "mainwindow.h"
#include <xml/enumerator.h>
#include <gui/guimenu.h>
#include "vscopegui.h"
#include "updatecamenum.h"

CamImages::CamImages(): ROIImages(QRect(0, 0, 512, 512)) {
}

CamImages::~CamImages() {
}

void CamImages::setup() {
  setCameras(Connections::allCams());
  connect(&Globals::trove->trial(), SIGNAL(newCameras()),
	  this, SLOT(updateCameras()));
  // I am not sure if the following needs to be done in updateCameras as well.
  setROIs(&Globals::trove->rois());
  SHOWROIS sm = (SHOWROIS)Globals::ptree->find("analysis/showROIs").toInt();
  showROIs(sm);
}

void CamImages::updateCameras() {
  QStringList ids = Globals::trove->trial().cameras();
  setCameras(ids);
}

void CamImages::setCameras(QStringList newids) {
  QStringList oldids = ids();

  QSet<QString> droppool;
  foreach (QString id, oldids)
    droppool.insert(id);

  foreach (QString id, newids) {
    if (droppool.contains(id)) {
      droppool.remove(id);
    } else {
      ROIImage *img = new ROIImage(Globals::leftplace);
      CamPair pair = Globals::trove->trial().camPair(id);
      img->setCamPair(pair);
      add(id, img);
      img->setGeometry(0,0,512,Globals::mainwindow->basey());
      img->hide();
    }
  }

  foreach (QString id, droppool)
    del(id);

  ::updateCamEnum(Enumerator::find("SHOWWHAT"), "CCD-", newids);
  ::updateCamEnum(Enumerator::find("CAMERAS"), "", newids);

  QStringList pp = QString("Left Right").split(" ");
  foreach (QString p, pp) {
    guiMenu *menu = dynamic_cast<guiMenu*>(Globals::gui->findpPage("panel"+p));
    if (!menu)
      throw Exception("CamImages", "No panel menu " + p);
    menu->updateAuto();
  }
  guiPage *page = Globals::gui->findpPage("acquisition");
  if (!page)
    throw Exception("CamImages", "No acquisition page");
  page->updateAuto();
  
  page = Globals::gui->findpPage("acqCCD");
  if (!page)
    throw Exception("CamImages", "No acqCCD page");
  page->updateAuto();    
}
