// math/taperid.cpp - This file is part of VScope.
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

// taperid.cpp

#include "taperid.h"
#include <base/minmax.h>

TaperID::TaperID(int N, double nw):
  N(N), nw(nw) {
  K = floori(2*nw - 1);
}

TaperID::TaperID(int len, double dt, double df):
  N(len),
  nw(len*dt*df) {
  K = floori(2*nw - 1);
}

bool TaperID::operator==(TaperID const &b) const {
  return N==b.N && nw==b.nw;
}

double TaperID::freqRes(double dt) const {
  return nw/(N*dt);
}

QString TaperID::name() const {
  return QString("%1-%2").arg(N).arg(nw,0,'f',2);
}

uint qHash(TaperID const &tid) {
  return uint(tid.N) + uint(172891*tid.nw);
}
