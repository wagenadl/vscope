// acq/ccdtimingdetail.h - This file is part of VScope.
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

// ccdtimingdetail.h

#ifndef CCDTIMINGDETAIL_H

#define CCDTIMINGDETAIL_H

#include <base/ccdtiming.h>
#include <xml/paramtree.h>

class CCDTimingDetail: public CCDTiming {
public:
  CCDTimingDetail();
  void prepTrial(ParamTree const *ptree, ParamTree const *camtree);
  void prepSnap(ParamTree const *ptree, ParamTree const *camtree);
  int neededScans() const { return nscans; }
  /*:F neededScans
   *:D Calculates the minimum length a ddata structure should have to
       accommodate a complete trial / snapshot.
  */
  bool isSnap() const { return is_snap; }
  int illumPreScans() const { return illumprescans; }
  int illumPostScans() const { return illumpostscans; }
  int shtrPreScans() const { return shtrprescans; }
  int shtrPostScans() const { return shtrpostscans; }
  int preHeatFrames() const { return preheat_frames; }
  bool trigEach() const { return trig_each; }
private:
  void generalPrep(ParamTree const *ptree, ParamTree const *camtree);
private:
  bool is_snap;
  int nscans; // Total number of scans in trial
  int illumprescans, illumpostscans;
  int shtrprescans, shtrpostscans;
  int preheat_frames;
  bool trig_each;
};

#endif
