// ccdtimingdetail.h

#ifndef CCDTIMINGDETAIL_H

#define CCDTIMINGDETAIL_H

#include <base/ccdtiming.h>
#include <xml/paramtree.h>

class CCDTimingDetail: public CCDTiming {
public:
  CCDTimingDetail(ParamTree const *ptree, bool snap);
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
  bool is_snap;
  int nscans; // Total number of scans in trial
  int illumprescans, illumpostscans;
  int shtrprescans, shtrpostscans;
  int preheat_frames;
  bool trig_each;
};

#endif
