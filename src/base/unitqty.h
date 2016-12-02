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
