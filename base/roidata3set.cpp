// roidata3set.cpp

#include "roidata3set.h"
#include <base/roi3data.h>
#include <base/roidata.h>
#include <base/ccddata.h>
#include <base/dbg.h>

ROIData3Set::ROIData3Set(ROISet const *rs) {
  d.roiset = rs;
  d.lastDebleach = DB_None;

  connect(rs, SIGNAL(newDatum(int)),
	  SLOT(updateROI(int)));
  connect(rs, SIGNAL(newAll()), SLOT(updateROIs()));
}

ROIData3Set::~ROIData3Set() {
  foreach (ROI3Data *rd, d.data.values()) 
    delete rd;
}


void ROIData3Set::setCCDData(QString id, CCDData const *data) {
  AllKeyGuard guard(*this);
  //  Dbg()<<"ROIData3Set("<<this<<"): setdata:"<<id<<"="<<data;
  d.ccdData[id] = data;
  foreach (int roiid, d.data.keys()) {
    CamPair const &cp = d.roiset->cam(roiid);
    if (cp.donor==id || cp.acceptor==id) {
      ROI3Data *dat = d.data[roiid];
      CCDData const *don = 0;
      CCDData const *acc = 0;
      if (d.ccdData.contains(cp.donor))
	don = d.ccdData[cp.donor];
      if (d.ccdData.contains(cp.acceptor))
	acc = d.ccdData[cp.acceptor];
      dat->setData(don, acc);
    }
  }
  updateCCDData();
}

void ROIData3Set::updateCCDData() {
  AllKeyGuard guard(*this);
  //  dbg("ROIData3Set(%p)::updateData. roiset=%p. n=%i",this,d.roiset,d.data.keys().size());
  foreach (ROI3Data *rd, d.data.values())
    rd->updateData();
}

ROI3Data const *ROIData3Set::getData(int id) const {
  if (d.data.contains(id))
    return d.data[id];
  else
    throw Exception("ROIData3Set","Request for unknown ID: "
		    +QString::number(id), "getData");
}

ROI3Data *ROIData3Set::accessData(KeyAccess::WriteKey *key) {
  int id = getID(key);
  if (d.data.contains(id))
    return d.data[id];
  else
    throw Exception("ROIData3Set","Request for unknown ID: "
		    + QString::number(id), "accessData");
}

ROI3Data *ROIData3Set::accessData(KeyAccess::WriteKey *key, int id) {
  verifyAllKey(key);
  if (d.data.contains(id))
    return d.data[id];
  else
    throw Exception("ROIData3Set","Request for unknown ID: "
		    + QString::number(id), "accessData");
}

ROICoords const &ROIData3Set::getCoords(int id) const {
  if (d.roiset->contains(id))
    return d.roiset->get(id);
  else
    throw Exception("ROIData3Set","Request for unknown ID: "
		    + QString::number(id), "getCoords");
}

CamPair const &ROIData3Set::getCam(int id) const {
  if (d.roiset->contains(id))
    return d.roiset->cam(id);
  else
    throw Exception("ROIData3Set","Request for unknown ID: "
		    + QString::number(id), "getCam");
}

bool ROIData3Set::haveData(int id) const {
  return d.data.contains(id);
}

void ROIData3Set::setDebleach(DEBLEACH db) {
  AllKeyGuard guard(*this);
  d.lastDebleach = db;
  foreach (ROI3Data *rd, d.data.values())
    rd->setDebleach(db);
}

QList<int> ROIData3Set::allIDs() const {
  return d.data.keys();
}

void ROIData3Set::updateROI(int id) {
  IDKeyGuard guard(*this, id);
  //  Dbg() << "ROIData3Set::changeROI("<<id<<")";
  changeROIcore(id);
}

void ROIData3Set::changeROIcore(int id) {
  bool exists = d.roiset && d.roiset->contains(id);
  bool existed = d.data.contains(id);
  if (exists) {
    if (!existed) {
      ROI3Data *dat = new ROI3Data();
      d.data.insert(id,dat);
      dat->setDebleach(d.lastDebleach);
      CCDData const *don = 0;
      CCDData const *acc = 0;
      CamPair const &cp = d.roiset->cam(id);
      if (d.ccdData.contains(cp.donor))
	don = d.ccdData[cp.donor];
      if (d.ccdData.contains(cp.acceptor))
	acc = d.ccdData[cp.acceptor];
      dat->setData(don, acc);
    }
    d.data[id]->setROI(&d.roiset->get(id));
  }
  if (existed && !exists) {
    delete d.data[id];
    d.data.remove(id);
  }
}

void ROIData3Set::updateROIs() {
  AllKeyGuard guard(*this);
  //  Dbg() << "ROIData3Set("<<this<<")::changeROIs()";
  QSet<int> ids = QSet<int>::fromList(d.data.keys());
  if (d.roiset)
    ids += d.roiset->ids();
  foreach (int id, ids)
    changeROIcore(id);
}

