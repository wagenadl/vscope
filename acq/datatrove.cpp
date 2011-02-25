// DataTrove.cpp

#include "datatrove.h"
#include <acq/trialdata.h>
#include <xml/paramtree.h>
#include <base/roiset.h>
#include <base/roidata3set.h>
#include <math/cohdata.h>
#include <base/dbg.h>

DataTrove::DataTrove(ParamTree *ptree): QObject() {
  ptree_ = ptree;
  ownptree = false;
  constructData();
}

void DataTrove::constructData() {
  trial_ = new TrialData();
  rois_ = new ROISet();
  roidata_ = new ROIData3Set(rois_);
  cohdata_ = new CohData();
  QObject::connect(rois_, SIGNAL(changed(int)), SLOT(saveROIs()));
  QObject::connect(rois_, SIGNAL(changedAll), SLOT(saveROIs()));
}


DataTrove::DataTrove(QDomElement elt): QObject() {
  ptree_ = new ParamTree(elt);
  ownptree = true;
  constructData();
}

DataTrove::~DataTrove() {
  if (ownptree)
    delete ptree_;
  delete trial_;
  delete rois_;
  delete roidata_;
  delete cohdata_;
}

void DataTrove::read(QString dir, QString exptname, QString trialid) {
  bool d = dummy;
  dummy = true; // prevent immediate resaving of rois
  trial_->read(dir, exptname, trialid, ptree_);
  rois_->load(QString("%1/%2/%3-rois.xml")
	      .arg(dir).arg(exptname).arg(trialid));
  Dbg() << "DataTrove::read: "
	<< trial_->exptName() << "/" << trialid;
  dummy = d;
}

void DataTrove::write() {
  if (dummy) {
    Dbg() << "DataTrove::write: not saving: dummy";
    return;
  }

  trial_->write();
  saveROIs();
}

void DataTrove::setDummy(bool d) {
  dummy = d;
}

void DataTrove::saveROIs() {
  if (dummy) {
    Dbg() << "DataTrove::saveROIs: not saving: dummy";
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
