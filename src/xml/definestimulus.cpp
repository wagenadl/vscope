// xml/definestimulus.cpp - This file is part of VScope.
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
 
