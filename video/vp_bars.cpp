// vp_bars.cpp

#include "vp_bars.h"
#include <base/numbers.h>
#include <math.h>
#include <base/minmax.h>
#include <base/dbg.h>

VP_Bars::VP_Bars() {
  temp_freq_per_s = 1;
  spat_freq_per_1 = 2;
  direction_rad = 0;
  starting_phase_1 = 0;
}

bool VP_Bars::setpar(unsigned int parno, double value) {
  switch (parno) {
  case 1:
    temp_freq_per_s = value;
    if (temp_freq_per_s < .1)
      temp_freq_per_s = .1;
    break;
  case 2:
    spat_freq_per_1 = int(value+.5);
    if (spat_freq_per_1<1)
      spat_freq_per_1 = 1;
    break;
  case 3:
    direction_rad = value * numbers.pi/180;
    break;
  }
  return true;
}

QString VP_Bars::parsuggest(unsigned int parno) {
  switch (parno) {
  case 1:
    return "0.5:1:1.5:2:3:5:7:10";
  case 2:
    return "1:2:3:5:7:10";
  case 3:
    return "0:45:90:135:180:215:270:315";
  }
  return "";
}

void VP_Bars::render(VPRenderInfo const &info) const {
  VPlugin::render(info);
  
  double *frmx = info.destx+info.i0*info.nchans;
  double *frmy = info.desty+info.i0*info.nchans;
  int dn = info.nchans;
  uint32_t *frmd = info.destd+info.i0;

  int nlines = info.nfill/6;
  int lines_per_halfcycle = nlines/spat_freq_per_1;
  if (lines_per_halfcycle<2)
    lines_per_halfcycle = 2;
  nlines = lines_per_halfcycle * spat_freq_per_1;
  int points_per_line = info.nfill/nlines;
  int nleft = info.nfill - nlines*points_per_line;
  int ninter = nleft/spat_freq_per_1; // Yeah, I'm wasting the final
  double cs = cos(direction_rad);
  double sn = sin(direction_rad);
  double dxi_base = 2./(points_per_line-1);
  double deta_cycle = 2./spat_freq_per_1;
  int lines_per_fullcycle = 2*lines_per_halfcycle;
  double deta_line = deta_cycle/(lines_per_fullcycle);
  double t_s = 0;
  for (int f=0; f<info.nframes; f++) {
    // make a frame
    int phasectr = int((starting_phase_1+temp_freq_per_s*t_s)
		       * lines_per_fullcycle);
    //dbg("frame %i phasectr %i",f,phasectr);
    double *ptrx = frmx;
    double *ptry = frmy;
    uint32_t *ptrd = frmd;

    frmx += info.frameival*dn;
    frmy += info.frameival*dn;
    frmd += info.frameival;

    bool flip = false;
    double eta = -1;
    bool prime = false;
    for (int k=0; k<spat_freq_per_1; k++) {
      for (int l=0; l<lines_per_fullcycle; l++) {
	phasectr = phasectr % lines_per_fullcycle;
	if (phasectr<lines_per_halfcycle) {
	  double xi = flip ? 1 : -1;
	  double dxi = flip ? -dxi_base : dxi_base;
	  for (int m=0; m<points_per_line; m++) {
	    if (ptrx>=frmx) {
	      dbg("vp_bars: frame overflow i line");
	      break;
	    }
	    *ptrx = xscale*(xi*cs - eta*sn);
	    *ptry = yscale*(xi*sn + eta*cs);
	    ptrx += dn;
	    ptry += dn;
	    ptrd ++;
	    xi += dxi;
	  }
	  flip=!flip;
	  prime = true;
	} else if (prime) {
	  for (int m=0; m<ninter; m++) {
	    if (ptrx>=frmx) {
	      dbg("vp_bars: frame overflow in inter");
	      break;
	    }
	    *ptrx = ptrx[-dn]; // really, I could do better by preparing the jump
	    *ptry = ptry[-dn];
	    *ptrd &= ~ info.dmask_light;
	    ptrx += dn;
	    ptry += dn;
	    ptrd ++;
	  }
	  prime = false;
	}
	eta += deta_line;
	phasectr++;
      }
    }
    while (ptrd<frmd) 
      *ptrd++ &= ~info.dmask_light; // we may not have filled all the way to nfill, and I'm lazy, so I'll fill the lot.

    t_s += info.frameival / info.fs_hz;
  }
}
