// roi3data.cpp

#include "roi3data.h"
#include <base/ccddata.h>
#include <base/memalloc.h>
#include <base/dbg.h>

ROI3Data::ROI3Data() {
  datRatio=0;
  nRatio=0;
  t0Ratio_ms = 0;
  dtRatio_ms = 0;
  validRatio=false;
  debleach="None";
}

ROI3Data::~ROI3Data() {
  if (datRatio)
    delete [] datRatio;
}

void ROI3Data::setROI(ROICoords const *roi) {
  datDonor.setROI(roi);
  datAcceptor.setROI(roi);
  validRatio = false;
}

void ROI3Data::setData(CCDData const *donor,
		       CCDData const *acceptor,
		       bool noupdate) {
  datDonor.setData(donor);
  datAcceptor.setData(acceptor);
  if (!noupdate)
    updateData();
}

void ROI3Data::updateData() {
  // Following is crude. Actually should think about appropriate time
  // if donor and acceptor are not the same.
  datDonor.updateData();
  datAcceptor.updateData();
  bool useDonor = datDonor.haveData();
  t0Ratio_ms = useDonor ? getDonorT0ms() : getAcceptorT0ms();
  dtRatio_ms = useDonor ? getDonorDTms() : getAcceptorDTms();
  int newNRatio = useDonor ? getDonorNFrames() : getAcceptorNFrames();
  if (newNRatio != nRatio) {
    if (datRatio)
      delete datRatio;
    datRatio = 0;
    nRatio = newNRatio;
  }
  validRatio = false;
}

void ROI3Data::setDebleach(QString d) {
  if (d!=debleach) {
    debleach = d;
    datDonor.setDebleach(d);
    datAcceptor.setDebleach(d);
    validRatio = false;
  }
}

double const *ROI3Data::dataDonor() const {
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

double const *ROI3Data::dataAcceptor() const {
  return datAcceptor.getDebleachedDFF();
}



double const *ROI3Data::dataRatio() const {
  //Dbg() << "ROI3Data::dataRatio valid="<<validRatio
  //	<< " donor.have="<<datDonor.haveData()
  //	<< " donor.data="<<dataDonor()
  //	<< " donor.n="<<datDonor.getNFrames()
  //	<< " acc.have="<<datAcceptor.haveData()
  //	<< " acc.data="<<dataAcceptor()
  //	<< " acc.n="<<datAcceptor.getNFrames()
  //	<< " datRatio="<<datRatio
  //	<< " nRatio="<<nRatio;
  if (validRatio)
    return datRatio;

  if (!datDonor.haveData() || !datAcceptor.haveData()) {
    if (datDonor.haveData())
      return dataDonor();
    else if (datAcceptor.haveData())
      return dataAcceptor();
    else
      return 0;
  }

  if (nRatio==0)
    return 0;

  if (!datRatio) 
    datRatio = memalloc<double>(nRatio, "ROI3Data");

  double const *dd = dataDonor();
  double const *da = dataAcceptor();
  
  for (int n=0; n<nRatio; n++)
    datRatio[n] = dd[n] - da[n];

  validRatio = true;
  return datRatio;
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
 
