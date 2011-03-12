// vco.h

#ifndef VCO_H

#define VCO_H

#include <QObject>
#include <QString>

#include <daq/analogout.h>

class VCO: public QObject {
  Q_OBJECT;
public:
  VCO();
  virtual ~VCO();
  void activate(class ParamTree *p);
  void deactivate();
  void setVoltage(double Vm_mV, double dt_s=0);
protected slots:
  void dataNeeded();
  void reconfig();
private:
  void makeData();
private:
  double Vpp;
  double f0_hz;
  /* f0 is frequency for 0 mV. */
  double dfdv_oct10mV;
  /* dfdv is number of octave to shift per 10 mV. Positive numbers mean
     negative voltages result in higher pitch.
  */
  double tau_s;
  
  QString aoc; // channel we're using
  int buflen;

  double Vm;
  double fnow_hz;
  double fsamp_hz;
  double phi;
  bool isActive;
  
  class AnalogData *data;
  class AnalogOut *aout;
  class ParamTree *ptree;
    
  QList<AnalogOut::Channel> chn;
};

#endif
