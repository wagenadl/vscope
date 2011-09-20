// vp_flashcirc.cpp

#include "vp_flashcirc.h"
#include <base/numbers.h>
#include <math.h>

VP_FlashCirc::VP_FlashCirc() {
  period_s = 1;
  duty_frc = 0.5;
}

bool VP_FlashCirc::setpar(unsigned int parno, double value) {
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

QString VP_FlashCirc::parsuggest(unsigned int parno) {
  switch (parno) {
  case 1:
    return "0.2:0.3:0.5:0.7:1:1.5:2:3:5";
  case 2:
    return "5:10:20:25:30:33:40:50:60:67:70:75:80:90:100";
  }
  return "";
}

void VP_FlashCirc::render(VPRenderInfo const &info) const {
  VPlugin::render(info);
  
  double *frmx = info.destx+info.i0*info.nchans;
  double *frmy = info.desty+info.i0*info.nchans;
  uint32_t *frmd = info.destd+info.i0;
  double t_s = 0;
  for (int f=0; f<info.nframes; f++) {
    t_s = f*info.frameival/info.fs_hz+1e-6;
    if (fmod(t_s,period_s)<duty_frc*period_s) {
      // make a frame
      double phihome = atan2(yhome,xhome);
      double phi1 = phihome + numbers.pi/2; // desired starting angle
      /* We're going to draw a spiral with info.nfill points. We'll draw it
	 from the outside in, but our iterator i will be numbered 1 in the
	 middle to N at the outside.
	 To draw a circle with uniform brightness, we must set the radius
	 to r = sqrt(i/N) * scale, and phi=sqrt(i/N)*dphi, where dphi is
	 such that sqrt(1/N)*dphi = 2*pi/3 or similar.
      */
      int N = info.nfill;
      double dphi = 2*numbers.pi/3 * sqrt(N) * 3;
      double *ptrx = frmx;
      double *ptry = frmy;
      double phiN = dphi; // this is how many radians we'll turn
      double phi0=phi1-phiN; // so this is how many radians we should have at i=0.
      int dn = info.nchans;
      for (int n=0; n<N; n++) {
	double i=N-n;
	double si=sqrt(i/N);
	double xr = si*xscale;
	double yr = si*yscale;
	double phi= phi0 + si*dphi;
	*ptrx = xr*cos(phi);
	*ptry = yr*sin(phi);
	ptrx += dn;
	ptry += dn;
      }
    } else {
      // make a dark frame
      uint32_t *ptrd = frmd;
      for (int k=0; k<info.nfill; k++) 
	*ptrd++ &= ~ info.dmask_light;
    }
    frmx+=info.frameival*info.nchans;
    frmy+=info.frameival*info.nchans;
    frmd+=info.frameival;
  }
}
