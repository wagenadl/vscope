// base/roicoords.h - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// roicoords.h

#ifndef ROICOORDS_H

#define ROICOORDS_H

#include <base/xyrra.h>
#include <base/polyblob.h>

class ROICoords {
public:
  ROICoords();
  ROICoords(PolyBlob const &blob);
  ROICoords(XYRRA const &xyrra);
  ROICoords(ROICoords const &roi);
  ~ROICoords();
  ROICoords(QDomElement doc);
  ROICoords &operator=(XYRRA const &xyrra);
  ROICoords &operator=(PolyBlob const &blob);
  ROICoords &operator=(ROICoords const &roi);
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
  QPointF center() const;
  bool inside(double x, double y, double marg) const;
  bool inside(QPointF xy, double marg) const;
private:
  XYRRA *dataXYRRA;
  PolyBlob *dataBlob;
};

#endif
