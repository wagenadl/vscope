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
#include <base/memalloc.h>

#define CCDImage_GAMMA_Entries 4096

CCDImage::CCDImage(QWidget *parent):
  QWidget(parent) {
  min = 0;
  max = 65535;
  gamma_table = 0;
  adjust_black = adjust_white = 0;
  rebuildGammaTable();
  rubberband = 0;
  recolor("reset");
  canvasRect = rect();
  setAutoFillBackground(true);
  QPalette p = palette();
  p.setColor(QPalette::Window, QColor("#333333"));
  setPalette(p);
  Dbg() << "CCDImage: CanvasRect is " << canvasRect;
  createTestImage();
  resetZoom();
}

CCDImage::~CCDImage() {
  if (gamma_table)
    delete [] gamma_table;
}

void CCDImage::setCanvas(QRect r) {
  canvasRect = r;
  resetZoom();
  update();
}

void CCDImage::placeImage(Transform const &t) {
  img2cnv = t;
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
  if (!gamma_table)
    gamma_table = memalloc<int>(CCDImage_GAMMA_Entries,
				"CCDImage::rebuildGammaTable");
  for (int k=0; k<CCDImage_GAMMA_Entries; k++) {
    double v = double(k) / (CCDImage_GAMMA_Entries-1);
    v = 1-pow(1-pow(v,1+adjust_black),1+adjust_white);
    gamma_table[k]=floori(255.99999999*v);
  }
}

static int ccdTestImageCounter = 0;

void CCDImage::createTestImage() {
  int wid = canvasRect.width();
  int hei = canvasRect.height();
  image = QImage(wid, hei, QImage::Format_RGB32);
  // I could use Indexed8 if that's faster.
  uint32_t *dst = (uint32_t*)image.bits();
  int phase1 = ccdTestImageCounter*138;
  int phase2 = ccdTestImageCounter*38901;
  uint8_t *costbl = memalloc<uint8_t>(256, "CCDImage::createTestImage");
  for (int i=0; i<256; i++)
    costbl[i] = uint8_t(127+127*cos(i*6.2832/256));
  ccdTestImageCounter++;
  for (int y=0; y<hei; y++) {
    for (int x=0; x<wid; x++) {
      uint8_t rd = costbl[int(256*2*y/hei+phase1)%256];
      uint8_t gr = costbl[int(256*3*x/wid+phase2)%256];
      uint8_t bl = costbl[int(256*2*y/hei+256*3*x/wid+phase1+phase2)%256];
      *dst++ = (rd<<16) + (gr<<8) + bl + 0xff000000;
    }
  }
  delete [] costbl;
}

void CCDImage::newImage(uint16_t const *data, int X, int Y,
			bool flipX, bool flipY) {
  if (image.width()!=X || image.height()!=Y)
    image = QImage(X,Y,QImage::Format_RGB32);
  // I could use Indexed8 if that's faster.
  Dbg() << "CCDImage::newImage " << data
	<< " " << X << "x" << Y
	<< " (" << flipX << "," << flipY << ")";
  uint32_t *dst = (uint32_t *)image.bits();
  int rng = 1 + max - min;
  Dbg() << "  max="<<max<<" min="<<min<<" rng="<<rng;
  for (int y=0; y<Y; y++) {
    uint16_t const *row = flipY ? (data+(Y-1-y)*X) : data+y*X;
    if (adjust_black>0 || adjust_white>0) {
      for (int x=0; x<X; x++) {
	int px = row[flipX ? (X-1-x) : x] - min;
	px*=CCDImage_GAMMA_Entries;
	px/=rng;
	if (px<0)
	  px=0;
	else if (px>=CCDImage_GAMMA_Entries)
	  px=CCDImage_GAMMA_Entries-1;
	*dst++ = gamma_table[px]*0x010101 + 0xff000000;
      }
    } else {
      for (int x=0; x<X; x++) {
	int px = row[flipX ? (X-1-x) : x] - min;
	px*=256;
	px/=rng;
	if (px<0)
	  px=0;
	else if (px>255)
	  px=255;
	*dst++ = px*0x010101 + 0xff000000;
      }
    }
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
  }
  dbg("recolor: black: %.2f white %.2f",adjust_black, adjust_white);
  rebuildGammaTable();
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
    if (frc0>0 || frc1<1) {
      uint16_t *cp = memalloc<uint16_t>(X*Y, "CCDImage::autorange");
      memcpy((void*)cp,(void*)data,X*Y*2);
    
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
    
      std::nth_element(cp,cp+kmin,cp+X*Y);
      min=cp[kmin];
    
      std::nth_element(cp,cp+kmax,cp+X*Y);
      max=cp[kmax];
    
      delete [] cp;
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

  Dbg() << "CCDImage::paintEvent. imgrect=" << image.rect()
	<< " zoomrect=" << zoomRect
	<< " canvasrect=" << canvasRect
	<< " rect=" << r;

  //  if (zoomRect==r)
  //    p.drawImage(r,image);
  //  else
  p.drawImage(r,image,img2cnv.inverse()(zoomRect),
	      Qt::DiffuseDither|Qt::ColorOnly|Qt::PreferDither);
}

void CCDImage::mousePressEvent(QMouseEvent *e) {
  // only zooming implemented here
  dbg("CCDImage: press (%i,%i)\n",e->x(),e->y());
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
