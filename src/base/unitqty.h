// base/unitqty.h - This file is part of VScope.
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

// UnitQty.H

#ifndef UNITQTY_H

#define UNITQTY_H

#include <QString>

class UnitQty {
public:
  /*:C UnitQty
   *:D Represents a quantity with a unit.
    :  The following units are understood: V, A, s, Hz, m, %.
    :  The following prefixes are understood: f, p, u, m, k, M, G, T
    :  We do not presently support arithmetic.
   */
  UnitQty(QString); // arg can be, e.g., "17 mV"
  UnitQty(double qty, QString unit);
  QString unitSI() const; // returns SI unit
  QString unitEPhys() const; // returns mV, nA, ms, um, or SI unit.
  QString pretty(int precision=3) const; // returns number + pretty unit
  double toDouble(QString unit) const; // returns number in terms of given unit
                                       // or NaN if bad conversion.
  void set(QString); // arg can be, e.g., "17 mV".
  void set(double qty, QString unit);
  bool ok() const;
  static double str2num(QString qty, QString unit);
  // str2num("17 mV", "V") returns 0.017, or exception if unable.
private:
  double qty; // always in SI units
  QString uni; // always an SI unit, or '%'.
};

#endif
