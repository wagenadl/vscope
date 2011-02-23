// roiset3data.cpp

#include "roiset3data.h"
#include <base/roi3data.h>
#include <base/roidata.h>
#include <base/ccddata.h>
#include <base/dbg.h>

ROISet3Data::ROISet3Data(ROISet const *rs) {
  roiset = rs;
  lastDonor = lastAcceptor = 0;
  lastDebleach = ROIData::None;
}

ROISet3Data::~ROISet3Data() {
  for (QMap<int, ROI3Data *>::iterator i=data.begin(); i!=data.end(); ++i) 
    delete i.value();
}


void ROISet3Data::setData(CCDData const *donor,
			  CCDData const *acceptor) {
  lastDonor = donor;       // For the benefit of
  lastAcceptor = acceptor; //    future ROIs.
  foreach (ROI3Data *rd, data.values())
    rd->setData(donor, acceptor);
  t0_ms = donor->getT0();
  dt_ms = donor->getDT();
}

ROI3Data *ROISet3Data::getData(int id) const {
  if (data.contains(id))
    return data[id];
  else
    throw Exception("ROISet3Data","Request for unknown ID");
}

bool ROISet3Data::haveData(int id) const {
  return data.contains(id);
}

void ROISet3Data::setDebleach(ROIData::Debleach d) {
 lastDebleach = d;
 foreach (ROI3Data *rd, data.values())
   rd->setDebleach(d);
}

void ROISet3Data::changeROI(int id) {
  if (roiset && roiset->contains(id)) {
    if (!data.contains(id)) {
      ROI3Data *dat = new ROI3Data();
      data.insert(id,dat);
      dat->setDebleach(lastDebleach);
      dat->setData(lastDonor,lastAcceptor);
    }
    data[id]->setROI(&roiset->get(id));
  } else {
    if (data.contains(id)) {
      delete data[id];
      data.remove(id);
    }
  }
}

void ROISet3Data::changeROIs() {
  if (roiset) {
    foreach (int id, roiset->ids()) 
      changeROI(id);
  } else {
    foreach (ROI3Data *rd, data.values())
      delete rd;
    data.clear();
  }
}

ROISet3Data::ROISet3Data(ROISet3Data const &other): ROISet3Data_(other) {
  foreach (int id, data.keys())
    data[id] = new ROI3Data(*data[id]); // make a deep copy
}

ROISet3Data &ROISet3Data::operator=(ROISet3Data const &other) {
  foreach (ROI3Data *rd, data.values())
    delete rd;
  *(ROISet3Data_*)this = other;
  foreach (int id, data.keys())
    data[id] = new ROI3Data(*data[id]); // make a deep copy
  return *this;
}

