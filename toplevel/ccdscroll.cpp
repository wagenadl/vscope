// ccdscroll.cpp

#include "ccdscroll.h"
#include <base/dbg.h>
#include <base/ccddata.h>
#include <QObject>
#include <toplevel/globals.h>
#include <acq/trialdata.h>
#include <acq/datatrove.h>
#include <gfx/ccdimage.h>
#include <gfx/roiimages.h>

CCDScroll::CCDScroll(QWidget *parent): HScrollBar(parent) {
  t0=t1=ival=0;
  rerange(0, 1, 1);
  connect(this, SIGNAL(moved(double)),
	  this, SLOT(sliderMove(double)));
  connect(this, SIGNAL(moving(double)),
	  this, SLOT(sliderMove(double)));
}

CCDScroll::~CCDScroll() {
}

void CCDScroll::newData() {
  Dbg() << "CCDScroll::newData";
  TrialData const &trial = Globals::trove->trial();
  double t0ms = 0;
  double t1ms = 1000;
  double ival = 1000;
  bool first = true;
  foreach (QString id, trial.cameras()) {
    CCDData const *cam = trial.ccdData(id);
    double t0a = cam->getT0ms();
    double ivala = cam->getDTms();
    double t1a = t0a + ivala*cam->getNFrames();
    Dbg() << "CCDScroll " << id << ": " << t0a << " - " << t1a << ": " << ivala;
    if (first || t0a<t0ms)
      t0ms = t0a;
    if (first || t1a>t1ms)
      t1ms = t1a;
    if (first || ivala<ival)
      ival = ivala;
    
    first = false;
  }
  rerange(t0ms, t1ms, ival);
}

void CCDScroll::rerange(double t0_, double t1_, double ival_) {
  if (t0_==t0 && t1_==t1 && ival_==ival)
    return;
  t0 = t0_;
  t1 = t1_;
  ival = ival_;
  setRange(t0, t1-t0);
  setSlider(t0, ival);
  sliderMove(t0);
}

void CCDScroll::sliderMove(double t_ms) {
  TrialData const &trial = Globals::trove->trial();
  Dbg() << "CCDScroll: " << t_ms;
  foreach (QString id, trial.cameras()) {
    CCDData const *src = trial.ccdData(id);
    int N = src->getNFrames();
    if (N<=0)
      continue;
    double t0 = src->getT0ms();
    double dt = src->getDTms();
    int n = int((t_ms-t0)/dt);
    Dbg() << "   " << id << ": " << n;
    if (n<0)
      n=0;
    else if (n>=N)
      n=N-1;
    CCDImage *img = Globals::ccdw->get(id);
    img->newImage(src->frameData(n), src->getSerPix(), src->getParPix(),
		  trial.ccdPlacement(id));
  }
}  
