// stimulusdef.cpp

#include "stimulusdef.h"
#include <base/minmax.h>
#include <base/dbg.h>

StimulusDef::StimulusDef() {
  delay_ms = 0;
  nTrains = 0;
  nPulses = 0;
  trainPeriod_ms = 0;
  pulsePeriod_ms = 0;
  pulseDur_ms = 0;
  pulseDur2_ms = 0;
  pulseAmp_mV = 0;
  pulseAmp2_mV = 0;
  pulseType = PT_TTL;
}

void StimulusDef::instantiateAnalog(double *data, int len, int stride,
				    double fs_Hz) const {
  for (int s=0; s<len; s++)
    data[s*stride] = 0;

  int pulsePeriod_scans = roundi(pulsePeriod_ms*fs_Hz/1000);
  int pulseDur_scans = roundi(pulseDur_ms*fs_Hz/1000);
  int pulseDur2_scans = roundi(pulseDur2_ms*fs_Hz/1000);
  
  for (int itr=0; itr<nTrains; itr++) {
    int s00 = roundi((delay_ms+itr*trainPeriod_ms)*fs_Hz/1000);
    for (int ipu=0; ipu<nPulses; ipu++) {
      int s0 = s00 + ipu*pulsePeriod_scans;
      /* This way of rounding ensures that the pulse period is the same
	 for each pulse in the train, even if that makes the entire
	 train duration imperfectly rounded. */
      switch (pulseType) {
      case PT_TTL: case PT_Square: case PT_Biphasic: {
	int s1 = mini(s0+pulseDur_scans, len);
	for (int s=s0; s<s1; s++)
	  data[s*stride] = pulseAmp_mV/1000;
	if (pulseType==PT_Biphasic) {
	  int s2 = mini(s1 + pulseDur2_scans, len);
	  for (int s=s1; s<s2; s++)
	    data[s*stride] = pulseAmp2_mV/1000;
	}
      } break;
      case PT_Ramp: {
	int s1 = mini(s0 + pulseDur_scans, len);
	int ds = s1 - s0 - 1;
	if (ds<1)
	  ds = 1;
	double dV = (pulseAmp2_mV - pulseAmp_mV)/1000/ds;
	for (int s=s0; s<s1; s++) 
	  data[s*stride] = pulseAmp_mV/1000 + dV*(s-s0);
      } break;
      }
    }
  }
}
  

void StimulusDef::instantiateDigital(DigitalData::DataType *data,
				     int len, int line, double fs_Hz) const {
  DigitalData::DataType one = 1;
  DigitalData::DataType cmask = one<<line;

  int pulsePeriod_scans = roundi(pulsePeriod_ms*fs_Hz/1000);
  int pulseDur_scans = roundi(pulseDur_ms*fs_Hz/1000);
  for (int itr=0; itr<nTrains; itr++) {
    int s00 = roundi((delay_ms + itr*trainPeriod_ms)*fs_Hz/1000);
    for (int ipu=0; ipu<nPulses; ipu++) {
      int s0 = s00 + ipu*pulsePeriod_scans;
      /* This way of rounding ensures that the pulseperiod is the same
	 for each pulse in the train, even if that makes the entire
	 train duration imperfectly rounded. */
      int s1=mini(s0+pulseDur_scans, len);
      for (int s=s0; s<s1; s++)
	data[s] |= cmask;
    }
  }
}  
   
void StimulusDef::instantiateTrainReference(double *data,
                                            int len,
                                            double t0_ms, double dt_ms) const {
  for (int s=0; s<len; s++)
    data[s] = 0;

  double trainDur_ms = (nPulses-1)*pulsePeriod_ms + pulseDur_ms;
  if (pulseType==PT_Biphasic)
    trainDur_ms += pulseDur2_ms;
  int trainDur_scans = roundi(trainDur_ms/dt_ms);
  Dbg() << "StimulusDef" << t0_ms << "+" << dt_ms;
  Dbg() << "  td = " << trainDur_ms << ":" << trainDur_scans;
  
  for (int itr=0; itr<nTrains; itr++) {
    int s0 = roundi((delay_ms + itr*trainPeriod_ms - t0_ms)/dt_ms);
    int s1 = s0 + trainDur_scans;
    if (s0<0)
      s0 = 0;
    if (s1>len)
      s1 = len;
    for (int s=s0; s<s1; s++)
      data[s] = 1;
  }
}
  
void StimulusDef::instantiatePulseReference(double *data,
                                            int len,
                                            double t0_ms, double dt_ms) const {
  for (int s=0; s<len; s++)
    data[s] = 0;

  int pulseDur_scans = roundi((pulseDur_ms
			       + (pulseType==PT_Biphasic?pulseDur2_ms:0))
			      / dt_ms);
  if (pulseDur_scans==0)
    pulseDur_scans = 1;
  int pulsePeriod_scans = roundi(pulsePeriod_ms/dt_ms);

  for (int itr=0; itr<nTrains; itr++) {
    int s00 = roundi((delay_ms + itr*trainPeriod_ms - t0_ms)/dt_ms);
    for (int ipu=0; ipu<nPulses; ipu++) {
      int s0 = s00 + ipu*pulsePeriod_scans;
      int s1 = s0 + pulseDur_scans;
      if (s0<0)
	s0 = 0;
      if (s1>len)
	s1 = len;
      for (int s=s0; s<s1; s++)
	data[s] = 1;
    }
  }
}
  
