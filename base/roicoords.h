// roicoords.h

#ifndef ROICOORDS_H

#define ROICOORDS_H

#include <base/xyrra.h>
#include <base/polyblob.h>

class ROICoords {
public:
  ROICoords();
  ~ROICoords();
  ROICoords(QDomElement doc);
  ROICoords &operator=(XYRRA const &xyrra);
  ROICoords &operator=(PolyBlob const &blob);
  void makeBlob(int log2n=6);
  void makeXYRRA();
  bool isXYRRA() const;
  bool isBlob() const;
  PolyBlob const &blob() const;
  PolyBlob &blob();
  XYRRA const &xyrra() const;
  XYRRA &xyrra();
  void read(QDomElement doc);
  void write(QDomElement doc) const;
  double centerX() const;
  double centerY() const;
  bool inside(double x, double y, double marg) const;
private:
  XYRRA *dataXYRRA;
  PolyBlob *dataBlob;
};

#endif
