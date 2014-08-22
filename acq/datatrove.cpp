// DataTrove.cpp

#include "datatrove.h"
#include <acq/trialdata.h>
#include <xml/paramtree.h>
#include <base/roiset.h>
#include <base/roidata3set.h>
#include <math/cohdata.h>
#include <base/dbg.h>
#include <xml/connections.h>
#include <QProgressDialog>

DataTrove::DataTrove(ParamTree *ptree): KeyAgg(0) {
  ptree_ = ptree;
  ownptree = false;
  constructData();
  dummy = false;
  asr = true;
}

void DataTrove::constructData() {
  trial_ = new TrialData();
  rois_ = new ROISet();
  roidata_ = new ROIData3Set(rois_);
  cohdata_ = new CohData();

  add(trial_);
  add(rois_);
  add(roidata_);
  add(cohdata_);

  cohdata_->setROISet(rois_);
  cohdata_->setCCDData(roidata_);
  cohdata_->setEPhys(trial_->analogData(), trial_->digitalData());

  updateCameras();

  connect(trial_, SIGNAL(newCameras()), SLOT(updateCameras()));
  connect(rois_, SIGNAL(newDatum(int)), SLOT(saveROIs()));
  connect(rois_, SIGNAL(newAll()), SLOT(saveROIs()));

  trial_->useThisPTree(ptree_);
  trial_->prepare();
}


//DataTrove::DataTrove(QDomElement elt): KeyAgg(0) {
//  ptree_ = new ParamTree(elt);
//  ownptree = true;
//  constructData();
//}

DataTrove::~DataTrove() {
  if (ownptree)
    delete ptree_;
  delete trial_;
  delete rois_;
  delete roidata_;
  delete cohdata_;
}

void DataTrove::updateCameras() {
  foreach (QString camid, trial_->cameras())
    roidata_->setCCDData(camid, trial_->ccdData(camid));

  foreach (QString camid, trial_->cameras())
    connect(trial_->ccdData(camid), SIGNAL(newData()),
	    roidata_, SLOT(updateCCDData()));
}

void DataTrove::read(QString dir, QString exptname, QString trialid,
                     QProgressDialog *pd) {
  KeyGuard guard(*this);
  bool d = dummy;
  bool a = asr;
  dummy = true; // prevent immediate resaving of rois
  asr = false; // prevent immediate resaving of rois
  rois_->clear();
  if (pd)
    pd->setValue(10);
  trial_->read(dir, exptname, trialid, pd);
  rois_->load(QString("%1/%2/%3-rois.xml")
	      .arg(dir).arg(exptname).arg(trialid));
  //Dbg() << "DataTrove::read: "
  //	<< trial_->exptName() << "/" << trialid;
  dummy = d;
  asr = a;
}

void DataTrove::write() {
  if (dummy) {
    //Dbg() << "DataTrove::write: not saving: dummy";
    return;
  }

  trial_->write();
  saveROIs();
}

void DataTrove::setDummy(bool d) {
  dummy = d;
}

void DataTrove::setAutoSaveROIs(bool a) {
  asr = a;
}

void DataTrove::saveROIs() {
  if (!asr) {
    Dbg() << "DataTrove::saveROIs: not saving";
    return;
  }
  
  Dbg() << "DataTrove::saveROIs" << trial_->exptName() << "/" << trial_->trialID();
  try {
    rois_->save(QString("%1/%2/%3-rois.xml")
		.arg(trial_->filePath())
		.arg(trial_->exptName())
		.arg(trial_->trialID()));
  } catch (Exception const &e) {
    e.report();
    warn("DataTrove::saveROIs: not saved - exception");
  }
}
