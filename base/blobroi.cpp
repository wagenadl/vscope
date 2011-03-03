// blobroi.cpp

#include "blobroi.h"
#include <base/polyblob.h>
#include <base/minmax.h>
#include <base/numbers.h>
#include <base/memalloc.h>

BlobROI::~BlobROI() {
  delete [] weight;
}

BlobROI::BlobROI(PolyBlob const &src, Transform const &t, double border) {
  QPointF p = t(src.xy(0));
  double xmin = p.x();
  double xmax = p.x();
  double ymin = p.y();
  double ymax = p.y();
  for (int i=1; i<src.nPoints(); i++) {
    QPointF xy = t(src.xy(i));
    double x = xy.x();
    double y = xy.y();
    if (x<xmin)
      xmin=x;
    else if (x>xmax)
      xmax=x;
    if (y<ymin)
      ymin=y;
    else if (y>ymax)
      ymax=y;
  }
  x0 = floori(xmin - 2*border);
  w = ceili(xmax + 2*border) - x0;
  y0 = floori(ymin - 2*border);
  h = ceili(ymax + 2*border) - y0;

  weight = memalloc<double>(w*h, "BlobROI");
  sumw = 0;
  npix = 0;
  Transform tinv = t.inverse();
  for (int y_=0; y_<h; y_++) {
    int y = y_ + y0;
    for (int x_=0; x_<w; x_++) {
      int x = x_ + x0;
      QPointF p(x,y); p = tinv(p);
      double wei = border>0 ? src.weight(p,border) : src.inside(p);
      weight[x_+w*y_] = wei;
      sumw += wei;
      if (wei>=0.5)
	npix++;
    }
  }
}

BlobROI::Result::Result() {
  mean_=0;
  std_=0;
  npix_=0;
  min_=numbers.inf;
  max_=-numbers.inf;
}

int BlobROI::nPixels() const {
  return npix;
}

int BlobROI::bitmap(bool *dst, int dstSize) const {
  if (dstSize<npix)
    return 0;
  double *src = weight;
  for (int j=0; j<h; j++)
    for (int i=0; i<w; i++)
      *dst++ = *src++ >= 0.5;
  return w*h;
}

BlobROI::BlobROI(BlobROI const &other): BlobROI_(other) {
  weight = 0;
  copy(other);
}

void BlobROI::copy(BlobROI const &other) {
  if (other.weight) {
    weight = memalloc<double>(w*h,"BlobROI");
    memcpy(weight, other.weight, w*h*sizeof(double));
  }
}

BlobROI &BlobROI::operator=(BlobROI const &other) {
  if (weight)
    delete [] weight;
  *(BlobROI_*)this = other;
  weight = 0;
  copy(other);
  return *this;
}
