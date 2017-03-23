// base/campair.h - This file is part of VScope.
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

// campair.h

#ifndef CAMPAIR_H

#define CAMPAIR_H

#include <QString>
#include <QHash>
#include <QDebug>

class CamPair {
public:
  CamPair(QString d="", QString a=""): donor(d), acceptor(a) { }
public:
  QString donor; // or single camera if not a pair
  QString acceptor; // or empty
public:
  bool operator==(CamPair const &other) const {
    return donor==other.donor && acceptor==other.acceptor;
  }
  bool operator!=(CamPair const &other) const {
    return !operator==(other);
  }
  bool looseMatch(CamPair const &other) const {
    /* A loose match between two camera pairs is:
       (1) [A,-] matches [A,-], [A,B], [B,A], [-,A]
       (2) [-,A] ditto
       (3) [A,B] matches [A,-], [-,A], [A,B], [B,A], [B,-], [-,B]
       (4) [-,-] matches [-,-]
     */
    if (donor.isEmpty() && !acceptor.isEmpty())
      return CamPair(acceptor,donor).looseMatch(other);
    else if (other.donor.isEmpty() && !other.acceptor.isEmpty())
      return looseMatch(CamPair(other.acceptor, other.donor));
    else if (acceptor.isEmpty())
      return donor==other.donor || donor==other.acceptor;
    else if (other.acceptor.isEmpty())
      return donor==other.donor || acceptor==other.donor;
    else
      return *this==other || CamPair(acceptor,donor)==other;
  }
};

inline uint qHash(CamPair const &cp) {
  return qHash(cp.donor) + ~qHash(cp.acceptor);
}

inline QDebug &operator<<(QDebug &ts, CamPair const &cp) {
  QString x = cp.donor + ":" + cp.acceptor;
  return ts << x;
}

#endif
