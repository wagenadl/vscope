// roidata3set.h

#ifndef ROIDATA3SET_H

#define ROIDATA3SET_H

#include <QMap>
#include <base/roiset.h>
#include <base/roidata.h>
#include <base/roi3data.h>
#include <base/idkeyaccess.h>

struct ROIData3Set_Data {
public:
  QMap<int, class ROI3Data *> data;
  QMap<QString, class CCDData const *> ccdData; // we do not own this
  DEBLEACH lastDebleach;
  ROISet const *roiset; // we do not own this
};

class ROIData3Set: public IDKeyAccess {
  /*:C ROIData3Set
   *:D ROIData3Set maintains a collection of ROI3Datas, i.e., data extracted
       from a number of ROIs in a particular (donor,acceptor) image pair.
  */
  Q_OBJECT;
public:
  ROIData3Set(ROISet const *coordset);
  virtual ~ROIData3Set();
  class ROI3Data const *getData(int id) const; // exception if non-existant
  class ROI3Data *accessData(IDKeyAccess::WriteKey *key);
  class ROI3Data *accessData(IDKeyAccess::WriteKey *key, int id);
  /*:F accessData
   *:D Accesses a certain ROI3Data using either a key that checked out that
       item, or using a key that checked out all (in which case the ID must be
       supplied explicitly).
  */
  ROICoords const &getCoords(int id) const;
  CamPair const &getCam(int id) const;
  ROISet const *getROISet() const { return d.roiset; }
  bool haveData(int id) const;
  QList<int> allIDs() const;
  void setCCDData(QString camid, class CCDData const *data);
  double getT0ms(QString camid) const;
  double getDTms(QString camid) const;
public slots:
  void updateROI(int id);
  void updateROIs();
  void updateCCDData();
  void setDebleach(DEBLEACH d);
private:
  void changeROIcore(int id);
private:
  ROIData3Set_Data d;
};

#endif
