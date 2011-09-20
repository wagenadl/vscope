// ccdscroll.cpp

#include "ccdscroll.h"
#include <base/dbg.h>
#include <base/ccddata.h>
#include <QObject>

CCDScroll::CCDScroll(QWidget *parent): HScrollBar(parent) {
  setRange(0, 1);
  setSlider(0, 1);
  connect(this, SIGNAL(moved(double)),
	  this, SLOT(sliderMove(double)));
  connect(this, SIGNAL(moving(double)),
	  this, SLOT(sliderMove(double)));
  oldpos = 0;
}

CCDScroll::~CCDScroll() {
}

void CCDScroll::setNFrames(int n) {
  Dbg() << "CCDScroll::setNFrames " << n;
  if (n<1)
    n=1;
  setRange(0, n);
  setSlider(0, 1);
  oldpos=0;
  emit gotoFrame(0);
}

void CCDScroll::sliderMove(double x0) {
  int newpos = int(x0);
  if (newpos!=oldpos) {
    oldpos = newpos;
    emit gotoFrame(newpos);
  }
}

void CCDScroll::newCCDData() {
  Dbg() << "new CCD Data";
  CCDData *src = qobject_cast<CCDData*>(sender());
  if (src)
    setNFrames(src->getNFrames());
}

