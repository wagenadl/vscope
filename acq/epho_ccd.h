// epho_ccd.h

#ifndef EPHO_CCD_H

#define EPHO_CCD_H

#include <xml/paramtree.h>
#include <xml/connections.h>
#include <base/digitaldata.h>
#include <acq/allccdtimingdetail.h>

class EPhO_CCD {
public:
  EPhO_CCD(AllCCDTimingDetail const &timing);
  /*:F Constructor
   *:N The contents of the ptree must not be changed between construction
       time and last use of this class instance.
   */
  void prepare(DigitalData *ddata) const;
  /*:F prepare
   *:D Sets up relevant digital lines for trial/snapshot acquisition.
   *:N If the dddata is not long enough to accommodate a full trial
       or snapshot, we throw an exception, so call neededScans() first.
  */
private:
  CCDTimingDetail const &timing; /* We do not own this */
};

#endif
