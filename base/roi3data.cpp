// roi3data.cpp

#include "roi3data.h"
#include <base/ccddata.h>
#include <base/memalloc.h>

bool ROI3Data_::donorflipx = false;
bool ROI3Data_::donorflipy = false;
bool ROI3Data_::acceptorflipx = false;
bool ROI3Data_::acceptorflipy = false;

ROI3Data::ROI3Data() {
  datRatio=0;
  nRatio=0;
  valid=false;
  debleach=ROIData::None;
  datDonor.setFlip(donorflipx, donorflipy);
  datAcceptor.setFlip(acceptorflipx, acceptorflipy);
}

ROI3Data::~ROI3Data() {
  if (datRatio)
    delete [] datRatio;
}

void ROI3Data::setROI(XYRRA el) {
  datDonor.setROI(el);
  datAcceptor.setROI(el);
  valid = false;
}

void ROI3Data::setROI(class PolyBlob const *pb) {
  datDonor.setROI(pb);
  datAcceptor.setROI(pb);
  valid = false;
}

void ROI3Data::setData(CCDData const *donor, CCDData const *acceptor) {
  datDonor.setData(donor);
  datAcceptor.setData(acceptor);
  valid = false;
}

void ROI3Data::setDebleach(ROIData::Debleach d) {
  if (d!=debleach) {
    debleach = d;
    datDonor.setDebleach(d);
    datAcceptor.setDebleach(d);
    valid = false;
  }
}

double const *ROI3Data::dataDonor() {
  return datDonor.getDebleachedDFF();
}

int ROI3Data::getNFrames() const {
  return datDonor.getNFrames();
}

double const *ROI3Data::dataAcceptor() {
  return datAcceptor.getDebleachedDFF();
}

double const *ROI3Data::dataRatio() {
  if (valid)
    return datRatio;

  if (!datAcceptor.haveData())
    return dataDonor();

  if (datRatio && nRatio!=datDonor.getNFrames()) {
    delete [] datRatio;
    datRatio = 0;
  }

  if (!datRatio) {
    nRatio = datDonor.getNFrames();
    if (nRatio==0)
      return 0;
    datRatio = memalloc<double>(nRatio, "ROI3Data");
  }

  double const *dd = dataDonor();
  double const *da = dataAcceptor();
  
  for (int n=0; n<nRatio; n++)
    datRatio[n] = dd[n] - da[n];

  valid = true;

  return datRatio;
}

void ROI3Data::setFlip(bool donflipx, bool donflipy,
		       bool accflipx, bool accflipy) {
  donorflipx=donflipx;
  donorflipy=donflipy;
  acceptorflipx=accflipx;
  acceptorflipy=accflipy;
}

ROI3Data::ROI3Data(ROI3Data const &other): ROI3Data_(other) {
  datRatio=0;
  copy(other);
}

void ROI3Data::copy(ROI3Data const &other) {
  if (other.datRatio) {
    datRatio = memalloc<double>(nRatio, "ROI3Data");
    memcpy(datRatio, other.datRatio, nRatio*sizeof(double));
  }
}

ROI3Data &ROI3Data::operator=(ROI3Data const &other) {
  if (datRatio)
    delete [] datRatio;
  *(ROI3Data_*)this = other;
  datRatio=0;
  copy(other);
  return *this;
}

  
  
