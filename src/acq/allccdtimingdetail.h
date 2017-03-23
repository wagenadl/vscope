// acq/allccdtimingdetail.h - This file is part of VScope.
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
