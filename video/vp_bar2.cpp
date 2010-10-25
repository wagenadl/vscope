// vp_bar2.cpp

#include "vp_bar2.h"
#include <base/numbers.h>
#include <math.h>
#include <base/minmax.h>
#include <base/dbg.h>

VP_Bar2::VP_Bar2() {
  period_s = 1;
  speed_fields_per_s = 1;
  direction_rad = 0;
  t0_s = 0;
  width_fields = 0.1;
}

bool VP_Bar2::setpar(unsigned int parno, double value) {
  switch (parno) {
  case 1:
    period_s = value;
    break;
  case 2:
    speed_fields_per_s = value;
    break;
  case 3:
    direction_rad = (value-10) * numbers.pi/180;
    break;
  }
  return true;
}

QString VP_Bar2::parsuggest(unsigned int parno) {
  switch (parno) {
  case 1:
    return "0.1:0.2:0.3:0.5:0.7:1:1.5:2";
  case 2:
    return "1:2:3:5:7:10:15:20:30:50";
  case 3:
    return "0:45:90:135:180:215:270:315";
  }
  return "";
}

void VP_Bar2::render(VPRenderInfo const &info) const {
  VPlugin::render(info);
  
  double *ptrx = info.destx+info.i0*info.nchans;
  double *ptry = info.desty+info.i0*info.nchans;
  int dn = info.nchans;
  uint32_t *ptrd = info.destd+info.i0;

  int points_per_line = 6;
  int npoints = info.frameival*(info.nframes-1) + info.nfill;
  int nlines = npoints/points_per_line;
  double cs = cos(direction_rad);
  double sn = sin(direction_rad);
  uint32_t *endd = ptrd + npoints;

  double minx=0, maxx=0, miny=0, maxy=0;
  bool flip = false;
  for (int n=0; n<nlines; n++) {
    for (int k=0; k<points_per_line; k++) {
      double t_s = (n*points_per_line+k)/info.fs_hz;
      double dt_s = fmod(t_s-t0_s, period_s);
      double eta0 = dt_s*speed_fields_per_s;
      // let's draw a line
      double eta = -1+2*eta0;
      if (eta>1)
	eta=1;
      double xi = 1-2.*k/points_per_line;
      //      Dbg() << t_s << ";" << dt_s << ";" << eta0 << ";" << eta << ";" << xi;
      if (flip)
	xi=-xi;
      double x = xscale*(xi*cs + eta*sn);
      double y = yscale*(xi*sn - eta*cs);
      *ptrx = x;
      *ptry = y;
      if (x<minx)
	minx=x;
      if (x>maxx)
	maxx=x;
      if (y<miny)
	miny=y;
      if (y>maxy)
	maxy=y;
      if (eta0<1)
	*ptrd |= info.dmask_light;
      else
	*ptrd &= ~info.dmask_light;
      ptrx += dn;
      ptry += dn;
      ptrd ++;
    }
    flip=!flip;
  }
  while (ptrd<endd) {
    *ptrd &= ~info.dmask_light;
    ptrd++;
  }
  //  Dbg() << "VP_BAR2: x: " << minx << " " << maxx << "; y: " << miny << " " << maxy;
}
