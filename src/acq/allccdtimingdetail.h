// allccdtimingdetail.h

#ifndef ALLCCDTIMINGDETAIL_H

#define ALLCCDTIMINGDETAIL_H

#include <QString>
#include <QMap>
#include <xml/paramtree.h>
#include <acq/ccdtimingdetail.h>

class AllCCDTimingDetail {
public:
  AllCCDTimingDetail();
  ~AllCCDTimingDetail();
  void prepTrial(ParamTree const *ptree);
  void prepSnap(ParamTree const *ptree);
  int neededScans() const;
  CCDTimingDetail const &operator[](QString camid) const;
  CCDTimingDetail const &first() const; // temp.
  bool isSnap() const { return is_snap; }
  bool has(QString id) const { return details.contains(id); }
private:
  void updateCameras(ParamTree const *ptree);
private:
  bool is_snap;
  QMap<QString, CCDTimingDetail *> details;
};

#endif
