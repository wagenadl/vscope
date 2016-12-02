// roi3data.cpp

#include "roi3data.h"
#include <base/ccddata.h>
#include <base/dbg.h>

ROI3Data::ROI3Data() {
  t0Ratio_ms = 0;
  dtRatio_ms = 0;
  durRatio_ms = 0;
  validRatio=false;
  debleach=DB_None;
  haveRatio = false;
}

ROI3Data::~ROI3Data() {
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
  durRatio_ms = useDonor ? getDonorDurms() : getAcceptorDurms();
  haveRatio = useDonor && getDonorNFrames() == getAcceptorNFrames();
  if (datDonor.haveData() && datAcceptor.haveData() && !haveRatio) {
    Dbg() << "ROI3Data cannot yet ratio if frame count doesn't match.";
    Dbg() << "Returning unratioed donor data.";
  }
  datRatio.resize(haveRatio ? getDonorNFrames() : 0);
  validRatio = false;
}

void ROI3Data::setDebleach(DEBLEACH d) {
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

double ROI3Data::getAcceptorDurms() const {
  return datAcceptor.getDurms();
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

double ROI3Data::getDonorDurms() const {
  return datDonor.getDurms();
}

int ROI3Data::getRatioNFrames() const {
  if (haveRatio)
    return datRatio.size();
  else if (datDonor.haveData())
    return getDonorNFrames();
  else if (datAcceptor.haveData())
    return getAcceptorNFrames();
  else
    return 0;

}

double ROI3Data::getRatioT0ms() const {
  return t0Ratio_ms;
}


double ROI3Data::getRatioDTms() const {
  return dtRatio_ms;
}

double ROI3Data::getRatioDurms() const {
  return durRatio_ms;
}

double const *ROI3Data::dataAcceptor() const {
  return datAcceptor.getDebleachedDFF();
}



double const *ROI3Data::dataRatio() const {
  if (validRatio)
    return datRatio.constData();

  if (haveRatio) {
    double const *dd = dataDonor();
    double const *da = dataAcceptor();
    double *dr = datRatio.data();
    int N = datRatio.size();
    for (int n=0; n<N; n++)
      *dr++ = *dd++ - *da++;
    
    validRatio = true;
    return datRatio.constData();
  } else if (datDonor.haveData()) {
    return dataDonor();
  } else if (datAcceptor.haveData()) {
    return dataAcceptor();
  } else {
    return 0;
  }
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
 
