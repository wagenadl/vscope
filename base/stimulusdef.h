// stimulusdef.h

#ifndef STIMULUSDEF_H

#define STIMULUSDEF_H

#include <base/digitaldata.h>
#include <base/enums.h>

class StimulusDef {
public:
  StimulusDef();
  void instantiateAnalog(double *data, int len, int stride, double fs_Hz) const;
  void instantiateDigital(DigitalData::DataType *data, int len, int line,
			  double fs_Hz) const;
  int neededLength(double fs_Hz) const;
public:
  double delay_ms;
  int nTrains;
  int nPulses;
  double trainPeriod_ms;
  double pulsePeriod_ms;
  double pulseDur_ms;
  double pulseDur2_ms;
  double pulseAmp_mV;
  double pulseAmp2_mV;
  PULSETYPE pulseType;
};

#endif
