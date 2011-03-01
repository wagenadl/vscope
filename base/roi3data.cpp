// roi3data.cpp

#include "roi3data.h"
#include <base/ccddata.h>
#include <base/memalloc.h>
#include <base/dbg.h>

bool ROI3Data_::donorflipx = false;
bool ROI3Data_::donorflipy = false;
bool ROI3Data_::acceptorflipx = false;
bool ROI3Data_::acceptorflipy = false;

ROI3Data::ROI3Data() {
  datRatio=0;
  nRatio=0;
  t0Ratio_ms = 0;
  dtRatio_ms = 0;
  valid=false;
  debleach=ROIData::None;
  datDonor.setFlip(donorflipx, donorflipy);
  datAcceptor.setFlip(acceptorflipx, acceptorflipy);
}

ROI3Data::~ROI3Data() {
  if (datRatio)
    delete [] datRatio;
}

void ROI3Data::setROI(ROICoords const *roi) {
  datDonor.setROI(roi);
  datAcceptor.setROI(roi);
  valid = false;
}

void ROI3Data::setData(CCDData const *donor, CCDData const *acceptor) {
  datDonor.setData(donor);
  datAcceptor.setData(acceptor);
  // Following is crude. Actually should think about appropriate time
  // if donor and acceptor are not the same.
  t0Ratio_ms = donor ? getDonorT0ms() : getAcceptorT0ms();
  dtRatio_ms = donor ? getDonorDTms() : getAcceptorDTms();
  int newNRatio = donor ? getDonorNFrames() : getAcceptorNFrames();
  if (newNRatio != nRatio) {
    if (datRatio)
      delete datRatio;
    datRatio = 0;
    nRatio = newNRatio;
  }
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

int ROI3Data::getAcceptorNFrames() const {
  return datAcceptor.getNFrames();
}

double ROI3Data::getAcceptorT0ms() const {
  return datAcceptor.getT0ms();
}

double ROI3Data::getAcceptorDTms() const {
  return datAcceptor.getDTms();
}

int ROI3Data::getDonorNFrames() const {
  return datDonor.getNFrames();
}

double ROI3Data::getDonorT0ms() const {
  return datDonor.getT0ms();
}

double ROI3Data::getDonorDTms() const {
  return datDonor.getDTms();
}

int ROI3Data::getRatioNFrames() const {
  return nRatio;
}

double ROI3Data::getRatioT0ms() const {
  return t0Ratio_ms;
}

double ROI3Data::getRatioDTms() const {
  return dtRatio_ms;
}

double const *ROI3Data::dataAcceptor() {
  return datAcceptor.getDebleachedDFF();
}



double const *ROI3Data::dataRatio() {
  Dbg() << "ROI3Data::dataRatio";
  if (valid)
    return datRatio;

  if (!datDonor.haveData() && !datAcceptor.haveData())
    return 0;
  else if (!datAcceptor.haveData())
    return dataDonor();
  else if (!datDonor.haveData())
    return dataAcceptor();

  if (!datRatio) {
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

Range ROI3Data::timeRange() const {
  double t0 = getRatioT0ms()/1e3;
  double dt = getRatioDTms()/1e3;
  int nfr = getRatioNFrames();
  Range tt;
  if (nfr)
    tt=Range(t0,t0+dt*nfr);
  tt.expand(datDonor.timeRange());
  tt.expand(datAcceptor.timeRange());
  return tt;
}
 
