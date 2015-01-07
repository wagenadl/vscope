// definestimulus.cpp

#include "definestimulus.h"
#include <base/dbg.h>

StimulusDef defineStimulus(ParamTree const *ptree, QString chnid) {
  QString path = QString("stimEphys/channel:%1/").arg(chnid);
  StimulusDef s;
  if (!ptree->findp(path+"delay")) {
    Dbg() << "defineStimulus: Stimulus channel " << chnid << " not found";
    return s; // stimulus not found
  }
  s.delay_ms = ptree->find(path+"delay").toDouble();
  s.nTrains = ptree->find(path+"nTrains").toInt();
  s.trainPeriod_ms = ptree->find(path+"trainPeriod").toDouble();
  s.nPulses = ptree->find(path+"nPulses").toInt();
  s.pulsePeriod_ms = ptree->find(path+"pulsePeriod").toDouble();
  s.pulseDur_ms = ptree->find(path+"pulseDur").toDouble();
  s.pulseDur2_ms = ptree->find(path+"pulseDur2").toDouble();
  s.pulseAmp_mV = ptree->find(path+"pulseAmp").toDouble();
  s.pulseAmp2_mV = ptree->find(path+"pulseAmp2").toDouble();
  s.pulseType = (PULSETYPE)ptree->find(path+"pulseType").toInt();
  if (s.pulseType==PT_TTL)
    s.pulseAmp_mV = 5000;
  return s;
}
 