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
  QMap<QString, class CCDData const *> ccdData; // we do not own this
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
  class ROI3Data *getData(int id) const; // exception if non-existant
  ROICoords const &getCoords(int id) const;
  CamPair const &getCam(int id) const;
  ROISet const *getROISet() const { return d.roiset; }
  bool haveData(int id) const;
  QList<int> allIDs() const;
  void setData(QString camid, class CCDData const *acceptor); // does not cause updateData. Call updateData directly
public slots:
  void changeROI(int id);
  void changeROIs();
  void updateData();
  void setDebleach(ROIData::Debleach d);
signals:
  void changedOne(int id);
  void changedAll();
private:
  void changeROIcore(int id);
private:
  ROIData3Set_Data d;
};

#endif
