// vcplugins.h

/* To be included ONLY by videocommplugins.cpp */

#ifndef VCPLUGINS_H

#define VCPLUGINS_H

#include "vp_flashsq.h"
#include "vp_flashcirc.h"
#include "vp_bars.h"
#include "vp_bar2.h"

void VideoCommPlugins::makePlugins() {
  progs[1] = new VP_FlashSq();
  progs[2] = new VP_FlashCirc();
  progs[3] = new VP_Bars();
  progs[4] = new VP_Bar2();
}

#endif
