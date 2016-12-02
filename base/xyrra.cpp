// xyrra.cpp - elliptical ROI

#include <base/xyrra.h>
#include <base/minmax.h>
#include <base/exception.h>
#include <base/numbers.h>
#include <base/solvep.h>
#include <base/dbg.h>
#include <base/transform.h>
#include <base/xyabc.h>

#include <QPainter>
#include <QPolygonF>

inline double sq(double x) { return x*x; }

// Following rsin, arsin, ratan2 serve to convert from Qt's left-handed
// coordinate system to Matlab's right-handed coordinate system so that
// translation of ellipse routines is more straightforward.
inline double rsin(double a) { return -sin(a); }
inline double arsin(double y) { return asin(-y); }
inline double ratan2(double y, double x) { return atan2(-y,x); }
//inline double rsin(double a) { return sin(a); }
//inline double arsin(double y) { return asin(y); }
//inline double ratan2(double y, double x) { return atan2(y,x); }

XYRRA::XYRRA(QDomElement doc) {
  read(doc);
}

XYRRA::XYRRA(QPointF xy0, double R, double r, double a): R(R), r(r), a(a) {
  x0 = xy0.x();
  y0 = xy0.y();
  if (r<=0)
    r=R;
}

void XYRRA::read(QDomElement doc) {
  bool ok;
  x0 = doc.attribute("x0").toDouble(&ok);
  if (!ok)
    throw Exception("XYRRA","Cannot read x0");
  
  y0 = doc.attribute("y0").toDouble(&ok);
  if (!ok)
    throw Exception("XYRRA","Cannot read y0");
  
  R = doc.attribute("R").toDouble(&ok);
  if (!ok)
    throw Exception("XYRRA","Cannot read R");
  
  r = doc.attribute("r").toDouble(&ok);
  if (!ok)
    throw Exception("XYRRA","Cannot read r");
  
  a = doc.attribute("a").toDouble(&ok);
  if (!ok)
    throw Exception("XYRRA","Cannot read a");
}

void XYRRA::write(QDomElement doc) const {
  doc.setAttribute("x0",QString::number(x0));
  doc.setAttribute("y0",QString::number(y0));
  doc.setAttribute("R",QString::number(R));
  doc.setAttribute("r",QString::number(r));
  doc.setAttribute("a",QString::number(a));
}

template <class DATATYPE> void XYRRA::fill(DATATYPE *data, int X, int Y, 
					   DATATYPE value) const {
  // Translated from elfill.m
  
  double Rm = max(R,r);
  int X0 = int(max(floor(x0-Rm),0));
  int X1 = int(min(ceil(x0+Rm),X));
  int Y0 = int(max(floor(y0-Rm),0));
  int Y1 = int(min(ceil(y0+Rm),Y));

  double cs = cos(a);
  double sn = rsin(a);
  double invR_2 = 1/(R*R);
  double invr_2 = 1/(r*r);

  for (int y=Y0; y<Y1; y++) {
    double csy = cs*(y-y0);
    double sny = sn*(y-y0);
    for (int x=X0; x<X1; x++) {
      double csx = cs*(x-x0);
      double snx = sn*(x-x0);
      double xi = csx + sny;
      double eta = -snx + csy;
      if (xi*xi*invR_2 + eta*eta*invr_2 <= 1)
	data[x+X*y] = value;
    }
  }
}

// Does the following force template instantiation?
void xyrra_force_instantiate_do_not_call() {
  XYRRA xy;
  xy.fill((short*)0,0,0,short(0));
  xy.fill((char*)0,0,0,char(0));
  xy.fill((unsigned short*)0,0,0,(unsigned short)(0));
  xy.fill((unsigned char*)0,0,0,(unsigned char)(0));
}


double XYRRA::maxY() const {
  // translated from elmaxy.m
  double om1 = ratan2(r*cos(a), R*rsin(a));
  double om2 = om1+Numbers::pi();
  double y1 = y0 + R*rsin(a)*cos(om1) + r*cos(a)*rsin(om1);
  double y2 = y0 + R*rsin(a)*cos(om2) + r*cos(a)*rsin(om2);
  return fmax(y1,y2);
}

double XYRRA::maxX() const {
  XYRRA alt;
  alt.x0 = y0;
  alt.y0 = x0;
  alt.R = R;
  alt.r = r;
  alt.a = a+Numbers::pi()/2;
  return alt.maxY();
}

QRectF XYRRA::bbox() const {
  double ymax = maxY();
  double xmax = maxX();
  double ymin = y0-(ymax-y0);
  double xmin = x0-(xmax-x0);
  return QRectF(QPointF(xmin,ymin),QPointF(xmax,ymax));
}

double XYRRA::parallel(double omega) const {
  // translated from elparallel.m
  double dxi = -R*rsin(omega);
  double deta = r*cos(omega);
  return ratan2(deta,dxi) + a;
}

bool XYRRA::inside(double x, double y, double marg) const {
  double omega = findNearest(x,y);
  double x1 = omegaToX(omega);
  double y1 = omegaToY(omega);
  double rr = sq(x-x0)+sq(y-y0);
  double rr1 = sq(x1-x0)+sq(y1-y0);
  return sqrt(rr) <= sqrt(rr1) + marg;
}

double XYRRA::omegaToX(double omega) const {
  double xi = R*cos(omega);
  double eta = r*rsin(omega);
  return x0 + cos(a)*xi - rsin(a)*eta;
}

double XYRRA::omegaToY(double omega) const {
  double xi = R*cos(omega);
  double eta = r*rsin(omega);
  return y0 + rsin(a)*xi + cos(a)*eta;
}


double XYRRA::findNearest(QPointF const &xy) const {
  return findNearest(xy.x(), xy.y());
}

double XYRRA::findNearest(double x, double y) const {
  if (r==0 || R==0)
    return 0;

  // translated from elfind_nearest.m
  double X = x-x0;
  double Y = y-y0;
  double xi  =  cos(a)*X + rsin(a)*Y;
  double eta = -rsin(a)*X + cos(a)*Y;

  double dR = R*R - r*r;

  // printf("dR=%g; xi=%g; eta=%g;\n",dR,xi,eta);
  
  QList<double> rsinomega = (dR==0)
    ? solvep2(r*r*eta*eta + R*R*xi*xi,
	      0,
	      -r*r*eta*eta)
    : solvep4(dR*dR,
	      2*dR*r*eta,
	      r*r*eta*eta + R*R*xi*xi - dR*dR,
	      -2*dR*r*eta,
	      -r*r*eta*eta);

  // printf("rsinomaga = [");
  // for (QList<double>::iterator i=rsinomega.begin(); i!=rsinomega.end(); ++i) {
  //   printf("%g ",*i);
  // }
  // printf("];\n");
  QList<double> omeg;
  // printf("omeg = [");
  for (QList<double>::iterator i=rsinomega.begin(); i!=rsinomega.end(); ++i) {
    double om = arsin(*i);
    omeg.push_back(om);
    omeg.push_back(Numbers::pi()-om);
    // printf("%g %g ",om,numbers.pi-om);
  }
  // printf("];\n");
  double dd = Numbers::inf();
  double om=0;
  // printf("xy1 = [");
  for (QList<double>::iterator i=omeg.begin(); i!=omeg.end(); ++i) {
    double xi = R*cos(*i);
    double eta = r*rsin(*i);
    double x1 = x0 + cos(a)*xi - rsin(a)*eta;
    double y1 = y0 + rsin(a)*xi + cos(a)*eta;
    double d = (x-x1)*(x-x1) + (y-y1)*(y-y1);
    if (d<dd) {
      om=*i;
      dd=d;
    }
    // printf("%g %g; ",x1,y1);
  }
  // printf("];\n");
  return om;
}

void XYRRA::paint(QPainter *pntr, int nPoints) const {
  //dbg("XYRRA::paint");
  QPolygonF poly(nPoints);
  
  // Following translated from elplot_xyrra.m
  double cs = cos(a);
  double sn = rsin(a);
  for (int i=0; i<nPoints; i++) {
    double omega = i*2*Numbers::pi()/nPoints;
    double xi = R*cos(omega);
    double eta = r*rsin(omega);
    double x = x0 + cs*xi - sn*eta;
    double y = y0 + sn*xi + cs*eta;
    poly[i]=QPointF(x,y);
  }
  // End of translated bit
  
  pntr->drawConvexPolygon(poly);
}

bool XYRRA::operator==(XYRRA const &other) {
  return x0==other.x0 && y0==other.y0
    && R==other.R && r==other.r
    && a==other.a;
}

XYRRA &XYRRA::transform(Transform const &t) {
  x0 = t.mapx(x0);
  y0 = t.mapy(y0);
  XYABC abc(*this);
  double ax = t.mapdx(1);
  double ay = t.mapdy(1);
  abc.a /= ax*ax;
  abc.b /= ay*ay;
  abc.c /= ax*ay;
  *this = abc.toXYRRA();  
  return *this;
}

XYRRA XYRRA::transformed(Transform const &t) const {
  XYRRA x = *this;
  x.transform(t);
  return x;
}
