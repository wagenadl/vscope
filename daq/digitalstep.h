// digitalstep.h

#ifndef DIGITALSTEP_H

#define DIGITALSTEP_H

#include <base/types.h>

class DigitalStep {
public:
  DigitalStep(QString id="");
  void setMask(uint32_t mask);
  void step(uint32_t value);
private:
  uint32_t mask;
  class DAQDevice &device;
};

#endif
