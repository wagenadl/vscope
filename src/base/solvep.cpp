// solvep.cpp

#include "solvep.h"
#include <math.h>

//inline uint qHash(cplx const &a) {
//  return qHash(QByteArray((char const *)&a,sizeof(a)));
//}

QList<cplx> solvep2(cplx A, cplx B, cplx C) {
  cplx x1 = (0.5/A)*(-B+sqrt(B*B-4*C*A));
  cplx x2 = (0.5/A)*(-B-sqrt(B*B-4*C*A));
  QList<cplx> r;
  r.push_back(x1);
  r.push_back(x2);
  return r;
}

QList<double> solvep2(double A, double B, double C) {
  // returns the two solutions to A x^2 + Bx + C = 0
  QList<cplx> r0 = solvep2(cplx(A),cplx(B), cplx(C));
  QList<double> r;
  for (QList<cplx>::iterator i=r0.begin(); i!=r0.end(); ++i) {
    if (fabs((*i).im)<fabs((*i).re))
      r.push_back((*i).re);
  }
  return r;
}

QList<cplx> solvep4(cplx A, cplx B, cplx C, cplx D, cplx E) {
#include "solvep_m2c.cpp"
  QList<cplx> r;
  r.push_back(x1);
  r.push_back(x2);
  r.push_back(x3);
  r.push_back(x4);
  return r;
}

QList<double> solvep4(double A, double B, double C, double D, double E) {
  QList<cplx> r0 = solvep4(cplx(A),cplx(B), cplx(C),cplx(D),cplx(E));
  QList<double> r;
  for (QList<cplx>::iterator i=r0.begin(); i!=r0.end(); ++i) {
    if (fabs((*i).im)<fabs((*i).re))
      r.push_back((*i).re);
  }
  return r;
}

