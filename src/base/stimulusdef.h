// base/stimulusdef.h - This file is part of VScope.
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
  void instantiateTrainReference(double *data, int len,
                                 double t0_ms, double dt_ms) const;
  void instantiatePulseReference(double *data, int len,
                                 double t0_ms, double dt_ms) const;
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
