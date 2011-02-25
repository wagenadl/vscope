// roidata3set.cpp

#include "roidata3set.h"
#include <base/roi3data.h>
#include <base/roidata.h>
#include <base/ccddata.h>
#include <base/dbg.h>

ROIData3Set::ROIData3Set(ROISet const *rs) {
  d.roiset = rs;
  d.lastDonor = d.lastAcceptor = 0;
  d.lastDebleach = ROIData::None;

  connect(rs, SIGNAL(changed(int)),
	  SLOT(changeROI(int)));
  connect(rs, SIGNAL(changedAll()),
	  SLOT(changeROIs()));
}

ROIData3Set::~ROIData3Set() {
  foreach (ROI3Data *rd, d.data.values()) 
    delete rd;
}


void ROIData3Set::setData(CCDData const *donor,
			  CCDData const *acceptor) {
  d.lastDonor = donor;       // For the benefit of
  d.lastAcceptor = acceptor; //    future ROIs.
  updateData();
}

void ROIData3Set::updateData() {
  if (!d.lastDonor)
    return;
  foreach (ROI3Data *rd, d.data.values())
    rd->setData(d.lastDonor, d.lastAcceptor);
  d.t0_ms = d.lastDonor->getT0();
  d.dt_ms = d.lastDonor->getDT();
  d.nframes = d.lastDonor->getNFrames();
  emit changedAll();
}

ROI3Data *ROIData3Set::getData(int id) const {
  if (d.data.contains(id))
    return d.data[id];
  else
    throw Exception("ROIData3Set","Request for unknown ID");
}

ROICoords const &ROIData3Set::getCoords(int id) const {
  if (d.roiset->contains(id))
    return d.roiset->get(id);
  else
    throw Exception("ROIData3Set","Request for unknown ID");
}

CamPair const &ROIData3Set::getCam(int id) const {
  if (d.roiset->contains(id))
    return d.roiset->cam(id);
  else
    throw Exception("ROIData3Set","Request for unknown ID");
}

bool ROIData3Set::haveData(int id) const {
  return d.data.contains(id);
}

void ROIData3Set::setDebleach(ROIData::Debleach db) {
  d.lastDebleach = db;
  foreach (ROI3Data *rd, d.data.values())
    rd->setDebleach(db);
  emit changedAll();
}

QList<int> ROIData3Set::allIDs() const {
  return d.data.keys();
}

void ROIData3Set::changeROI(int id) {
  changeROIcore(id);
  emit changedOne(id);
}

void ROIData3Set::changeROIcore(int id) {
  bool exists = d.roiset && d.roiset->contains(id);
  bool existed = d.data.contains(id);
  if (exists) {
    if (!existed) {
      ROI3Data *dat = new ROI3Data();
      d.data.insert(id,dat);
      dat->setDebleach(d.lastDebleach);
      dat->setData(d.lastDonor,d.lastAcceptor);
    }
    d.data[id]->setROI(&d.roiset->get(id));
  }
  if (existed && !exists) {
    delete d.data[id];
    d.data.remove(id);
  }
}

void ROIData3Set::changeROIs() {
  QSet<int> ids = QSet<int>::fromList(d.data.keys());
  if (d.roiset)
    ids += d.roiset->ids();
  foreach (int id, ids)
    changeROIcore(id);
  emit changedAll();
}

ROIData3Set::ROIData3Set(ROIData3Set const &other):
  QObject(0), d(other.d) {
  foreach (int id, d.data.keys())
    d.data[id] = new ROI3Data(*other.d.data[id]); // make a deep copy
}

ROIData3Set &ROIData3Set::operator=(ROIData3Set const &other) {
  foreach (ROI3Data *rd, d.data.values())
    delete rd;
  d = other.d;
  foreach (int id, d.data.keys())
    d.data[id] = new ROI3Data(*other.d.data[id]); // make a deep copy
  return *this;
}

