// pvp/ccdregion.cpp - This file is part of VScope.
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

// ccdregion.cpp

#include "ccdregion.h"

CCDRegion::CCDRegion() {
  smin=0;
  smax=511;

  pmin=0;
  pmax=511;
}

CCDRegion::CCDRegion(int smin_, int smax_, int pmin_, int pmax_) {
  smin = smin_;
  smax = smax_;

  pmin = pmin_;
  pmax = pmax_;
}

CCDRegion::CCDRegion(QRect const &r0) {
  QRect r = r0.normalized();
  smin = r.left();
  smax = r.right();
  pmin = r.top();
  pmax = r.bottom();
}

CCDRegion CCDRegion::flipSerial(int serpix) const {
  CCDRegion r = *this;
  r.smin = serpix-1-smax;
  r.smax = serpix-1-smin;
  return r;
}

CCDRegion CCDRegion::flipParallel(int parpix) const {
  CCDRegion r = *this;
  r.pmin = parpix-1-pmax;
  r.pmax = parpix-1-pmin;
  return r;
}
