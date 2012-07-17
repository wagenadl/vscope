// roidata.cpp

#include "roidata.h"
#include <base/ccddata.h>
#include <base/numbers.h>
#include <base/minmax.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <base/blobroi.h>
#include <base/memalloc.h>
#include <base/range.h>

inline double sq(double x) { return x*x; }

inline int rangelimit(int x, int min, int max) {
  if (x<min)
    x=min;
  else if (x>max)
    x=max;
  return x;
}

ROIData::ROIData() {
  source = 0;
  debleach = "None";
}

ROIData::~ROIData() {
  // real destruction done by caches.
}

void ROIData::setDebleach(QString d) {
  if (d!=debleach) {
    debleach = d;
    debleached.invalidate();
  }
}

void ROIData::setData(CCDData const *src) {
  if (src==0)
    bitmap.unsetTransformAndClip();
  else
    bitmap.setTransformAndClip(src->dataToCanvas(),
			       QRect(0,0, src->getSerPix(), src->getParPix()));
  source = src;
  updateData();
}

void ROIData::updateData() {
  raw.invalidate();
  debleached.invalidate();
}


void ROIData::setROI(ROICoords const *roi) {
  bitmap.setROI(roi);
  raw.invalidate();
  debleached.invalidate();
}

bool ROIData::haveData() const {
  return (source && source->getNFrames()) ? true : false;
}

int ROIData::getNFrames() const {
  return source ? source->getNFrames() : 0;
}

double ROIData::getT0ms() const {
  return source ? source->getT0ms() : 0;
}

double ROIData::getDTms() const {
  return source ? source->getDTms() : 0;
}

Range ROIData::timeRange() const {
  if (source) 
    return Range(getT0ms(), getT0ms() + getDTms()*getNFrames());
  else
    return Range();
}

bool ROIData::ensureBitmap() const {
  /* The bitmap holds which pixels are inside the ROI and which are not.
     To avoid wasted effort, we only calculate inside a rectangular
     bounding box.
     It would not be too terribly hard to modify this function to produce
     a soft-edged ROI, but that's a luxury for later.
  */
  return bitmap.validate();
}

//////////////////////////////////////////////////////////////////////
ROIData::DataCache::DataCache() {
  dat = 0;
  len = 0;
}

ROIData::DataCache::~DataCache() {
  if (dat)
    delete [] dat;
}

void ROIData::DataCache::resize(int len_) {
  if (len_==len)
    return;

  if (dat)
    delete [] dat;
  dat = 0;
  
  len = len_;
  if (len>0) 
    dat = memalloc<double>(len, "ROIData");
  valid = len==0;
}

//////////////////////////////////////////////////////////////////////
ROIData::BitmapCache::BitmapCache() {
  bm = 0;
  roi = 0;
  blobROI = 0;
  haveTransformAndClip = 0;
}

ROIData::BitmapCache::~BitmapCache() {
  if (bm)
    delete bm;
  if (blobROI)
    delete blobROI;
}

void ROIData::BitmapCache::setROI(ROICoords const *roi_) {
  roi = roi_;
  valid = false;
}

void ROIData::BitmapCache::unsetTransformAndClip() {
  haveTransformAndClip = false;
  valid = false;
}

void ROIData::BitmapCache::setTransformAndClip(Transform const &t_,
					       QRect const &bb) {
  if (haveTransformAndClip && t==t_ && clip==bb)
    return;
  t = t_;
  clip = bb;
  haveTransformAndClip = true;
  valid = false;
}

void ROIData::BitmapCache::validator() {
  if (roi && roi->isBlob())
    makePolyBitmap(roi->blob());
  else if (roi && roi->isXYRRA())
    makeXYRRABitmap(roi->xyrra());
}

void ROIData::BitmapCache::makePolyBitmap(PolyBlob const &blob) {
  if (!haveTransformAndClip)
    return;
  if (blobROI)
    delete blobROI;
  blobROI = 0;
  blobROI = new BlobROI(blob, t.inverse(), clip);
  int xl = blobROI->bitmapX0();
  int yt = blobROI->bitmapY0();
  int w1 = blobROI->bitmapW();
  int h1 = blobROI->bitmapH();
  npix = blobROI->nPixels();

  if (bm && rec.width()*rec.height() != w1*h1) {
    delete bm;
    bm = 0;
  }
  rec = QRect(xl, yt, w1, h1);
  int rectpix = w1*h1;
  if (!bm)
    bm = memalloc<bool>(rectpix, "ROIData");
  int r = blobROI->bitmap(bm, rectpix);
  if (r!=rectpix)
    throw Exception("ROIData",
		    QString("Unexpected bm size: %1 instead of %2*%3")
		    .arg(r).arg(w1).arg(h1));
  valid = true;
}      

void ROIData::BitmapCache::makeXYRRABitmap(XYRRA const &roi) {
  if (!haveTransformAndClip)
    return;
  XYRRA xyrra = roi;
  xyrra.transform(t.inverse());
  QRectF bb = xyrra.bbox();
  int xl = rangelimit(floori(bb.left()),clip.left(),clip.right()+1);
  int xr = rangelimit(ceili(bb.right()),clip.left(),clip.right()+1);
  int yt = rangelimit(floori(bb.top()),clip.top(),clip.bottom()+1);
  int yb = rangelimit(ceili(bb.bottom()),clip.top(),clip.bottom()+1);
  int w1 = xr-xl;
  int h1 = yb-yt;
  if (bm && rec.width()*rec.height() != w1*h1) {
    delete bm;
    bm = 0;
  }
  rec = QRect(xl, yt, w1, h1);
  int rectpix = rec.width()*rec.height();
  if (!bm) 
    bm = memalloc<bool>(rectpix, "ROIData");
  bool *ptr = bm;
  double cs = cos(xyrra.a);
  double sn = sin(xyrra.a);
  npix = 0;
  for (int y=yt; y<yb; y++) {
    double dy = y+0.5 - xyrra.y0;
    for (int x=xl; x<xr; x++) {
      double dx = x+0.5 - xyrra.x0;
      double xi = dx*cs + dy*sn;
      double eta = -dx*sn + dy*cs;
      bool isInside = sq(xi)/sq(xyrra.R) + sq(eta)/sq(xyrra.r) < 1;
      *ptr++ = isInside;
      if (isInside)
	npix++;
    }
  }
  valid = true;
}


double const *ROIData::getRaw() const {
  if (raw.isvalid())
    return raw.data();

  if (!source)
    return 0;

  int N = getNFrames();  
  raw.resize(N);
  if (N==0)
    return 0;

  if (!ensureBitmap())
    return 0;

  QRect rect = bitmap.rect();
  int h = rect.height();
  int w = rect.width();
  int ymul = source->getSerPix();
  double sum1 = bitmap.npixels();
    double *data = raw.data();
  for (int n=0; n<N; n++) {
    double sum=0;
    uint16_t const *srcfrm = source->frameData(n)
      + rect.left()
      + rect.top()*ymul;
    bool const *bitptr = bitmap.bitmap();
    for (int y=0; y<h; y++) {
      uint16_t const *srcrow = srcfrm + y*ymul;
      for (int x=0; x<w; x++) {
	if (*bitptr++)
	  sum+=*srcrow;
	srcrow++;
      }
    }
    data[n] = (sum1>0) ? sum/sum1 : 0;
  }

  //  Dbg() << "ROIData: returning dataRaw=" << data;

  raw.validate();
  return data;
}

double const *ROIData::getDebleachedDFF() const {
  if (debleached.isvalid())
    return debleached.data();

  if (!getRaw())
    return 0;

  int N = getNFrames();
  debleached.resize(N);

  double *dst = debleached.data();
  double const *src = raw.data();
  if (debleach == "None") {
    for (int n=0; n<N; n++)
      dst[n] = src[n];
  } else if (debleach == "Linear") {
    /* I will fit the data y to A*(t-t0)+B, where t0 is the half-length
       of the data, then subtract A*(t-t0).
       Start with: chi2 = sum_t (A*(t-t0)+B - y(t))^2,
       and demand dchi2/dA = 0 and dchi2/dB = 0. That is:
         sum_t (t-t0) (A*(t-t0) + B - y(t)) = 0
       and
         sum_t (A*(t-t0) + B - y(t)) = 0.
       Take constants out of the sum:
         A*sum_t (t-t0)^2 + B*sum_t(t-t0) = sum_t (t-t0)*y(t)
	 A*sum_t (t-t0)   + B*sum_t       = sum_t y(t)
       All the sums can simply be calculated ahead of time. Let's
       write this as:
         A*sTT + B*sT = sTY
	 A*sT + B*s1 = sY
       Let's eliminate B:
         A*(sTT*s1 - sT*sT) = sTY*s1 - sY*sT.
       Note that sT=0, so this simplifies to:
         A*sTT*s1 = sTY*s1,
       or:
         A=sTY/sTT.

       Let's ignore first and last point; they are often messed up.
    */
    double t0 = double(N-1)/2;
    double sY=0;
    for (int n=1; n<N-1; n++)
      sY+=src[n];
    double y0 = sY/(N-2);

    double sTT=0, sTY=0;
    for (int n=1; n<N-1; n++) {
      sTT+=sq(n-t0);
      sTY+=(n-t0)*(src[n]-y0);
    }
    double A = sTY/sTT;
    for (int n=0; n<N; n++)
      dst[n] = dst[n] - A*(n-t0);
    // dbg("sTT=%g sTY=%g A=%g",sTT,sTY,A);
  } else if (debleach == "Quadratic") {
    /* I will fit the data y to A*(t-t0)^2 + B*(t-t0) + C, where
       t0 is the half-length of the data, then subtract
       A*(t-t0)^2 + B*(t-t0).
       Start with: chi2 = sum_t (A*(t-t0)^2 + B*(t-t0) + B - y(t))^2,
       and demand chi2/dA = dchi2/dB = dchi2/dC = 0. That is:
         sum_t (t-t0)^2/2 * (A*(t-t0)^2 + B*(t-t0) + C - y(t)) = 0,
         sum_t (t-t0) * (A*(t-t0)^2 + B*(t-t0) + C - y(t)) = 0,
       and
         sum_t (A*(t-t0)^2 + B*(t-t0) + C - y(t)) = 0.
       Take csts out of the sums:
         A * sum_t (t-t0)^4 + B * sum_t (t-t0)^3 + C * sum_t (t-t0)^2
	   = sum_t (t-t0)^2 y_t,
         A * sum_t (t-t0)^3 + B * sum_t (t-t0)^2 + C * sum_t (t-t0)
	   = sum_t (t-t0) y_t,
         A * sum_t (t-t0)^2 + B * sum_t (t-t0) + C * sum_t 1
	   = sum_t  y_t.
       Write this is:
         A * sTTTT + B*sTTT + C*sTT = sTTY
	 A * sTTT  + B*sTT  + C*sT  = sTY
	 A * sTT   + B*sT   + C*s1  = sY
       Note that sT = sTTT = 0 by construction, so this simplifies to:
         A * sTTTT + C*sTT = sTTY,
	 B * sTT = sTY,
	 A * sTT + C*s1 = sY.
       Thus we find that
         B = sTY/sTT.
       Eliminate C from the last expression:
         C = (sY - A*sTT) / s1
       and insert in the first:
         A*sTTTT + (sY-A*sTT)/s1*sTT = sTTY
       Multiply by s1 and rearrange:
         A*(sTTTT*s1 - sTT*sTT) + sY*sTT = sTTY*s1,
       that is,
         A = (sTTY*s1 - sY*sTT) / (sTTTT*s1 - sTT*sTT).
    */
    // As for the Linear case, let's ignore first and last point.
    /* For numerical stability, I replace y(t) by (y(t)-avg(y(t)))
       in all equations. This only affects C, which I don't actually
       need anyway.
    */
    double t0 = double(N-1)/2;
    double sY=0, s1=0;
    for (int n=1; n<N-1; n++) {
      sY+=src[n];
      s1++;
    }
    double y0 = sY/s1;
    double sTY=0, sTTY=0;
    double sTT=0, sTTTT=0;
    for (int n=1; n<N-1; n++) {
      sTT+=sq(n-t0);
      sTTTT+=sq(sq(n-t0));
      double dy = src[n]-y0;
      sTY+=(n-t0)*dy;
      sTTY+=sq(n-t0)*dy;
    }
    double A = sTTY*s1 / (sTTTT*s1 - sTT*sTT);
    double B = sTY / sTT;
    for (int n=0; n<N; n++)
      dst[n] = src[n] - A*sq(n-t0) - B*(n-t0);
  } else {
    Dbg() << "Warning: Unknown debleach method: " << debleach;
    for (int n=0; n<N; n++)
      dst[n] = src[n];
  }

  /* The final step is to normalize the debleached data. */
  double sY=0, sYY=0;
  for (int n=1; n<N-1; n++) {
    double x = dst[n];
    sY+=x;
    sYY+=x*x;
  }
  double avg = sY/(N-2);
  //double var = (sYY-sY*sY/(N-2))/(N-2);
  //  dbg("getdebDFF: avg=%g var=%g",sY,avg,var);

  for (int n=0; n<N; n++)
    dst[n] = 100*((dst[n]/avg) - 1);
  
  debleached.validate();
  return dst;
}

