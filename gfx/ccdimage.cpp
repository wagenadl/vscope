// ccdimage.cpp

#include <gfx/ccdimage.h>
#include <base/exception.h>
#include <base/types.h>
#include <algorithm>
#include <QPainter>
#include <math.h>
#include <stdio.h>
#include <QMouseEvent>
#include <QRubberBand>
#include <base/dbg.h>
#include <base/xyrra.h>
#include <base/numbers.h>
#include <base/minmax.h>
#include <base/xyabc.h>

#define CCDImage_GAMMA_Entries 4096

CCDImage::CCDImage(QWidget *parent):
  QWidget(parent) {
  min = 0;
  max = 65535;
  adjust_black = adjust_white = 0;
  aceFraction = 0;
  hasACE = false;
  rebuildGammaTable();
  rubberband = 0;
  recolor("reset");
  canvasRect = rect();
  setAutoFillBackground(true);
  QPalette p = palette();
  p.setColor(QPalette::Window, QColor("#333333"));
  setPalette(p);
  //Dbg() << "CCDImage: CanvasRect is " << canvasRect;
  resetZoom();
}

CCDImage::~CCDImage() {
}

void CCDImage::setCanvas(QRect r) {
  canvasRect = r;
  resetZoom();
  update();
}

QRect CCDImage::currentCanvas() const {
  return canvasRect.isNull() ? rect() : canvasRect;
}

void CCDImage::resetCanvas() {
  canvasRect = QRect();
  constrainZoom();
  update();
}

void CCDImage::rebuildGammaTable() {
  gamma_table.resize(CCDImage_GAMMA_Entries);
  for (int k=0; k<CCDImage_GAMMA_Entries; k++) {
    double v = double(k) / (CCDImage_GAMMA_Entries-1);
    v = 1-pow(1-pow(v,1+adjust_black),1+adjust_white);
    gamma_table[k]=floori(255.99999999*v);
  }
}

void CCDImage::newImage(uint16_t const *data, int X, int Y,
			bool flipX, bool flipY) {
  Transform t;
  if (flipX)
    t.flipx(X);
  if (flipY)
    t.flipy(Y);
  newImage(data, X, Y, t);
}

void CCDImage::black() {
  image.fill(0);
}

void CCDImage::newImage(uint16_t const *data, int X, int Y,
			Transform const &t) {

  bool flipX = t.reflectsX();
  bool flipY = t.reflectsY();

  Dbg() << "newImage " << data << " " << X << "x" << Y;

  // simply copy data, taking care of flips
  origImage.ensureSize(X, Y);
  float *dst = origImage.data();
  float rng = 1 + max - min;
  for (int y=0; y<Y; y++) {
    uint16_t const *row = flipY ? (data+(Y-1-y)*X) : data+y*X;
    for (int x=0; x<X; x++) 
      *dst++ = 256.*(row[flipX ? (X-1-x) : x] - min) / rng;
  }

  Transform t0;
  if (flipX)
    t0.flipx(X);
  if (flipY)
    t0.flipy(Y);
  img2cnv = t(t0);

  hasACE = false;

  rebuildImage();
}

void CCDImage::rebuildImage() {
  int X = origImage.width();
  int Y = origImage.height();
  Dbg() << "rebuildimage" << X << "x" << Y;

  // rebuild gamma image if needed
  if (adjust_black>0 || adjust_white>0) {
    gammaImage.ensureSize(X, Y);
    float const *src = origImage.data();
    float *dst = gammaImage.data();
    float const *gamma = gamma_table.constData();
    int N = X*Y;
    while (N--) {
      int idx = *src++ * CCDImage_GAMMA_Entries / 256;
      if (idx<0)
	idx = 0;
      else if (idx>=CCDImage_GAMMA_Entries)
	idx = CCDImage_GAMMA_Entries-1;
      *dst++ = gamma[idx];
    }
  }

  // rebuild ACE immage if needed
  if (aceFraction>0 && !hasACE) {
    aceImage = origImage.ace(X/51, X/21, Y/51, Y/21, true);
    hasACE = true;
  }

  // reconstruct final image
  if (aceFraction>0) {
    FloatImage x = (adjust_black>0 || adjust_white>0) ? gammaImage : origImage;
    x *= (1-aceFraction);
    FloatImage y = aceImage;
    y += 2; // convert [-2, 2] to [0, 4]
    y *= 64 * aceFraction; // convert [0, 4] to [0, 256], and mul by frac
    x += y;
    image = x.toImage(0, 255);
  } else {
    image = ((adjust_black>0 || adjust_white>0) ? gammaImage : origImage)
      .toImage(0, 255);
  }

  update();
}

void CCDImage::recolor(QString act) {
  if (act=="reset") {
    adjust_black = 0;
    adjust_white = 0;
  } else if (act=="blackPlus") {
    if (adjust_black<.05)
      adjust_black += .05;
    else
      adjust_black *= 1.3;
  } else if (act=="blackMin") {
    adjust_black /= 1.3;
  } else if (act=="whitePlus") {
    if (adjust_white<.05)
      adjust_white += .05;
    else
      adjust_white *= 1.3;
  } else if (act=="whiteMin") {
    adjust_white /= 1.3;
  } else if (act=="acePlus") {
    aceFraction += 0.10;
    if (aceFraction>1)
      aceFraction = 1;
  } else if (act=="aceMin") {
    aceFraction -= 0.10;
    if (aceFraction<0)
      aceFraction = 0;
  }
  // dbg("recolor: black: %.2f white %.2f",adjust_black, adjust_white);
  rebuildGammaTable();
  rebuildImage();
}

void CCDImage::adjustedRange(uint16_t const *data, int X, int Y) {
  autoRange(data,X,Y,0,1);
  //int dif = max-min;
  //min += int(adjust_black*dif);
  //max -= int(adjust_white*dif);
}

void CCDImage::autoRange(uint16_t const *data, int X, int Y,
			 double frc0, double frc1) {
  if (data && X*Y>0) {
    int kmin = int(floor(frc0*X*Y));
    if (kmin<0)
      kmin=0;
    else if (kmin>=X*Y)
      kmin=X*Y-1;
    
    int kmax = int(floor(frc1*X*Y));
    if (kmax<0)
      kmax=0;
    else if (kmax>=X*Y)
      kmax=X*Y-1;

    if (kmin>0 || kmax<X*Y-1) {
      QVector<uint16_t> cp(X*Y);
      memcpy((void *)cp.data(), (void const *)data, X*Y*2);

      uint16_t *d = cp.data();

      std::nth_element(d, d+kmin, d+X*Y);
      min=cp[kmin];
    
      std::nth_element(d, d+kmax, d+X*Y);
      max=cp[kmax];
    } else {
      min=data[0];
      max=data[0];
      int K=X*Y;
      while (K--) {
	uint16_t v = *data++;
	if (v<min)
	  min=v;
	if (v>max)
	  max=v;
      }
    }
  } else {
    min = 0;
    max = 65535;
  }
}

void CCDImage::setRange(uint16_t mn, uint16_t mx) {
  min = mn;
  max = mx;
}

void CCDImage::setZoom(QRect const &z) {
  zoomRect = z;
  constrainZoom();
  update();
  emit newZoom(zoomRect);
}

void CCDImage::resetZoom() {
  zoomRect = currentCanvas();
  constrainZoom();
  update();
  emit newZoom(zoomRect);
}

void CCDImage::zoomIn(int x0, int y0) {
  // zoom in to given point
  int w = zoomRect.width();
  int h = zoomRect.height();
  setZoom(QRect(x0-w/4,y0-h/4,w/2,h/2));
}

void CCDImage::zoomIn(QPoint p) {
  zoomIn(p.x(), p.y());
}

void CCDImage::zoomIn() {
  QRect r = zoomRect;
  int w = r.width();
  int h = r.height();
  setZoom(QRect(r.left()+w/4,r.top()+h/4,w/2,h/2));
}  

void CCDImage::zoomOut() {
  QRect z = zoomRect;
  int w = z.width();
  int h = z.height();
  setZoom(QRect(z.left()-w/2,z.top()-h/2,w*2,h*2));
}

void CCDImage::constrainZoom() {
  QRect cr = currentCanvas();
  int iw = cr.width();
  int ih = cr.height();
  int ix = cr.left();
  int iy = cr.top();
  int zw = zoomRect.width();
  int zh = zoomRect.height();
  int zx = zoomRect.left();
  int zy = zoomRect.top();

  // make aspect ratio match
  double irat = double(iw) / double(ih);
  if (zw<zh*irat) {
    zx -= int(zh*irat - zw)/2;
    zw = int(zh*irat);
  } else if (zh<zw/irat) {
    zy -= int(zw/irat - zh)/2;
    zh = int(zw/irat);
  }

  // shift x around if needed
  if (zx+zw > ix+iw)
    zx = ix+iw - zw;
  if (zx<ix)
    zx=ix;

  // shift y around if needed
  if (zy+zh > iy+ih)
    zy = iy+ih - zh;
  if (zy<0)
    zy=0;

  // reset if still not inside
  if (zx+zw>ix+iw || zy+zh>iy+ih)
    zoomRect = currentCanvas();
  else
    zoomRect = QRect(zx,zy,zw,zh);

  // recalculate transformation
  cnv2scr = Transform::inferred(zoomRect, rect());
  
}

void CCDImage::updateZoom(QRect rect) {
  zoomRect = rect;
  constrainZoom();
  update();
}

void CCDImage::paintEvent(class QPaintEvent *) {
  QPainter p(this);
  QRect r = rect(); // (0,0,width,height) of this widget
  constrainZoom();
  p.drawImage(r, image, img2cnv.inverse()(zoomRect),
	      Qt::DiffuseDither|Qt::ColorOnly|Qt::PreferDither);
}

void CCDImage::mousePressEvent(QMouseEvent *e) {
  // only zooming implemented here, rois are in ROIImage.cpp
  clickPoint = e->pos();
  if (!rubberband)
    rubberband = new QRubberBand(QRubberBand::Rectangle, this);
  rubberband->setGeometry(e->x(),e->y(),0,0);
  rubberband->show();
}

void CCDImage::mouseReleaseEvent(QMouseEvent *) {
  if (clickPoint.x()<0) 
    return; // this is the release of a double click; we do not care.

  if (!rubberband)
    return; // this shouldn't happen

  QRect r = rubberband->geometry().normalized();
  rubberband->hide();

  if (r.width()<5 && r.height()<5) 
    // too small, so simply zoom in
    zoomIn(cnv2scr.inverse()(clickPoint));
  else 
    setZoom(cnv2scr.inverse()(r));
}

void CCDImage::mouseMoveEvent(QMouseEvent *e) {
  if (!rubberband)
    return; // this shouldn't happen

  int dx = e->x() - clickPoint.x();
  int dy = e->y() - clickPoint.y();
  int sdx = dx>0 ? 1 : -1;
  int sdy = dy>0 ? 1 : -1;

  // make it square
  if (dx*sdx>dy*sdy)
    dy = dx*sdx*sdy;
  else
    dx = dy*sdx*sdy;

  rubberband->setGeometry(QRect(clickPoint,QSize(dx,dy)).normalized());
}

void CCDImage::mouseDoubleClickEvent(QMouseEvent *) {
  clickPoint=QPoint(-1,-1);
  resetZoom();
}

void CCDImage::overwriteImage(QImage const &img) {
  image = img;
  constrainZoom();
  update();
}
