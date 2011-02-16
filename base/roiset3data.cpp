// roiset3data.cpp

#include "roiset3data.h"
#include <base/roi3data.h>
#include <base/roidata.h>
#include <base/ccddata.h>
#include <base/dbg.h>

ROISet3Data::ROISet3Data() {
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

void ROISet3Data::setROI(int id, XYRRA el) {
  setROIbase(id);
  data[id]->setROI(el);
}

void ROISet3Data::setROI(int id, class PolyBlob const *pb) {
  setROIbase(id);
  data[id]->setROI(pb);
}

void ROISet3Data::setROIbase(int id) {
  if (!data.contains(id)) {
    ROI3Data *dat = new ROI3Data();
    data.insert(id,dat);
    dat->setDebleach(lastDebleach);
    dat->setData(lastDonor,lastAcceptor);
  }
}

void ROISet3Data::removeROI(int id) {
  if (data.contains(id)) {
    delete data[id];
    data.remove(id);
  }
}

void ROISet3Data::clearROIs() {
  foreach (ROI3Data *rd, data.values())
    delete rd;
  data.clear();
}

ROISet3Data::ROISet3Data(ROISet3Data const &other): ROISet3Data_(other) {
  copy(other);
}

void ROISet3Data::copy(ROISet3Data const &other) {
  for (QMap<int, ROI3Data *>::iterator i=data.begin(); i!=data.end(); ++i) 
    if (*i) 
      *i = new ROI3Data(*other.getData(i.key()));
}

ROISet3Data &ROISet3Data::operator=(ROISet3Data const &other) {
  clearROIs();
  *(ROISet3Data_*)this = other;
  copy(other);
  return *this;
}

