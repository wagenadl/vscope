// acq/epho_ccd.h - This file is part of VScope.
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
  AllCCDTimingDetail const &timing; /* We do not own this */
};

#endif
