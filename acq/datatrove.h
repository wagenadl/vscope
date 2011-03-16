// DataTrove.h

#ifndef DATATROVE_H

#define DATATROVE_H

#include <base/xml.h>
#include <base/keyagg.h>

class DataTrove: public KeyAgg {
  Q_OBJECT;
public:
  DataTrove(class ParamTree *ptree);
  DataTrove(QDomElement elt);
  virtual ~DataTrove();
public slots:
  void read(QString dir, QString exptname, QString trialid);
  void write(); // does nothing if dummy is set
  void saveROIs(); // only works after write().
  void setDummy(bool dummy);
  /*:F setDummy
   *:D If set, saving is disabled and the write() and saveROIs() slots do
       nothing.
  */
public:
  class TrialData &trial() { return *trial_; }
  class ROISet &rois() { return *rois_; }
  class ROIData3Set &roidata() { return *roidata_; }
  class CohData &cohdata() { return *cohdata_; }
private:
  class ParamTree *ptree_;
  class TrialData *trial_;
  class ROISet *rois_;
  class ROIData3Set *roidata_;
  class CohData *cohdata_;
private:
  void constructData();
private:
  bool ownptree;
  bool dummy;
private: // not implemented
  DataTrove(DataTrove const &);
  DataTrove &operator=(DataTrove const &);
};

#endif
