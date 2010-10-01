// vp_flashsq.cpp

#include "vp_flashsq.h"
#include <math.h>
#include <base/dbg.h>

VP_FlashSq::VP_FlashSq() {
  period_s = 1;
  duty_frc = 0.5;
}

bool VP_FlashSq::setpar(unsigned int parno, double value) {
  switch (parno) {
  case 1:
    period_s = value;
    break;
  case 2:
    duty_frc = value/100.0;
    break;
  }
  return true;
}

QString VP_FlashSq::parsuggest(unsigned int parno) {
  switch (parno) {
  case 1:
    return "0.5:1:1.5:2:3:5";
  case 2:
    return "10:20:30:40:50:60:70:80:90";
  }
  return "";
}

void VP_FlashSq::render(VPRenderInfo const &info) const {
  dbg("flashsq: render");
  VPlugin::render(info);
  
  double *frmx = info.destx+info.i0*info.nchans;
  double *frmy = info.desty+info.i0*info.nchans;
  double t_s = 0;
  for (int f=0; f<info.nframes; f++) {
    dbg("flashsq f=%i frmx=%p frmy=%p t_s=%g period_s=%g duty=%g",
	f,frmx,frmy,t_s,period_s,duty_frc);
    if (fmod(t_s,period_s)<duty_frc) {
      // make a frame
      int X = 5;
      int Y = info.nfill/X;
      double y0 = (Y-1)/2.;
      double x0 = (X-1)/2.;
      bool flip=0;
      double *ptrx = frmx;
      double *ptry = frmy;
      for (int y=0; y<Y; y++) {
	double y_ = yscale*(y-y0)/y0;
	for (int x=0; x<X; x++) {
	  double x_ = (flip?1:-1)*xscale*(x-x0)/x0;
	  *ptrx = x_;
	  *ptry = y_;
	  ptrx+=info.nchans;
	  ptry+=info.nchans;
	}
	flip=!flip;
      }
    }
    frmx+=info.frameival*info.nchans;
    frmy+=info.frameival*info.nchans;
    t_s += info.frameival/info.fs_hz;
  }
}
