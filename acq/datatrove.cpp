// DataTrove.cpp

#include "datatrove.h"
#include <acq/trialdata.h>
#include <xml/paramtree.h>
#include <base/roiset.h>
#include <base/roiset3data.h>
#include <math/cohdata.h>
#include <base/dbg.h>

DataTrove::DataTrove(ParamTree *ptree): QObject() {
  ptree_ = ptree;
  ownptree = false;
  trial_ = new TrialData();
  rois_ = new ROISet();
  roidata_ = new ROISet3Data();
  cohdata_ = new CohData();
}


DataTrove::DataTrove(QDomElement elt): QObject() {
  ptree_ = new ParamTree(elt);
  ownptree = true;
  trial_ = new TrialData();
  rois_ = new ROISet();
  roidata_ = new ROISet3Data();
  cohdata_ = new CohData();
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
  trial_->read(dir, exptname, trialid, ptree_);
  rois_->load(QString("%1/%2/%3-rois.xml")
	      .arg(dir).arg(exptname).arg(trialid));
  savedir = dir;
  Dbg() << "DataTrove::read" << trial_->exptName() << "/" << trial_->trialID();
}

void DataTrove::write(QString dir) {
  savedir = dir;

  if (dummy) {
    Dbg() << "DataTrove::write: not saving: dummy";
    return;
  }

  trial_->write(dir);
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
  if (savedir.isNull()) {
    warn("DataTrove::saveROIs: not saving - no directory set");
    return;
  }
  try {
    rois_->save(QString("%1/%2/%3-rois.xml")
		.arg(savedir).arg(trial_->exptName()).arg(trial_->trialID()));
  } catch (Exception const &e) {
    e.report();
    warn("DataTrove::saveROIs: not saved - exception");
  }
}
