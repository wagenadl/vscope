// cvector.h

#ifndef CVECTOR_H

#define CVECTOR_H

#include <math/cplx.h>
#include <QVector>

typedef QVector<Cplx> cvector;
typedef QVector<double> rvector;

extern void real(cvector const &src, rvector &dst);
extern void imag(cvector const &src, rvector &dst);
extern void abs2(cvector const &src, rvector &dst);
extern void abs(cvector const &src, rvector &dst);
extern void phase(cvector const &src, rvector &dst);

extern void conj(cvector const &src, cvector &dst);
extern void conj(cvector &zz);

extern void copy(rvector const &src, cvector &dst);

extern void add(rvector const &zz1, rvector const &zz2, rvector &dst);
extern void add(rvector const &zz1, rvector &dst); // dst += zz1
extern void sub(rvector const &zz1, rvector const &zz2, rvector &dst);
extern void sub(rvector const &zz1, rvector &dst); // dst-=zz1
extern void mul(rvector const &zz1, rvector const &zz2, rvector &dst);
extern void mul(rvector const &zz1, rvector &dst); // dst*=zz1
extern void cmul(rvector const &zz1, double c, rvector &dst);
extern void cmul(rvector &zz1, double c);

extern void mul(rvector const &zz1, rvector const &zz2, cvector &dst);
extern void mulsqrt(rvector const &zz1, rvector const &zz2, rvector &dst); // dst=sqrt(zz1*zz2)
extern void mulsqrt(rvector const &zz1, rvector &zz2); // zz2 = sqrt(zz1*zz2).

extern void add(cvector const &zz1, cvector const &zz2, cvector &dst);
extern void add(cvector const &zz1, cvector &zz2); // zz2+=zz1.
extern void sub(cvector const &zz1, cvector const &zz2, cvector &dst);
extern void sub(cvector const &zz1, cvector &dst); // dst-=zz1
extern void mul(cvector const &zz1, cvector const &zz2, cvector &dst);
extern void mul(cvector const &zz1, cvector &zz2); // zz2 *= zz1.
extern void sum(cvector const &zz1, Cplx &dst);
extern void div(cvector const &zz1, rvector const &zz2, cvector &dst);
extern void div(cvector &zz1, rvector const &zz2); // zz1 /= zz2.
extern void cmul(cvector const &zz1, double c, cvector &dst);
extern void cmul(cvector &zz1, double c);

extern void mulconj(cvector const &zz1, cvector const &zz2_conj, cvector &dst);

extern void matchsize(rvector const &src, rvector &dst);
extern void matchsize(cvector const &src, rvector &dst);
extern void matchsize(cvector const &src, cvector &dst);
extern void matchsize(rvector const &src, cvector &dst);

#endif
