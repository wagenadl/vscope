// roidata.cpp

#include "roidata.h"
#include <base/ccddata.h>
#include <base/numbers.h>
#include <base/minmax.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <base/blobroi.h>

inline double sq(double x) { return x*x; }

inline int rangelimit(int x, int min, int max) {
  if (x<min)
    x=min;
  else if (x>max)
    x=max;
  return x;
}

ROIData::ROIData() {
  dataRaw = 0;
  dataDebleached = 0;
  roip = 0;
  blobROI = 0;
  lengthRaw = 0;
  lengthDebleached = 0;
  validRaw = false;
  validDebleached = false;
  validBitmap = false;
  validBlobROI = false;
  usePolyNotXyrra = false;
  source = 0;
  bitmap = 0;
  debleach = None;
  flipY = false;
  flipX = false;
}

ROIData::~ROIData() {
  if (dataRaw)
    delete dataRaw;
  if (dataDebleached)
    delete dataDebleached;
  if (bitmap)
    delete bitmap;
  if (blobROI)
    delete blobROI;
}

void ROIData::setDebleach(ROIData::Debleach d) {
  if (d!=debleach) {
    debleach = d;
    validDebleached = false;
  }
}

void ROIData::setFlip(bool x, bool y) {
  bool chgd = false;
  if (flipX != x) {
    flipX = x;
    chgd = true;
  }
  if (flipY != y) {
    flipY = y;
    chgd = true;
  }
  if (chgd)
    validRaw = validDebleached = false;
}
  

void ROIData::setData(CCDData const *source0) {
  source = source0;
  validRaw = false;
  validDebleached = false;
}

void ROIData::setROI(XYRRA const &roi0) {
  roi = roi0;
  usePolyNotXyrra = false;
  validBlobROI = false;
  validBitmap = false;
  validRaw = false;
  validDebleached = false;
}

void ROIData::setROI(PolyBlob const *roi0) {
  roip = roi0;
  usePolyNotXyrra = true;
  validBlobROI = false;
  validBitmap = false;
  validRaw = false;
  validDebleached = false;
}

int ROIData::getNFrames() const {
  return source ? source->getNFrames() : 0;
}

void ROIData::ensureBitmap() {
  /* The bitmap holds which pixels are inside the ROI and which are not.
     To avoid wasted effort, we only calculate inside a rectangular
     bounding box.
     It would not be too terribly hard to modify this function to produce
     a soft-edged ROI, but that's a luxury for later.
  */
  if (validBitmap)
    return;

  if (usePolyNotXyrra)
    makePolyBitmap();
  else
    makeXYRRABitmap();
}

void ROIData::makePolyBitmap() {
  if (blobROI) 
    delete blobROI;
  blobROI = new BlobROI(*roip);
  validBlobROI = true;
  xl = blobROI->bitmapX0();
  yt = blobROI->bitmapY0();
  int w1 = blobROI->bitmapW();
  int h1 = blobROI->bitmapH();
  npix = blobROI->nPixels();
  //dbg("xl=%i yt=%i w1=%i h1=%i npix=%i w=%i h=%i",xl,yt,w1,h1,npix,w,h);
  if (w1!=w || h1!=h) {
    if (bitmap)
      delete bitmap;
    bitmap = 0;
    w=w1;
    h=h1;
  }
  if (!bitmap)
    bitmap = new bool[w*h];
  int r = blobROI->bitmap(bitmap,w*h);
  if (r!=w*h)
    throw Exception("ROIData",
		    QString("Unexpected bitmap size: %1 instead of %2*%3")
		    .arg(r).arg(w).arg(h));
  validBitmap = true;
}      

void ROIData::makeXYRRABitmap() {
  QRectF bb = roi.bbox();
  int maxx = source ? source->getSerPix() : 0;
  int maxy = source ? source->getParPix() : 0;
  xl = rangelimit(floori(bb.left()),0,maxx);
  int xr = rangelimit(ceili(bb.right()),0,maxx);
  yt = rangelimit(floori(bb.top()),0,maxy);
  int yb = rangelimit(ceili(bb.bottom()),0,maxy);
  int w1 = xr-xl;
  int h1 = yb-yt;
  if (bitmap && (w!=w1 || h!=h1)) {
    delete [] bitmap;
    bitmap = 0;
  }
  w = w1;
  h = h1;
  if (!bitmap) 
    bitmap = new bool[w*h];
  bool *ptr = bitmap;
  double cs = cos(roi.a);
  double sn = sin(roi.a);
  npix = 0;
  for (int y=yt; y<yb; y++) {
    double dy = y-roi.y0;
    for (int x=xl; x<xr; x++) {
      double dx = x-roi.x0;
      double xi = dx*cs + dy*sn;
      double eta = -dx*sn + dy*cs;
      bool isInside = sq(xi)/sq(roi.R) + sq(eta)/sq(roi.r) < 1;
      *ptr++ = isInside;
      if (isInside)
	npix++;
    }
  }
  validBitmap = true;
}

double const *ROIData::getRaw() {
  if (validRaw)
    return dataRaw;

  if (!source)
    return 0;

  int len = getNFrames();
  if (dataRaw && lengthRaw!=len) {
    delete [] dataRaw;
    dataRaw=0;
  }
  if (!dataRaw) {
    dataRaw = new double[len];
    lengthRaw = len;
  }

  ensureBitmap();

  int W = source->getSerPix();
  int H = source->getParPix();
  int yt_eff = flipY ? H - 1 - yt : yt;
  int ymul = W * (flipY ? -1 : 1);
  int xl_eff = flipX ? W - 1 - xl : xl;
  int xmul = flipX ? -1 : 1;

  dbg("flipx=%i flipy=%i xl_eff=%i",flipX,flipY,xl_eff);
  
  int w_eff = w;
  if (flipX) {
    if (xl_eff>=W) {
      w_eff -= 1 + xl_eff - W;
      xl_eff = W-1;
    }
    if (xl_eff - w_eff < -1)
      w_eff = xl_eff + 1;
  } else {
    if (xl_eff<0) {
      w_eff += xl_eff;
      xl_eff = 0;
    }
    if (xl_eff + w_eff > W)
      w_eff = W - xl_eff;
  }
  
  int h_eff = h;
  if (flipY) {
    if (yt_eff>=H) {
      h_eff -= 1 + yt_eff - H;
      yt_eff = H-1;
    }
    if (yt_eff - h_eff < -1)
      h_eff = yt_eff + 1;
  } else {
    if (yt_eff < 0) {
      h_eff += yt_eff;
      yt_eff = 0;
    }
    if (yt_eff + h_eff > H) 
      h_eff = H - yt_eff;
  }

  for (int n=0; n<len; n++) {
    double sum=0;
    double sum1=0;
    uint16_t const *frm = source->frameData(n) + xl_eff
      + yt_eff*source->getParPix();
    bool *bm = bitmap;
    for (int y=0; y<h_eff; y++) {
      uint16_t const *row = frm + y*ymul;
      for (int x=0; x<w_eff; x++) {
	if (*bm++)
	  sum+=*row;
	sum1++;
	row+=xmul;
      }
    }
    dataRaw[n] = sum/sum1;
  }

  validRaw = true;
  return dataRaw;
}

double const *ROIData::getDebleachedDFF() {
  //  dbg("getDebleachedDFF. valid=%c",validDebleached?'y':'n');
  if (validDebleached)
    return dataDebleached;

  if (!getRaw())
    return 0;

  int len = getNFrames();
  if (dataDebleached && lengthDebleached!=len) {
    delete [] dataDebleached;
    dataDebleached=0;
  }
  if (!dataDebleached) {
    dataDebleached = new double[len];
    lengthDebleached = len;
  }

  switch (debleach) {
  case None:
    for (int n=0; n<len; n++)
      dataDebleached[n] = dataRaw[n];
    break;
  case Linear: case Exponential: {
    // We treat exponential like linear until I get around to coding it.
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
    double t0 = double(len-1)/2;
    double sY=0;
    for (int n=1; n<len-1; n++)
      sY+=dataRaw[n];
    double y0 = sY/(len-2);

    double sTT=0, sTY=0;
    for (int n=1; n<len-1; n++) {
      sTT+=sq(n-t0);
      sTY+=(n-t0)*(dataRaw[n]-y0);
    }
    double A = sTY/sTT;
    for (int n=0; n<len; n++)
      dataDebleached[n] = dataRaw[n] - A*(n-t0);
    // dbg("sTT=%g sTY=%g A=%g",sTT,sTY,A);
  } break;
  case Quadratic: {
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
    double t0 = double(len-1)/2;
    double sY=0, s1=0;
    for (int n=1; n<len-1; n++) {
      sY+=dataRaw[n];
      s1++;
    }
    double y0 = sY/s1;
    double sTY=0, sTTY=0;
    double sTT=0, sTTTT=0;
    for (int n=1; n<len-1; n++) {
      sTT+=sq(n-t0);
      sTTTT+=sq(sq(n-t0));
      double dy = dataRaw[n]-y0;
      sTY+=(n-t0)*dy;
      sTTY+=sq(n-t0)*dy;
    }
    double A = sTTY*s1 / (sTTTT*s1 - sTT*sTT);
    double B = sTY / sTT;
    for (int n=0; n<len; n++)
      dataDebleached[n] = dataRaw[n] - A*sq(n-t0) - B*(n-t0);
  } break;
  }

  /* The final step is to normalize the debleached data. */
  double sY=0;
  for (int n=1; n<len-1; n++)
    sY+=dataDebleached[n];
  double avg = sY/(len-2);
  //dbg("sY=%g avg=%g",sY,avg);

  for (int n=0; n<len; n++)
    dataDebleached[n] = 100*((dataDebleached[n]/avg) - 1);
  
  validDebleached = true;
  return dataDebleached;
}

ROIData::ROIData(ROIData const &other): ROIData_(other) {
  copy(other);
}

void ROIData::copy(ROIData const &other) {
  if (dataRaw) {
    dataRaw = new double[lengthRaw];
    memcpy(dataRaw, other.dataRaw, lengthRaw*sizeof(double));
  }
  if (dataDebleached) {
    dataDebleached = new double[lengthDebleached];
    memcpy(dataDebleached, other.dataDebleached,
	   lengthDebleached*sizeof(double));
  }
  if (bitmap) {
    bitmap = new bool[w*h];
    memcpy(bitmap, other.bitmap, w*h*sizeof(bool));
  }
  if (blobROI)
    blobROI = new BlobROI(*other.blobROI);
}

ROIData &ROIData::operator=(ROIData const &other) {
  if (dataRaw)
    delete [] dataRaw;
  if (dataDebleached)
    delete [] dataDebleached;
  if (bitmap)
    delete [] bitmap;
  if (blobROI)
    delete blobROI;
  *(ROIData_*)this = other;
  copy(other);
  return *this;
}
