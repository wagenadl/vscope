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

CCDImage::CCDImage(QWidget *parent): QWidget(parent) {
  //  dbg("CCDImage(%p)::CCDImage(%p)",this,parent);
  min = 0;
  max = 65535;
  gamma_table = 0;
  adjust_black = adjust_white = 0;
  rebuildGammaTable();
  hasZoom = false;
  rubberband = 0;
  recolor("reset");
  createTestImage();
}

CCDImage::~CCDImage() {
  if (gamma_table)
    delete [] gamma_table;
}

//CCDImage::CCDImage(CCDImage const &other): QWidget(other.parentWidget()) {
//  throw Exception("CCDImage","Cannot copy CCDImages");
//}
//
//CCDImage &CCDImage::operator=(CCDImage const &) {
//  throw Exception("CCDImage","Cannot assign CCDImages");
//}

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
  image = QImage(512,512,QImage::Format_RGB32);
  // I could use Indexed8 if that's faster.
  uint32_t *dst = (uint32_t*)image.bits();
  int phase1 = ccdTestImageCounter*138;
  int phase2 = ccdTestImageCounter*38901;
  uint8_t *costbl = memalloc<uint8_t>(256, "CCDImage::createTestImage");
  for (int i=0; i<256; i++)
    costbl[i] = uint8_t(127+127*cos(i*6.2832/256));
  ccdTestImageCounter++;
  for (int y=0; y<512; y++) {
    for (int x=0; x<512; x++) {
      uint8_t rd = costbl[int(256*2*y/512+phase1)%256];
      uint8_t gr = costbl[int(256*3*x/512+phase2)%256];
      uint8_t bl = costbl[int(256*2*y/512+256*3*x/512+phase1+phase2)%256];
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
  dbg("ccdimage::setZoom(%p) l=%i t=%i r=%i b=%i im=%ix%i",this,
      z.left(),z.top(),z.right(),z.bottom(),image.width(),image.height());
  //if (z.left()<0 || z.top()<0
  //    || z.right()>image.width() || z.bottom()>image.height()) {
  //  hasZoom = false;
  //} else {
  zoomRect = z;
  hasZoom = true;
  constrainZoom();
  //}
  update();
  emit shareZoom(hasZoom,zoomRect);
}

void CCDImage::resetZoom() {
  hasZoom = false;
  update();
  emit shareZoom(hasZoom,zoomRect);
}

void CCDImage::zoomIn(int x0, int y0) {
  dbg("ccdimage: zoomin(%i,%i)",x0,y0);
  // zoom in to given point
  int w,h;
  if (hasZoom) {
    w = zoomRect.width();
    h = zoomRect.height();
  } else {
    w = image.width();
    h = image.height();
  }
  setZoom(QRect(x0-w/4,y0-h/4,w/2,h/2));
}

void CCDImage::zoomIn() {
  dbg("ccdimage: zoomin");
  QRect r = hasZoom ? zoomRect
    : QRect(0,0,image.width(),image.height());
  int w = r.width();
  int h = r.height();
  setZoom(QRect(r.left()+w/4,r.top()+h/4,w/2,h/2));
}  

void CCDImage::zoomOut() {
  if (!hasZoom)
    return;
  QRect z = zoomRect;
  //  dbg("zoomOut(%p) l=%i t=%i r=%i b=%i im=%ix%i",this,
  //      z.left(),z.top(),z.right(),z.bottom(),image.width(),image.height());
  int w = z.width();
  int h = z.height();
  setZoom(QRect(z.left()-w/2,z.top()-h/2,w*2,h*2));
}

void CCDImage::constrainZoom() {
  if (!hasZoom)
    return;
  int iw = image.width();
  int ih = image.height();
  int zw = zoomRect.width();
  int zh = zoomRect.height();
  int zx = zoomRect.left();
  int zy = zoomRect.top();

  double irat = double(iw) / double(ih);
  if (zw<zh*irat) {
    zx -= int(zh*irat - zw)/2;
    zw = int(zh*irat);
  } else if (zh<zw/irat) {
    zy -= int(zw/irat - zh)/2;
    zh = int(zw/irat);
  }

  if (zx<0)
    zx=0;
  else if (zx+zw > iw)
    zx = iw-zw;
  if (zx+zw>iw)
    hasZoom=false;

  if (zy<0)
    zy=0;
  else if (zy+zh > ih)
    zy = ih-zh;
  if (zy+zh>ih)
    hasZoom=false;

  zoomRect = QRect(zx,zy,zw,zh);
}

void CCDImage::sharedZoom(bool has, QRect rect) {
  //  dbg("ccdimage(%p):sharedzoom %i",this,has);
  hasZoom = has;
  zoomRect = rect;
  update();
}

CCDImage::ZoomInfo CCDImage::makeZoomInfo() {
  ZoomInfo z;
  QRect zr = safeZoomRect();

  /* We have to do the zoom transformation if hasZoom is true.
     Screen coordinates (0,0)-(w,h) correspond to
       (zoomRect.left(),zoomRect.top())-
         (zoomRect.right()+1,zoomRect.bottom()+1).
     (I think the +1 is correct.)
     We need an equation:
       x_screen = a*x_world + b
     that captures that. The correspondence above says:
       0 = a*zR.left + b
       w = a*(zR.right+1) + b
     Subtract these:
       w = a*(zR.right+1) - a*zR.left = a*zR.width
     Thus:
       a = w / zR.width.
     And:
	 b = -a * zR.left.
  */

  z.ax = double(width()) / zr.width();
  z.ay = double(height()) / zr.height();
  /* Note that we always zoom with ratio preserved, thus ax = ay, but
     this is more general.
  */
  z.bx = -z.ax*zr.left();
  z.by = -z.ay*zr.top();
  return z;
  }


void CCDImage::paintEvent(class QPaintEvent *) {
  QPainter p(this);
  QRect r = rect(); // (0,0,width,height) of this widget
  constrainZoom();

  if (hasZoom)
    p.drawImage(r,image,zoomRect,
		Qt::DiffuseDither|Qt::ColorOnly|Qt::PreferDither);
  else
    p.drawImage(r,image);
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

void CCDImage::mouseReleaseEvent(QMouseEvent *e) {
  if (clickPoint.x()<0) {
    dbg("ccdimage: release from double click");
    return; // this is the release of a double click; we do not care.
  }
  
  dbg("CCDImage: release (%i,%i)\n",e->x(),e->y());

  if (!rubberband)
    return; // this shouldn't happen

  QRect r = rubberband->geometry().normalized();
  dbg("release l=%i t=%i r=%i b=%i",
      r.left(),r.top(),r.right(),r.bottom());
  if (r.width()<5 && r.height()<5) {
    r = hasZoom ? zoomRect : rect();
    r = QRect(clickPoint.x()-r.width()/4,
	      clickPoint.y()-r.height()/4,
	      r.width()/2,
	      r.height()/2);
  } 
  rubberband->hide();
  setZoom(screenToImage(r));
}

void CCDImage::mouseMoveEvent(QMouseEvent *e) {
  //printf("CCDImage: move (%i,%i)\n",e->x(),e->y());
  if (!rubberband)
    return; // this shouldn't happen
  int dx = e->x() - clickPoint.x();
  int dy = e->y() - clickPoint.y();
  int sdx = dx>0 ? 1 : -1;
  int sdy = dy>0 ? 1 : -1;
  if (dx*sdx>dy*sdy)
    dy = dx*sdx*sdy;
  else
    dx = dy*sdx*sdy;
  rubberband->setGeometry(QRect(clickPoint,QSize(dx,dy)).normalized());
}

void CCDImage::mouseDoubleClickEvent(QMouseEvent *) {
  //printf("CCDImage: double Click (%i,%i)\n",e->x(),e->y());
  clickPoint=QPoint(-1,-1);
  resetZoom();
}

void CCDImage::overwriteImage(QImage img) {
  image = img;
  update();
}

QRect CCDImage::safeZoomRect() const {
  if (hasZoom)
    return zoomRect;
  else
    return QRect(0,0,image.width(),image.height());
}


XYRRA CCDImage::imageToScreen(XYRRA el) const {
  QRect r = safeZoomRect();
  double ax = double(width()) / r.width();
  double ay = double(height()) / r.height();
  double bx = -ax*r.left();
  double by = -ay*r.top();
  el.x0 = ax*el.x0+bx;
  el.y0 = ay*el.y0+by;
  XYABC abc(el);
  abc.a /= ax*ax;
  abc.b /= ay*ay;
  abc.c /= ax*ay;
  el = abc.toXYRRA();
  return el;
}

QRect CCDImage::imageToScreen(QRect xy) const {
  QRect r = safeZoomRect();
  double ax = double(width()) / r.width();
  double ay = double(height()) / r.height();
  double bx = -ax*r.left();
  double by = -ay*r.top();
  return QRect(roundi(ax*xy.left()+bx), roundi(ay*xy.top()+by),
	       roundi(ax*xy.width()), roundi(ay*xy.height()));
}

QPoint CCDImage::imageToScreen(QPointF xy) const {
  QRect r = safeZoomRect();
  double ax = double(width()) / r.width();
  double ay = double(height()) / r.height();
  double bx = -ax*r.left();
  double by = -ay*r.top();
  return QPoint(roundi(ax*xy.x()+bx), roundi(ay*xy.y()+by));
}



double CCDImage::imageToScreen(double len) const {
  QRect r = safeZoomRect();
  double ax = double(width()) / r.width();
  double ay = double(height()) / r.height();
  len *= sqrt(ax*ay);
  return len;
}

XYRRA CCDImage::screenToImage(XYRRA el) const {
  QRect r = safeZoomRect();
  /* As in paintEvent, we write x_screen = a*x_image + bx, thus
     x_image = (x_screen-bx)/a. */
  double ax = double(width()) / r.width();
  double ay = double(height()) / r.height();
  double bx = -ax*r.left();
  double by = -ay*r.top();
  el.x0 = (el.x0-bx)/ax;
  el.y0 = (el.y0-by)/ay;
  XYABC abc(el);
  abc.a *= ax*ax;
  abc.b *= ay*ay;
  abc.c *= ax*ay;
  el = abc.toXYRRA();
  return el;
}

QPointF CCDImage::screenToImage(QPoint xy) const {
  QRect r = safeZoomRect();
  double ax = double(width()) / r.width();
  double ay = double(height()) / r.height();
  double bx = -ax*r.left();
  double by = -ay*r.top();
  return QPointF((xy.x()-bx)/ax, (xy.y()-by)/ay);
}

QRect CCDImage::screenToImage(QRect r) const {
  QRect zr = safeZoomRect();
  double ax = double(width()) / zr.width();
  double ay = double(height()) / zr.height();
  /* Since we preserve aspect ratio in zoom, a=ax=ay by definition. */
  double bx = -ax*zr.left();
  double by = -ay*zr.top();
  return QRect(roundi((r.left()-bx)/ax), roundi((r.top()-by)/ay),
	       roundi(r.width()/ax), roundi(r.height()/ay));
}

double CCDImage::screenToImage(double len) const {
  QRect r = safeZoomRect();
  double ax = double(width()) / r.width();
  double ay = double(height()) / r.height();
  len /= sqrt(ax*ay);
  return len;
}

