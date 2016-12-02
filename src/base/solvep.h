// solvep.h

#ifndef SOLVEP_H

#define SOLVEP_H

#include <QList>
#include <base/cplx.h>

extern QList<cplx> solvep2(cplx A, cplx B, cplx C);
extern QList<cplx> solvep4(cplx A, cplx B, cplx C, cplx D, cplx E);
extern QList<double> solvep2(double A, double B, double C);
extern QList<double> solvep4(double A, double B, double C, double D, double E);

/* These solve quadratic and quartic polynomial equations.
   The <double> versions attempt to return the real solutions. This
   is implemented in a slightly unsatisfying way: if the real part of
   a solution is greater in magnitude than the imaginary part, the
   solution is accepted and its real part is returned. The proper test,
   checking that the imaginary part equals zero, is not practical for
   solvep4, because it performs thousands of multiplications and is not
   terribly precise as a result. The heuristic works fine for solvep2,
   because quadratic equations with real coefficients have either real
   or strictly imaginary solutions.
*/

#endif
