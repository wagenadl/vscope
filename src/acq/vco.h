// acq/vco.h - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

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
