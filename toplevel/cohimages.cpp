// cohimages.cpp

#include "cohimages.h"
#include <gfx/roiimages.h>
#include <xml/connections.h>
#include "globals.h"
#include <acq/datatrove.h>
#include <acq/trialdata.h>
#include <xml/paramtree.h>
#include "mainwindow.h"
#include <xml/enumerator.h>
#include <gui/guimenu.h>
#include "vscopegui.h"

CohImages::CohImages(CohData *data):
  CohMaps(QRect(0, 0, 512, 512)), data(data) {
}

CohImages::~CohImages() {
}

void CohImages::setup() {
  setCameras(Connections::allCams());
  connect(&Globals::trove->trial(), SIGNAL(newCameras()),
	  this, SLOT(updateCameras()));
  //  setROIs(&Globals::trove->rois());
}

void CohImages::updateCameras() {
  QStringList ids = Globals::trove->trial().cameras();
  setCameras(ids);
}

void CohImages::setCameras(QStringList newids) {
  QStringList oldids = ids();

  QSet<QString> droppool;
  foreach (QString id, oldids)
    droppool.insert(id);

  foreach (QString id, newids) {
    if (droppool.contains(id)) {
      droppool.remove(id);
    } else {
      Coherence *img = new Coherence(data, Globals::leftplace);
      img->setCanvas(Globals::ccdw->currentCanvas());
      img->setGeometry(0,0,512,Globals::mainwindow->basey());
      img->setRefTrace(Globals::ptree->find("analysis/refTrace").toString());
      img->setShowMode((SHOWROIS)Globals::ptree->find("analysis/showROIs").
                       toInt());
      img->setCamPair(Connections::camPair(id)); // Really, this should be
      // ... derived from TrialData too, but we don't have it there yet.
      add(id, img);
      img->setGeometry(0,0,512,Globals::mainwindow->basey());
      img->hide();
    }
  }

  foreach (QString id, droppool)
    del(id);

  Enumerator *curcams = Enumerator::find("CURRENTCAMERAS");
  Enumerator *showwhat = Enumerator::find("SHOWWHAT");
  foreach (QString id, curcams->getAllTags())
    showwhat->remove("Coh-"+id);
  foreach (QString id, newids) 
    showwhat->add("Coh-"+id);
}