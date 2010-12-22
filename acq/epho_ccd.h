// epho_ccd.h

#ifndef EPHO_CCD_H

#define EPHO_CCD_H

#include <xml/paramtree.h>
#include <xml/connections.h>
#include <base/digitaldata.h>
#include <base/ccdtiming.h>

class EPhO_CCD {
public:
  EPhO_CCD(ParamTree const *ptree, bool snap=false);
  /*:F Constructor
   *:N The contents of the ptree must not be changed between construction
       time and last use of this class instance.
   */
  CCDTiming const &timing() const { return timing_; }
  int neededScans() const;
  /*:F neededScans
   *:D Calculates the minimum length a ddata structure should have to
       accommodate a complete trial / snapshot.
  */
  void prepare(DigitalData *ddata) const;
  /*:F prepare
   *:D Sets up relevant digital lines for trial/snapshot acquisition.
   *:N If the dddata is not long enough to accommodate a full trial
       or snapshot, we throw an exception, so call neededScans() first.
  */
private:
  ParamTree const *ptree; /* We do not own this */
  bool isSnap;
  CCDTiming timing_;
  int nscans;
  int illumprescans, illumpostscans;
  int shtrprescans, shtrpostscans;
  int preheat_frames;
  bool trigEach;
};

#endif
