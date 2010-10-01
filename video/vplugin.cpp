// VPlugin.C

#include "vplugin.h"

#include <string.h>
#include <stdio.h>


VPlugin::VPlugin() {
}

VPlugin::~VPlugin() {
}

bool VPlugin::checkpars() {
  return true;
}

QString VPlugin::parsuggest(unsigned int) {
  return "";
}

bool VPlugin::setpar(unsigned int, double) {
  return true;
}

void VPlugin::setscale(double x, double y) {
  xscale = x;
  yscale = y;
}

void VPlugin::sethome(double x, double y) {
  xhome = x;
  yhome = y;
}

void VPlugin::render(VPRenderInfo const &info) const {
  // Set laser to home position throughout trial
  double *xptr = info.destx;
  double *yptr = info.desty;
  int stp = info.nchans;
  for (int n=0; n<info.nscans; n++) {
    *xptr = xhome;
    *yptr = yhome;
    xptr += stp;
    yptr += stp;
  }

  // Switch on light during frames only
  for (int n=0; n<info.nscans; n++)
    info.destd[n] &= ~info.dmask_light;
  for (int k=0; k<info.nframes; k++) {
    uint32_t *dptr = info.destd + info.i0 + k*info.frameival;
    for (int n=0; n<info.nfill; n++)
      dptr[n] |= info.dmask_light;
  }
}
