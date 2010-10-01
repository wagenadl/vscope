// roiset3data.h

#ifndef ROISET3DATA_H

#define ROISET3DATA_H

#include <QMap>
#include <base/xyrra.h>
#include <base/roidata.h>
#include <base/roi3data.h>

class ROISet3Data_ {
public:
  QMap<int, class ROI3Data *> data;
  double t0_ms, dt_ms;
  class CCDData const *lastDonor, *lastAcceptor;
  ROIData::Debleach lastDebleach;
};

class ROISet3Data: private ROISet3Data_ {
public:
  ROISet3Data();
  virtual ~ROISet3Data();
  ROISet3Data(ROISet3Data const &other);
  ROISet3Data &operator=(ROISet3Data const &other);
  void setROI(int id, XYRRA el);
  void setROI(int id, class PolyBlob const *pb);
  void removeROI(int id);
  void clearROIs();
  void setData(class CCDData const *donor, class CCDData const *acceptor);
  void setDebleach(ROIData::Debleach d);
  class ROI3Data *getData(int id) const;
  bool haveData(int id) const;
  double getT0_ms() const { return t0_ms; } // only valid after setData
  double getDT_ms() const { return dt_ms; }
private:
  void setROIbase(int id);
  void copy(ROISet3Data const &other);
};

#endif
