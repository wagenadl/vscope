// roiset.h

#ifndef ROISET_H

#define ROISET_H

#include <QDomElement>
#include <QSet>
#include <QMap>
#include <base/roicoords.h>

class ROISet {
  /*:C ROISet
   *:D This is the list of ROI definitions. It does not contain data for
       any particular actual image.
  */
public:
  ROISet(QString dfltcam="");
  ~ROISet();
  ROICoords &insert(int id, QString campair=QString());
  ROICoords const &get(int id) const; // throws exception if not found
  ROICoords &get(int id); // throws exception if not found
  QString const &cam(int id) const;
  double centerX(int id) const;
  double centerY(int id) const;
  bool inside(int id, double x, double y, double marg) const;
  bool contains(int id) const;
  bool isXYRRA(int id) const;
  bool isPoly(int id) const;
  void remove(int id);
  void write(QDomElement dst) const;
  void save(QString fn) const;
  void read(QDomElement src);
  void load(QString fn);
  QSet<int> const &ids() const;
  QSet<int> idsForCam(QString cam) const;
  void clear();
private:
  QSet<int> allids;
  QMap<int, QString> cams;
  QMap<int, ROICoords> map;
  QString dfltcam;
};

#endif
