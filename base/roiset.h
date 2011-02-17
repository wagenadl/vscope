// roiset.h

#ifndef ROISET_H

#define ROISET_H

#include <QDomElement>
#include <QSet>
#include <QMap>
#include <base/xyrra.h>
#include <base/polyblob.h>

class ROISet {
  /*:C ROISet
   *:D This is the list of ROI definitions. It does not contain data for
       any particular actual image.
  */
public:
  ROISet();
  ~ROISet();
  void set(int id, XYRRA const &roi);
  void set(int id, PolyBlob const &roip); // this makes a copy
  PolyBlob &newblob(int id);
  PolyBlob &newblob(int id, int log2n);
  XYRRA get(int id) const; // throws exception if not found
  PolyBlob const &getp(int id) const; // throws exception if not found (!)
  PolyBlob &getp(int id); // throws exception if not found
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
  void clear();
private:
  QSet<int> allids;
  QMap<int, XYRRA> emap;
  QMap<int, PolyBlob> pmap;
};

#endif
