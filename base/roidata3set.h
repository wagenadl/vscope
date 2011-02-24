// roidata3set.h

#ifndef ROIDATA3SET_H

#define ROIDATA3SET_H

#include <QMap>
#include <base/roiset.h>
#include <base/roidata.h>
#include <base/roi3data.h>
#include <QObject>

struct ROIData3Set_Data {
public:
  QMap<int, class ROI3Data *> data;
  double t0_ms, dt_ms;
  class CCDData const *lastDonor, *lastAcceptor;
  ROIData::Debleach lastDebleach;
  ROISet const *roiset;
};

class ROIData3Set: public QObject {
  /*:C ROIData3Set
   *:D ROIData3Set maintains a collection of ROI3Datas, i.e., data extracted
       from a number of ROIs in a particular (donor,acceptor) image pair.
  */
  Q_OBJECT;
public:
  ROIData3Set(ROISet const *coordset);
  virtual ~ROIData3Set();
  ROIData3Set(ROIData3Set const &other);
  ROIData3Set &operator=(ROIData3Set const &other);
  class ROI3Data *getData(int id) const;
  bool haveData(int id) const;
  double getT0_ms() const { return d.t0_ms; } // only valid after setData
  double getDT_ms() const { return d.dt_ms; }
public slots:
  void changeROI(int id);
  void changeROIs();
  void setData(class CCDData const *donor, class CCDData const *acceptor);
  void setDebleach(ROIData::Debleach d);
signals:
  void changedOne(int id);
  void changedAll();
private:
  ROIData3Set_Data d;
};

#endif
