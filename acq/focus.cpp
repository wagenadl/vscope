// focus.cpp

#include "focus.h"

#include <base/enums.h>
#include <math.h>
#include <xml/paramtree.h>
#include <xml/enumerator.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <gfx/ccdimage.h>
#include <gui/guiroot.h>
#include <gui/guibutton.h>

#define SELFPOLL 1
// If SELFPOLL is set, we use a timer to force updates rather than pvp's
// mechanism.

#include <QTimer>
#include <pvp/campool.h>
#include <pvp/ccdconfig.h>
#include <xml/connections.h>

Focus::Focus(QWidget *parent): QFrame(parent) {
  CamPool::initialize();
  isActive = false;
  viewMode = FM_TwoFull;
  timer = 0;
  
  butFocus[0] = butFocus[1] = 0;
  butRotate = butScale = 0;
  butShiftX = butShiftY = 0;

  camA = camB = 0;
  frmA = frmB = 0;
  
  left = new CCDImage(this);
  right = new CCDImage(this);
  hiddenA = new CCDImage(this); hiddenA->hide();
  hiddenB = new CCDImage(this); hiddenB->hide();

  QRect cnv(0,0,512,512);
  left->setCanvas(cnv);
  right->setCanvas(cnv);
  hiddenA->setCanvas(cnv);
  hiddenB->setCanvas(cnv);

  cfgA = new CCDConfig();
  cfgB = new CCDConfig();

  cfgA->iscont = true;
  cfgB->iscont = true;

  setCams("");
}

QString Focus::getCamA() const {
  return camA ? camA->getID() : "none";
}

QString Focus::getCamB() const {
  return camA ? camB->getID() : "none";
}

void Focus::setCams(QString idA) {
  setCams(idA, "");
}

void Focus::setCamA(QString idA) {
  setCams(idA, camB ? camB->getID() : "");
}

void Focus::setCamB(QString idB) {
  setCams(camA ? camA->getID() : "", idB);
}

void Focus::setCams(QString idA, QString idB) {
  if (idA=="")
    idA = Connections::leaderCamera();

  bool wasactive = isActive;
  if (wasactive)
    deactivate();

  Dbg() << "Focus::setCams("<<idA<<")";
  
  Connections::CamCon const *ccA = Connections::findpCam(idA);
  Connections::CamCon const *ccB =
    idB!=""
    ? Connections::findpCam(idA)
    : ccA
    ? Connections::findpCam(ccA->partnerid)
    : 0;
  Dbg() << "  Focus: ccA=" << ccA << ". ccB=" << ccB;

  flipXA = ccA ? ccA->placement.reflectsX() : false;
  flipYA = ccA ? ccA->placement.reflectsY() : false;
  exp_msA = ccA ? ccA->focusexp_ms : 100;

  flipXB = ccB ? ccB->placement.reflectsX() : false;
  flipYB = ccB ? ccB->placement.reflectsY() : false;
  exp_msB = ccB ? ccB->focusexp_ms : 100;

  X = ccA ? ccA->xpix : 512; 
  Y = ccA ? ccA->ypix : 512;
  npix = X*Y;
  if (ccB && (ccB->xpix!=X || ccB->ypix!=Y))
    throw Exception("Focus","Pixel count mismatch","constructor");

  cfgA->expose_ms = exp_msA;
  cfgA->clear_every_frame = exp_msA < 100;
  cfgA->region = CCDRegion(0, X-1, 0, Y-1);

  cfgB->expose_ms = exp_msB;
  cfgB->clear_every_frame = exp_msA < 100;
  cfgB->region = CCDRegion(0, X-1, 0, Y-1);

  camA = ccA ? CamPool::findp(ccA->serno) : 0;
  camB = ccB ? CamPool::findp(ccB->serno) : 0;

  if (wasactive)
    activate();
}

Focus::~Focus() {
  if (isActive) {
    try {
      deactivate();
    } catch(Exception const &e) {
      fprintf(stderr,
	      "Focus: Exception caught in destructor. Armageddon imminent.\n");
      e.report();
    }
  }
  delete cfgA;
  delete cfgB;
}

void Focus::setViewMode(enum FOCUSMODE vm) {
  viewMode=vm;
  newViewMode();
}

void Focus::newViewMode() {
  left->resetZoom();
  right->resetZoom();
  switch (viewMode) {
  case FM_TwoFull:
    break;
  case FM_AZoom:
    right->zoomIn();
    break;
  case FM_BZoom:
    left->zoomIn();
    break;
  case FM_TwoZoom:
    left->zoomIn();
    right->zoomIn();
    break;
  case FM_Difference:
    right->zoomIn();
    break;
  }
  autoRange();
}

void Focus::dataAvailable() {
  bool newA = false;
  bool newB = false;
  if (camA && camA->haveNewFrame()) {   
    try {
      frmA = camA->getLatestFrame();
      newA = true;
    } catch(Exception const &e) {
      dbg("  No A frame read");
    }
  }
  if (camB && camB->haveNewFrame()) {   
    try {
      frmB = camB->getLatestFrame();
      newB = true;
    } catch(Exception const &e) {
      dbg("  No B frame read");
    }
  }
  if (viewMode==FM_Difference) {
    if (newA) {
      if (isfirstA)
	hiddenA->autoRange(frmA,X,Y);
      isfirstA=false;
      hiddenA->newImage(frmA,X,Y,flipXA,flipYA);
    }
    if (newB) {
      if (isfirstB)
	hiddenB->autoRange(frmB,X,Y);
      isfirstB=false;
      hiddenB->newImage(frmB,X,Y,flipXB,flipYB);
    }
    if (newA || newB) {
      QImage imgA = hiddenA->currentImage();
      QImage imgB = hiddenB->currentImage();
      if (imgA.width()==X && imgA.height()==Y &&
	  imgB.width()==X && imgB.height()==Y) {
	// so we have good images on both sides
	QImage img=left->currentImage();
	if (img.width()!=X || img.height()!=Y)
	  img = QImage(X,Y,QImage::Format_RGB32);
	uint32_t *dst = (uint32_t *)img.bits();
	uint32_t *srcA = (uint32_t *)imgA.bits();
	uint32_t *srcB = (uint32_t *)imgB.bits();
	uint32_t mskA=0x0000ff00;
	uint32_t mskB=~mskA;
	for (int y=0; y<Y; y++)
	  for (int x=0; x<X; x++) 
	    *dst++ = (*srcA++ & mskA) | (*srcB++ & mskB);
	left->overwriteImage(img);
	right->overwriteImage(img);
      }
    }    
  } else {
    if (viewMode!=FM_BZoom && newA) {
      if (isfirstA)
	left->autoRange(frmA,X,Y);
      isfirstA=false;
      left->newImage(frmA,X,Y,flipXA,flipYA);
      if (viewMode==FM_AZoom)
	right->overwriteImage(left->currentImage());
    }
    if (viewMode!=FM_AZoom && newB) {
      if (isfirstB)
	right->autoRange(frmB,X,Y);
      isfirstB=false;
      right->newImage(frmB,X,Y,flipXB,flipYB);
      if (viewMode==FM_BZoom)
	left->overwriteImage(right->currentImage());
    }
  }
  
  if (newA)
    reportFocus(0,frmA);
  if (newB)
    reportFocus(1,frmB);
  if ((newA || newB) && frmA && frmB)
    reportAlignment();
}

void Focus::autoRange() {
  isfirstA = isfirstB = true; // so we'll auto-range at next frame.
}

void Focus::activate() {
  frmA = frmB = 0;
  //  dbg("Focus::activate");
  if (isActive)
    throw Exception("Focus","Attempt to activate while already active");

#if SELFPOLL
  if (!timer) {
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()), this,SLOT(dataAvailable()));
  }
  timer->setInterval(10);
  timer->setSingleShot(false);
#endif
  
  if (camA) 
    camA->setConfig(*cfgA);
  if (camB) 
    camB->setConfig(*cfgB);
  if (camA)
    camA->startContinuous();
  if (camB)
    camB->startContinuous();

  autoRange();

  isActive = true;
  left->show();
  right->show();
  show();
#if SELFPOLL
  timer->start();
#endif
}

void Focus::resizeEvent(class QResizeEvent *) {
  left->setGeometry(0,0,width()/2,height());
  right->setGeometry(width()/2,0,width()/2,height());
}

void Focus::deactivate() {
  Dbg() << "Focus::deactivate";
  if (!isActive) {
    fprintf(stderr,"Focus: Deactivated while not active\n");
    return;
  }

#if SELFPOLL
  timer->stop();
#endif

  try {
    if (camA)
      camA->stopContinuous();
  } catch(Exception const &) {
    fprintf(stderr, "Caught. (Focus::deactivate A)\n");
  }

  try {
    if (camB)
      camB->stopContinuous();
  } catch(Exception const &) {
    fprintf(stderr, "Caught. (Focus::deactivate B)\n");
  }

  isActive = false;
  hide();
  //  dbg("  deactivate: hidden");
  frmA = frmB = 0;
}

void Focus::contactButtons(guiRoot *gui) {
  butFocus[0] = &(gui->findButton("maintenance/focus/focusA"));
  butFocus[1] = &(gui->findButton("maintenance/focus/focusB"));
  butIntensity[0] = &(gui->findButton("maintenance/focus/intensityA"));
  butIntensity[1] = &(gui->findButton("maintenance/focus/intensityB"));
  butShiftX = &(gui->findButton("maintenance/focus/alignX"));
  butShiftY = &(gui->findButton("maintenance/focus/alignY"));
  butScale = &(gui->findButton("maintenance/focus/alignScale"));
  butRotate = &(gui->findButton("maintenance/focus/alignRotate"));
}

void Focus::reportFocus(int idx, uint16_t const *img) {
  /* My proxy for focus quality is this: over some region of the image,
     calculate
     
       d(x,y) = I(x,y) - (1/6)*(I(x+1,y) + I(x-1,y) + I(x,y-1) + I(x,y+1)
                                + (1/2)*(I(x+1,y+1) + I(x-1,y+1)  
                                         + I(x+1,y-1) + I(x-1,y-1))),

     where I(x,y) are the image values, and the region is such that
     we don't have to worry about edge effects.
     From that, we calculate:

       F = (1/C)*sqrt(sum_xy[w(I(x,y)) d(x,y)^2])

     where C is a normalization constant and w(I) is a weighting
     function. Given the SNR scales with sqrt(I), w(I) = 1/I.
     is probably optimal. In that case, we should have:

       C = sqrt(sum_xy[I(x,y)]).
  */
  /* Pick a region: 1/4 of the width and 1/4 of the height. This
     corresponds to the ZOOM area. Those areas should probably
     be changed together. */
  int x0 = X/2-X/4/2;
  int x1 = X-x0;
  int y0 = Y/2-Y/4/2;
  int y1 = Y-y0;

  double sumI=0;
  int maxI=0;
  double sumwd2=0;
  double sum1=0;
  for (int y=y0; y<y1; y++) {
    uint16_t const *row = img + X*y;
    for (int x=x0; x<x1; x++) {
      uint16_t const *pix = row + x;
      int p00 = *(pix-1-X);
      int p01 = *(pix-X);
      int p02 = *(pix+1-X);
      int p10 = *(pix-1);
      int p11 = *pix;
      int p12 = *(pix+1);
      int p20 = *(pix-1+X);
      int p21 = *(pix+X);
      int p22 = *(pix+1+X);
      double d = p11 - (p01+p21+p10+p12)/6.0 - (p00+p02+p20+p22)/12.0;
      sum1 += 1;
      sumI += p11;
      sumwd2 += (d*d)/p11;
      if (p11>maxI)
	maxI=p11;
    }
  }
  double F = 1e6*sumwd2/sumI; // big number is better
  double avg = sumI/sum1;
  double mx = maxI;
  butFocus[idx]->setValue(QString("%1").arg(F,7,'f',2));
  butIntensity[idx]->setValue(QString("%1 / %2")
			      .arg(avg,5,'f',0)
			      .arg(mx,5,'f',0));
}

void Focus::reportAlignment() {
  /* My proxy for alignment is based on estimating optimal shifts in
     4 areas: "left", "right", "top", "bottom". In each area, I calculate:

     cc(dx,dy) = sum_xy [(I(x,y)-I0)*(I'(x+dx,y+dy)-I'0)]

     where I and I' are the two images, and I0 and I'0 are their averages
     over the area of interest.
     Probably, it is best to normalize by

       N = sqrt(sum_xy [(I(x,y)-I0)^2])

     and

       N' = sqrt(sum_xy [(I'(x+dx,y+dy)-I'0)^2]).

     I will first do that for dy=0 and dx=-5..+5 and use that to find
     the optimal dx (by fitting a quadratic). Then, same for dx=0 and
     dy=-5..+5.

     Based on the dx and dy in each region, I then calculate:

       ShiftX = dx_left + dx_right + dx_top + dx_bottom
       ShiftY = dy_left + dy_right + dy_top + dy_bottom

       Scale = (dx_right - dx_left) + (dy_bottom - dy_top)
       Rotate = (dy_right - dy_left) + (dx_top - dx_bottom)

     Note that ShiftX and ShiftY are in pixels, but Scale and Rotate
     are not normalized. I don't think there is any virtue in
     normalization.
  */
  double dx[4]; // left, right, top, bottom
  dx[0] = calcdx(  X/8,     X/4,   Y/2-Y/8, Y/2+Y/8, false); // left
  dx[1] = calcdx( X-X/4,   X-X/8,  Y/2-Y/8, Y/2+Y/8, false); // right
  dx[2] = calcdx(X/2-X/8, X/2+X/8,   Y/8,     Y/4,   false); // top
  dx[3] = calcdx(X/2-X/8, X/2+X/8,  Y-Y/4,   Y-Y/8,  false); // bottom
  double dy[4]; // left, right, top, bottom
  dy[0] = calcdx(  X/8,     X/4,   Y/2-Y/8, Y/2+Y/8, true); // left
  dy[1] = calcdx( X-X/4,   X-X/8,  Y/2-Y/8, Y/2+Y/8, true); // right
  dy[2] = calcdx(X/2-X/8, X/2+X/8,   Y/8,     Y/4,   true); // top
  dy[3] = calcdx(X/2-X/8, X/2+X/8,  Y-Y/4,   Y-Y/8,  true); // bottom

  double shiftx = dx[0] + dx[1] + dx[2] + dx[3];
  double shifty = dy[0] + dy[1] + dy[2] + dy[3];
  double scale = -dx[0] + dx[1] - dy[2] + dy[3];
  double rota  =  dy[0] - dy[1] - dx[2] + dx[3];

  butShiftX->setValue(QString("%1").arg(shiftx,0,'f',3));
  butShiftY->setValue(QString("%1").arg(shifty,0,'f',3));
  butScale->setValue(QString("%1").arg(scale,0,'f',3));
  butRotate->setValue(QString("%1").arg(rota,0,'f',3));
}

double Focus::calcdx(int x0, int x1, int y0, int y1, bool ynotx) {
  double cc[11];
  
  int sgnX = flipXA ? 1 : -1;
  if (flipXB)
    sgnX = -sgnX;
  int sgnY = flipYA ? 1 : -1;
  if (flipYB)
    sgnY = -sgnY;
  int shiftfac = ynotx ? X*sgnX : sgnY;
  for (int k=0; k<11; k++) {
    int dx = k-5;
    uint16_t const *frmB1 = frmB + dx*shiftfac;
    double avgA = average(frmA, x0,x1,y0,y1, flipXA,flipYA);
    double avgB = average(frmB1,x0,x1,y0,y1, flipXB,flipYB);
    double varA = variance(frmA, x0,x1,y0,y1, avgA,  flipXA,flipYA); 
    double varB = variance(frmB1,x0,x1,y0,y1, avgB, flipXB,flipYB);
    double cov = covariance(frmA, frmB1, x0,x1,y0,y1, avgA,avgB);
    cc[k] = cov/sqrt(varA*varB);
  }
  return fitpeak(cc,5);
}

double Focus::fitpeak(double *yy, int x0) {
  /* Fits a quadratic to the data yy, which is sampled at x=-x0..+x0,
     and use it to estimate the location of the peak in yy.
     If f(x) = ax^2 + bx + c, then
     chi2 = sum_x (f(x)-y(x))^2,
     so dchi2/da = sum_x x^2 * (f(x)-y(x)) == 0
        dchi2/db = sum_x x * (f(x)-y(x)) == 0
        dchi2/dc = sum_x (f(x)-y(x)) == 0
     i.e.
       sum_x ax^4 + bx^3 + cx^2 - y(x) x^2 = 0
       sum_x ax^3 + bx^2 + cx   - y(x) x = 0
       sum_x ax^2 + bx   + cx^2 - y(x) = 0
     i.e.
       X4 a + X3 b + X2 c = YX2
       X3 a + X2 b + X1 c = YX1
       X2 a + X1 b + X0 c = YX0
     Eliminate c from second-third:
       (X0 X3 - X1 X2) a + (X0 X2 - X1 X1) b = X0 YX1 - X1 YX0
     Write that as
             p3        a +        p2       b =        pp1
     Eliminate b from that-first:
       (X3 p3 - p2 X4) a = X3 p1 - p2 YX2
     Solve for a:
       a = (X3 p1 - p2 YX2) / (X3 p3 - p2 X4)
     Find b:
       b = (p1 - p3 a) / p2
     And finally:
       x_peak = -b/2a.
  */

  /* The above scheme ought to work, but I'm having trouble with it.
     For now, I'll use the center-of-mass of yy as a proxy of the peak
     location.
  */

#if 1
  double sumy=0;
  double sumxy=0;
  for (int x=-x0; x<=x0; x++) {
    double y=yy[x+x0];
    sumy+=y;
    sumxy+=x*y;
  }
  return sumxy/sumy;
#else
  double X1=0;
  double X2=0;
  double X3=0;
  double X4=0;
  double YX0=0;
  double YX1=0;
  double YX2=0;
  int k=0;
  for (int x=-x0; x<=x0; x++) {
    X1+=x;
    double x2=x*x;
    X2+=x2;
    double x3=x2*x;
    X3+=x3;
    double x4=x2*x2;
    X4+=x4;
    double y = yy[k];
    YX0+=y;
    YX1+=y*x;
    YX2+=y*x2;
  }
  double p3 = X0*X3-X1*X2;
  double p2 = X0*X2-X1*X1;
  double p1 = X0*YX1-X1*YX0;
  double a = (X3*p1-p2*YX2) / (X3*p3-p2*X4);
  double b = (p1-p3*a) / p2;
  // should we issue a warning if a>0?
  return -b/(2*a);
#endif
}

double Focus::average(uint16_t const *img,
		      int x0, int x1, int y0, int y1,
		      bool flipx, bool flipy) {
  double sI=0;
  for (int y=y0; y<y1; y++) {
    uint16_t const *row = flipy ? (img+(Y-1-y)*X): (img+y*X);
    for (int x=x0; x<x1; x++) {
      double pix = row[flipx ? (X-x) : x];
      sI += pix;
    }
  }
  double avg = sI/(y1-y0)/(x1-x0);
  return avg;
}

double Focus::variance(uint16_t const *img,
		       int x0, int x1, int y0, int y1,
		       double avg,
		       bool flipx, bool flipy) {
  double sI2=0;
  for (int y=y0; y<y1; y++) {
    uint16_t const *row =  flipy ? (img+(Y-1-y)*X): (img+y*X);
    for (int x=x0; x<x1; x++) {
      double pix = row[flipx ? (X-x) : x] - avg;
      sI2 += pix*pix;
    }
  }
  double var = sI2/(y1-y0)/(x1-x0);
  return var;
}

double Focus::covariance(uint16_t const *imgA, uint16_t const *imgB,
			 int x0, int x1, int y0, int y1,
			 double avgA, double avgB) {
  double sI2=0;
  for (int y=y0; y<y1; y++) {
    uint16_t const *rowA = flipYA ? (imgA+(Y-1-y)*X): (imgA+y*X);
    uint16_t const *rowB = flipYB ? (imgB+(Y-1-y)*X): (imgB+y*X);
    for (int x=x0; x<x1; x++) {
      double pixA = rowA[flipXA ? (X-x) : x] - avgA;
      double pixB = rowB[flipXB ? (X-x) : x] - avgB;
      sI2 += pixA*pixB;
    }
  }
  double var = sI2/(y1-y0)/(x1-x0);
  return var;
}
