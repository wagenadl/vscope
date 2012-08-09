// videolight.cpp

#include <daq/digitalstep.h>
#include <xml/enumerator.h>

#include "videolight.h"

void VideoLight::set(bool on) {
  int dline = Enumerator::find("DIGILINES")->lookup("Lamp:Video");
  DigitalStep ds;
  uint32_t one=1;
  ds.setMask(one<<dline);
  ds.step(on ? (one<<dline) : 0);
}

