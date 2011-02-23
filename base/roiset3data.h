// roiset3data.h

#ifndef ROISET3DATA_H

#define ROISET3DATA_H

#include <QMap>
#include <base/roiset.h>
#include <base/roidata.h>
#include <base/roi3data.h>

class ROISet3Data_ {
public:
  QMap<int, class ROI3Data *> data;
  double t0_ms, dt_ms;
  class CCDData const *lastDonor, *lastAcceptor;
  ROIData::Debleach lastDebleach;
  ROISet const *roiset;
};

class ROISet3Data: private ROISet3Data_ {
  /*:C ROISet3Data
   *:D ROISet3Data maintains a collection of ROI3Datas, i.e., data extracted
       from a number of ROIs in a particular (donor,acceptor) image pair.
  */
public:
  ROISet3Data(ROISet const *coordset);
  virtual ~ROISet3Data();
  ROISet3Data(ROISet3Data const &other);
  ROISet3Data &operator=(ROISet3Data const &other);
  void changeROI(int id);
  void changeROIs();
  void setData(class CCDData const *donor, class CCDData const *acceptor);
  void setDebleach(ROIData::Debleach d);
  class ROI3Data *getData(int id) const;
  bool haveData(int id) const;
  double getT0_ms() const { return t0_ms; } // only valid after setData
  double getDT_ms() const { return dt_ms; }
};

#endif
