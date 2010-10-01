// blobroi.cpp

#include "blobroi.h"
#include <base/polyblob.h>
#include <base/minmax.h>
#include <base/numbers.h>

BlobROI::~BlobROI() {
  delete [] weight;
}

BlobROI::BlobROI(PolyBlob const &src, double border) {
  double xmin = src.x(0);
  double xmax = src.x(0);
  double ymin = src.y(0);
  double ymax = src.y(0);
  for (int i=1; i<src.nPoints(); i++) {
    double x = src.x(i);
    if (x<xmin)
      xmin=x;
    else if (x>xmax)
      xmax=x;
    double y = src.y(i);
    if (y<ymin)
      ymin=y;
    else if (y>ymax)
      ymax=y;
  }
  x0 = floori(xmin - 2*border);
  w = ceili(xmax + 2*border) - x0;
  y0 = floori(ymin - 2*border);
  h = ceili(ymax + 2*border) - y0;

  weight = new double[w*h];
  sumw = 0;
  npix = 0;
  for (int y_=0; y_<h; y_++) {
    int y = y_ + y0;
    for (int x_=0; x_<w; x_++) {
      int x = x_ + x0;
      double wei = border>0 ? src.weight(x,y,border) : src.inside(x,y);
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

int BlobROI::extract(uint16_t *dst, int dstSize,
		     uint16_t const *data, int X, int Y,
		     bool flipY) const {
  if (dstSize<npix)
    return 0;
  int x1 = x0 + w;
  if (x1>X)
    x1 = X;
  int y1 = y0 + h;
  if (y1>Y)
    y1 = Y;
  BlobROI::Result r;
  for (int y=y0; y<y1; y++) {
    double *wp = weight + (y-y0)*w;
    uint16_t const *dp = data + (flipY ? (Y-1-y):y) * X;
    for (int x=x0; x<x1; x++) {
      double wei = wp[x-x0];
      if (wei>=0.5)
	*dst++ = dp[x-x0];
    }
  }
  return npix;
}
  
BlobROI::Result BlobROI::study(uint16_t const *data, int X, int Y,
			       bool flipY) const {
  int x1 = x0 + w;
  if (x1>X)
    x1 = X;
  int y1 = y0 + h;
  if (y1>Y)
    y1 = Y;
  BlobROI::Result r;
  double sum1=0, sumd=0, sumdd=0;
  double min=65535;
  double max=0;
  for (int y=y0; y<y1; y++) {
    double *wp = weight + (y-y0)*w;
    uint16_t const *dp = data + (flipY ? (Y-1-y):y) * X;
    for (int x=x0; x<x1; x++) {
      double wei = wp[x-x0];
      double d = dp[x];
      sum1+=wei;
      sumd+=wei*d;
      sumdd+=wei*d*d;
      if (wei>=0.5) {
	if (d>max)
	  max=d;
	if (d<min)
	  min=d;
      }
    }
  }
  r.mean_ = sumd/sum1;
  r.std_ = sqrt(sumdd/(sum1-1)-sumd*sumd/sum1/(sum1-1));
  r.npix_ = sum1;
  r.min_ = min;
  r.max_ = max;

  return r;
}

BlobROI::BlobROI(BlobROI const &other): BlobROI_(other) {
  copy(other);
}

void BlobROI::copy(BlobROI const &other) {
  if (weight) {
    weight = new double[w*h];
    memcpy(weight, other.weight, w*h*sizeof(double));
  }
}

BlobROI &BlobROI::operator=(BlobROI const &other) {
  if (weight)
    delete [] weight;
  *(BlobROI_*)this = other;
  copy(other);
  return *this;
}
