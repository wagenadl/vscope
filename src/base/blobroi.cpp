// blobroi.cpp

#include "blobroi.h"
#include <base/polyblob.h>
#include <base/minmax.h>
#include <base/numbers.h>
#include <base/dbg.h>

BlobROI::~BlobROI() {
}

BlobROI::BlobROI(PolyBlob const &src, Transform const &t, double border) {
  construct(src, t, QRect(), border);
}

BlobROI::BlobROI(PolyBlob const &src, Transform const &t,
		 QRect bbox, double border) {
  construct(src, t, bbox, border);
}

void BlobROI::construct(PolyBlob const &src, Transform const &t,
			QRect bbox, double border) {
  //  Dbg() << "BlobROI::construct " << this;
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
  //Dbg() << "BlobROI: x0="<<x0<< " w="<<w<< " y0="<<y0<< " h="<<h;
  //Dbg() << "  bbox.l=" << bbox.left() << " .r="<<bbox.right()
  //	<< " .t=" << bbox.top() << " .b="<<bbox.bottom();
  if (bbox.isValid()) {
    if (x0<bbox.left())
      x0 = bbox.left();
    else if (x0>bbox.right())
      x0 = bbox.right();
    if (y0<bbox.top())
      y0 = bbox.top();
    else if (y0>bbox.bottom())
      y0 = bbox.bottom();
    if (x0+w>bbox.right())
      w = bbox.right()+1-x0;
    if (y0+h>bbox.bottom())
      h = bbox.bottom()+1-y0;
  }

  if (w<=0 || h<=0) 
    w=h=1;
    
  
  weight.resize(w*h);
  sumw = 0;
  npix = 0;
  Transform tinv = t.inverse();
  for (int y_=0; y_<h; y_++) {
    int y = y_ + y0;
    for (int x_=0; x_<w; x_++) {
      int x = x_ + x0;
      QPointF p = tinv(Transform::pixelCenter(x,y));
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
  min_=Numbers::inf();
  max_=-Numbers::inf();
}

int BlobROI::nPixels() const {
  return npix;
}

QVector<bool> BlobROI::bitmap() const {
  QVector<bool> dst(w*h);
  Q_ASSERT(bitmap(dst.data(), w*h));
  //  Dbg() << "BlobROI::bitmap" << this;
  return dst;
}

int BlobROI::bitmap(bool *dst, int dstSize) const {
  if (dstSize<w*h)
    return 0;
  double const *src = weight.constData();
  Q_ASSERT(weight.size()==w*h);
  for (int j=0; j<h; j++)
    for (int i=0; i<w; i++)
      *dst++ = *src++ >= 0.5;
  return w*h;
}
