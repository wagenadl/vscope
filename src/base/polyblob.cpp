// polyblob.cpp

#include "polyblob.h"
#include <math.h>
#include <base/numbers.h>
#include <base/minmax.h>
#include <QString>
#include <QTextStream>
#include <QDomText>
#include <QDomDocument>
#include <QPainter>
#include <base/dbg.h>
#include <base/transform.h>

#define POLYBLOB_SCALE  50.0

PolyBlob::PolyBlob(int log2n): log2n(log2n) {
  n = 1<<log2n;
  //Dbg() << "PolyBlob constructor "<<this<<": n="<<n;
  msk = n-1;
  x50.resize(n);
  y50.resize(n);
  for (int i=0; i<n; i++)
    x50[i]=0;
  sumx=0;
  for (int i=0; i<n; i++)
    y50[i]=0;
  sumy=0;
}

PolyBlob::PolyBlob(PolyBlob const &other) {
  //Dbg() << "PolyBlob copy constructor" << this << " < " << &other;
  *this = other;
  //Dbg() << "n="<<n;
}

PolyBlob::~PolyBlob() {
  //Dbg() << "PolyBlob destructor" << this << "n="<<n;
}

PolyBlob &PolyBlob::operator=(PolyBlob const &other) {
  //Dbg() << "PolyBlob operator = " << this << " < " << &other << " n="<<other.n;
  log2n = other.log2n;
  n = other.n;
  msk = other.msk;
  x50 = other.x50;
  y50 = other.y50;
  sumx = other.sumx;
  sumy = other.sumy;

  lastx = other.lastx;
  lasty = other.lasty;
  lastr = other.lastr;
  lastphi = other.lastphi;
  lastk = other.lastk;

  return *this;
}

int PolyBlob::write(FILE *ofd) const {
  size_t k = fwrite(x50.constData(),2,n,ofd);
  if (int(k)<n)
    throw SysExc("PolyBlob","Cannot write x-coordinates","write");
  k = fwrite(y50.constData(),2,n,ofd);
  if (int(k)<n)
    throw SysExc("PolyBlob","Cannot write y-coordinates","write");
  return 2*2*n;
}

int PolyBlob::read(FILE *ifd) {
  size_t k = fread(x50.data(),2,n,ifd);
  if (int(k)<n)
    throw SysExc("PolyBlob","Cannot read x-coordinates","read");
  k = fread(y50.data(),2,n,ifd);
  if (int(k)<n)
    throw SysExc("PolyBlob","Cannot read y-coordinates","read");
  recalc_center();
  return 2*2*n;
}

double PolyBlob::x(int i) const {
  return x50[i&msk] / POLYBLOB_SCALE;
}

double PolyBlob::y(int i) const {
  return y50[i&msk] / POLYBLOB_SCALE;
}

double PolyBlob::x0() const {
  return sumx / POLYBLOB_SCALE / n;
}

double PolyBlob::y0() const {
  return sumy / POLYBLOB_SCALE / n;
}

void PolyBlob::recenter(double x0new, double y0new) {
  double x0old = x0();
  double y0old = y0();
  short dx = short(POLYBLOB_SCALE * (x0new - x0old));
  short dy = short(POLYBLOB_SCALE * (y0new - y0old));
  for (int i=0; i<n; i++)
    x50[i] += dx;
  for (int i=0; i<n; i++)
    y50[i] += dy;
  sumx+=n*dx;
  sumy+=n*dy;
}
  

void PolyBlob::set(int i, double x, double y) {
  i = i & msk;
  sumx -= x50[i];
  x50[i] = short(x * POLYBLOB_SCALE + 0.5);
  sumx += x50[i];
  sumy -= y50[i];
  y50[i] = short(y * POLYBLOB_SCALE + 0.5);
  sumy += y50[i];
}

class polyblob_rphi {
public:
  void set(double x, double y, double x0, double y0);
  double r;
  double phi;
  double x;
  double y;
};

void polyblob_rphi::set(double x_, double y_, double x0, double y0) {
  x = x_;
  y = y_;
  double dx = x-x0;
  double dy = y-y0;
  r = sqrt(dx*dx+dy*dy);
  phi = atan2(dy,dx);
  if (phi<0)
    phi += 2*Numbers::pi();
}

int polyblob_rphi_less(polyblob_rphi const &a, polyblob_rphi const &b) {
  return a.phi < b.phi;
}

void PolyBlob::build(QVector<double> const &xx, QVector<double> const &yy) {
  Q_ASSERT(xx.size()==yy.size());
  build(xx.size(), xx.constData(), yy.constData());
}

void PolyBlob::build(int k, double const *xx, double const *yy) {
  double x0=0;
  for (int j=0; j<k; j++)
    x0 += xx[j]/k;
  double y0=0;
  for (int j=0; j<k; j++)
    y0 += yy[j]/k;

  QVector<polyblob_rphi> src(k+1);
  for (int j=0; j<k; j++)
    src[j].set(xx[j], yy[j], x0, y0);
  qSort(src.begin(), src.end()-1, polyblob_rphi_less);
  src[k] = src[0];
  src[k].phi += 2*Numbers::pi();
  
  for (int j=0; j<k; j++) {
    double phi0 = src[j].phi;
    double phi1 = src[j+1].phi;
    int i0 = ceili(phi0*n/(2*Numbers::pi()));
    int i1 = ceili(phi1*n/(2*Numbers::pi()));
    for (int i=i0; i<i1; i++) {
      double phi = i*2*Numbers::pi()/n;
      double a = (phi-phi0) / (phi1-phi0+1e-20);
      double x = src[j].x*(1-a) + src[j+1].x*a;
      double y = src[j].y*(1-a) + src[j+1].y*a;
      x50[i&msk] = short(POLYBLOB_SCALE * x + .5);
      y50[i&msk] = short(POLYBLOB_SCALE * y + .5);
      //      fprintf(stderr,"  i=%i phi=%g x=%g y=%g -> %g,%g\n",i,phi,x,y,x50[i&msk]/POLYBLOB_SCALE,y50[i&msk]/POLYBLOB_SCALE);
    }
  }
  recalc_center();
}

void PolyBlob::recalc_center() {
  sumx = 0;
  for (int i=0; i<n; i++)
    sumx += x50[i];

  sumy = 0;
  for (int i=0; i<n; i++)
    sumy += y50[i];
}

void PolyBlob::revertex(int log2m) {
  if (log2m == log2n)
    return;
  
  int m = 1<<log2m;
  QVector<short> x50a(m,0);
  QVector<short> y50a(m,0);
  if (m>n) {
    int k = m/n;
    for (int i=0; i<n; i++) {
      for (int j=0; j<k; j++) {
      double a = j/double(k);
      x50a[i*k+j] = short(x50[i]*(1-a) + x50[(i+1)&msk]*a);
      y50a[i*k+j] = short(y50[i]*(1-a) + y50[(i+1)&msk]*a);
      }
    }
  } else {
    int k = n/m;
    for (int i=0; i<m; i++) {
      double x=0;
      double y=0;
      for (int j=0; j<k; j++) {
        x+=x50[i*k+j];
        y+=y50[i*k+j];
      }
      x50a[i] = short(x/k+.5);
      y50a[i] = short(y/k+.5);
    }
  }
  log2n = log2m;
  n = m;
  msk = n-1;
  //  delete [] x50;
  //  delete [] y50;
  x50 = x50a;
  y50 = y50a;
  recalc_center();
}

double PolyBlob::distToCenter(double x, double y) const {
  double dx = x-x0();
  double dy = y-y0();
  return sqrt(dx*dx+dy*dy);
}

double PolyBlob::distToEdge(double x, double y) const {
  double phi = atan2(y-y0(),x-x0());
  int k = int(phi*n / (2*Numbers::pi()) + 0.5) & msk;
  double dx = x - this->x(k);
  double dy = y - this->y(k);
  return sqrt(dx*dx+dy*dy);
}

bool PolyBlob::inside(double x, double y, double margin) const {
  double dr0 = distToCenter(x,y);
  double phi = atan2(y-y0(),x-x0());
  int k = int(phi*n / (2*Numbers::pi()) + 0.5) & msk;
  double dr1 = distToCenter(this->x(k), this->y(k));
  return dr0 <= dr1 + margin;
}

double PolyBlob::weight(double x, double y, double borderwidth) const {
  double dr0 = distToCenter(x,y);
  double phi = atan2(y-y0(),x-x0());
  int k = int(phi*n / (2*Numbers::pi()) + 0.5) & msk;
  double dr1 = distToCenter(this->x(k), this->y(k));
  return 1/(1+exp((dr0-dr1)/borderwidth));
}

void PolyBlob::adjust(double x, double y, bool first) {
  double dx = x-x0();
  double dy = y-y0();
  double r = sqrt(dx*dx+dy*dy);
  double phi = atan2(dy,dx);
  int k = floori(n*phi/(2*Numbers::pi())+.5);
  if (first) {
    set(k,x,y);
  } else {
    int dk = k-lastk;
    if (dk>n/2)
      lastk+=n;
    else if (dk<-n/2)
      lastk-=n;
    dk = k-lastk;
    if (dk==0) {
      set(k,x,y);
    } else if (dk>0) {
      for (int l=lastk+1; l<=k; l++) {
      double a = double(l-lastk)/double(k-lastk);
      set(l,(1-a)*lastx+a*x,(1-a)*lasty+a*y);
      }
    } else {
      // dk<0
      for (int l=lastk-1; l>=k; l--) {
        double a = double(lastk-l)/double(lastk-k);
        set(l,(1-a)*lastx+a*x,(1-a)*lasty+a*y);
      }
    }	
  }
  lastx = x;
  lasty = y;
  lastr = r;
  lastphi = phi;
  lastk = k;
}

void PolyBlob::reshape(double x1, double y1, double x2, double y2) {
  // This is the interesting part.
  // See matlab code in blobroi!

  double phi1 = atan2(y1-y0(),x1-x0());
  double phi2 = atan2(y2-y0(),x2-x0());

  //dbg("polyblob:reshape (%.0f,%.0f)-(%.0f,%.0f) phi=%.4f:%.4f",
  //    x1,y1,x2,y2,phi1*180/3.1415,phi2*180/3.1415);
  
  // should we work from phi1 to phi2, or the other way around?
  if (fmod(4*Numbers::pi() + phi2 - phi1, 2*Numbers::pi()) > Numbers::pi()) {
    /* above funky fmod returns a number between 0 and 2*pi */
    double phi = phi1;
    phi1 = phi2;
    phi2 = phi;
    double x = x1;
    x1 = x2;
    x2 = x;
    double y = y1;
    y1 = y2;
    y2 = y;
  }
  // We're working from phi1 to phi2.
  int k1 = ceili(phi1*n/(2*Numbers::pi()));
  int k2 = ceili(phi2*n/(2*Numbers::pi()));
  // Must include both k1 and k2.
  // Make sure the range is >=0 but not >=2*pi:
  // dbg("  -> (%.0f,%.0f)-(%.0f,%.0f) phi=%.4f:%.4f k=%i:%i",
  //     x1,y1,x2,y2,phi1*180/3.1415,phi2*180/3.1415,k1,k2);
  while (k2>k1)
    k2-=n;
  while (k2<k1)
    k2+=n;
  if (k2==k1)
    k2++;
  double dphi = fmod(4*Numbers::pi()+phi2-phi1, 2*Numbers::pi());
  // dbg("    -> k=%i:%i dphi=%.4f",k1,k2,dphi*180/3.1415);
  for (int k=k1; k<k2; k++) {
    double phi = k*2*Numbers::pi()/n;
    double a = fmod(4*Numbers::pi()+phi-phi1,2*Numbers::pi()) / dphi;
    if (a<0)
      a=0;
    else if (a>1)
      a=1;
    double x = x1*(1-a) + x2*a;
    double y = y1*(1-a) + y2*a;
    set(k,x,y);
  }
}

double PolyBlob::binaryScale() const {
  return POLYBLOB_SCALE;
}

int PolyBlob::binaryTypeSize() const {
  return sizeof(short);
}

int PolyBlob::nPoints() const {
  return n;
}

int PolyBlob::log2nPoints() const {
  return log2n;
}


void PolyBlob::write(QDomElement doc) const {
  if (doc.hasChildNodes()) 
    for (QDomNode e=doc.firstChild(); !e.isNull(); e=e.nextSibling())
      e.clear();

  QString s;
  for (int i=0; i<n; i++) 
    s += QString("%1 %2\n").arg(x(i),0,'f',2).arg(y(i),0,'f',2);

  QDomDocument owner = doc.ownerDocument();
  QDomText t = owner.createTextNode(s);
  doc.appendChild(t);
  doc.setAttribute("n",QString("%1").arg(n));
}

PolyBlob::PolyBlob(QDomElement doc) {
  n=0;
  read(doc);
}

void PolyBlob::read(QDomElement doc) {
  QString newns = doc.attribute("n");
  if (newns=="")
    throw Exception("PolyBlob","<roi> element does not define vertex count","read");
  bool ok;
  int newn = newns.toInt(&ok);
  if (!ok)
    throw Exception("PolyBlob","<roi> element does not define vertex count","read");
  if (newn!=n) {
    int newlogn=0;
    while ((1<<newlogn) < newn)
      newlogn++;
    if (newlogn>newn)
      throw Exception("PolyBlob","Vertex count must be power of two","read");
    if (n) {
      // delete [] x50;
      // delete [] y50;
    }
    log2n = newlogn;
    n = newn;
    msk = n-1;
    x50.resize(n);
    y50.resize(n);
  }
  QDomText t = doc.firstChild().toText();
  if (t.isNull())
    throw Exception("PolyBlob","<roi> element is empty","read");
  QString s = t.data();
  QTextStream ts(&s,QIODevice::ReadOnly);
  for (int i=0; i<n; i++) {
    double x, y;
    ts >> x;
    ts >> y;
    x50[i] = short(x*POLYBLOB_SCALE);
    y50[i] = short(y*POLYBLOB_SCALE);
  }
  recalc_center();
}

void PolyBlob::paint(QPainter *pntr) const {
  QPolygonF poly(n);
  for (int i=0; i<n; i++)
    poly[i]=QPointF(x50[i]/POLYBLOB_SCALE, y50[i]/POLYBLOB_SCALE);
  pntr -> drawPolygon(poly);
}

void PolyBlob::paint(QPainter *pntr, Transform const &t) const {
  QPolygonF poly(n);
  for (int i=0; i<n; i++)
    poly[i]=t(QPointF(x50[i]/POLYBLOB_SCALE, y50[i]/POLYBLOB_SCALE));
  pntr -> drawPolygon(poly);
}

double PolyBlob::greatestRadius() const {
  double r250=0;
  double x050 = double(sumx)/n;
  double y050 = double(sumy)/n;
  for (int i=0; i<n; i++) {
    double dx50 = x50[i]-x050;
    double dy50 = y50[i]-y050;
    double r250i = dx50*dx50 + dy50*dy50;
    if (r250i > r250)
      r250 = r250i;
  }
  return sqrt(r250) / POLYBLOB_SCALE;
}

void PolyBlob::set(int i, QPointF xy) {
  set(i, xy.x(), xy.y());
}

void PolyBlob::adjust(QPointF xy, bool first) {
  adjust(xy.x(), xy.y(), first);
}

void PolyBlob::reshape(QPointF xy1, QPointF xy2) {
  reshape(xy1.x(), xy1.y(), xy2.x(), xy2.y());
}

void PolyBlob::reshape(QLineF xy12) {
  reshape(xy12.p1(), xy12.p2());
}

void PolyBlob::recenter(QPointF p) {
  recenter(p.x(), p.y());
}

double PolyBlob::distToCenter(QPointF xy) const {
  return distToCenter(xy.x(), xy.y());
}

double PolyBlob::distToEdge(QPointF xy) const {
  return distToEdge(xy.x(), xy.y());
}

bool PolyBlob::inside(QPointF xy, double margin) const {
  return inside(xy.x(), xy.y(), margin);
}

double PolyBlob::weight(QPointF xy, double margin) const {
  return weight(xy.x(), xy.y(), margin);
}
