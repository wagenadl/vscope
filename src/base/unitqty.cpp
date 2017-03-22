// base/unitqty.cpp - This file is part of VScope.
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

// UnitQty.C

#include "unitqty.h"
#include <base/numbers.h>
#include <math.h>
#include <base/dbg.h>
#include <base/exception.h>

UnitQty::UnitQty(QString v) {
  set(v);
}

UnitQty::UnitQty(double q, QString u) {
  set(q, u);
}

void UnitQty::set(QString v) {
  v = v.simplified();
  if (v.size()<1) {
    qty=0;
    uni="";
    return;
  }
  //Dbg() << "UnitQty: " << v;

  QString u;
  if (v.endsWith("Hz"))
    u = "Hz";
  else
    u = v.right(1);
  v = v.left(v.size() - u.size());

  QString p = v.right(1);
  QString prfs = "munpfazykMGTPEZY";
  if (prfs.contains(p)) {
    u = p + u;
    v = v.left(v.size() - 1);
  }
  v = v.simplified();

  //  Dbg() << "  u="<<u << " v="<<v;
  
  bool ok;
  double q = v.toDouble(&ok);
  if (ok) {
    set(q, u);
  } else {
    qty = Numbers::nan();
    uni="";
  }
}

void UnitQty::set(double q, QString u) {
  u = u.trimmed();
  if (u.size()>1) {
    QString p = u.left(1);
    QString prfs = "munpfazy";
    if (prfs.contains(p)) {
      u = u.mid(1);
      int idx = prfs.indexOf(p);
      while (idx>=0) {
	q/=1000;
	idx--;
      }
    }
    prfs = "kMGTPEZY";
    if (prfs.contains(p)) {
      u = u.mid(1);
      int idx = prfs.indexOf(p);
      while (idx>=0) {
	q*=1000;
	idx--;
      }
    }
  }
  uni = u;
  qty = q;
}

bool UnitQty::ok() const {
  return !Numbers::isNaN(qty);
}

QString UnitQty::unitSI() const {
  return uni;
}

QString UnitQty::unitEPhys() const {
  if (uni=="V")
    return "mV";
  else if (uni=="A")
    return "nA";
  else if (uni=="s")
    return "ms";
  else if (uni=="m")
    return "um";
  else
    return uni;
}

QString UnitQty::pretty(int prec) const {
  double q = qty;
  if (fabs(q)<1e-15)
    return "0 " + uni;

  if (uni=="%" || uni=="") 
    return QString("%1").arg(qty,0,'g',prec) + uni;

  QString prf = "";

  if (fabs(q)<1) {
    int shft=0;
    while (fabs(q) < 1) {
      q*=1000;
      shft+=1;
    }
    prf=QString(" munpfazy").mid(shft,1);
  } else if (fabs(q)>=1000) {
    int shft=0;
    while (fabs(q) >= 1000) {
      q/=1000;
      shft+=1;
    }
    prf=QString(" kMGTPEZY").mid(shft,1);
  }
  return QString("%1 ").arg(q,0,'g',3) + prf + uni;
}

double UnitQty::toDouble(QString u) const {
  if (!u.endsWith(uni)) 
    return Numbers::nan();

  if (u==uni)
    return qty;

  QString prf = u.left(1);

  QString prfs = "munpfazy";
  if (prfs.contains(prf)) {
    double q = qty;
    int idx = prfs.indexOf(prf);
    while (idx>=0) {
      q*=1000;
      idx--;
    }
    return q;
  }

  prfs = "kMGTPEZY";
  if (prfs.contains(prf)) {
    double q = qty;
    int idx = prfs.indexOf(prf);
    while (idx>=0) {
      q/=1000;
      idx--;
    }
    return q;
  }

  return Numbers::nan();
}

double UnitQty::str2num(QString qty, QString unit) {
  UnitQty x(qty);
  double y = x.toDouble(unit);
  if (Numbers::isNaN(y))
    throw Exception("UnitQty", "Cannot convert " + qty + " to " + unit);
  return y;
}
