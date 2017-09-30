// daq/digitalstep.cpp - This file is part of VScope.
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

// digitalstep.cpp

#include "digitalstep.h"
#include <base/dbg.h>
#include <daq/daqtask.h>
#include <daq/daqbase.h>

DigitalStep::DigitalStep(QString id): device(DAQDevice::find(id)) {
  mask = 0;
  device.demand(); // just to make sure we have one...
}

void DigitalStep::setMask(uint32_t m) {
  mask = m;
}

void DigitalStep::step(uint32_t value) {
  value &= mask;

  dbg("digitalstep: v=%i m=%i",value,mask);


  TaskHandle th=0;
  daqTry(DAQmxCreateTask("",&th),
	 "DigitalStep","Create task");
   
  uint32_t const one = 1;
  char chnames[64*32];
  bool empty=true;
  for (int c=0; c<32; c++) {
    char chname[64];
    if (mask & (one<<c)) {
      sprintf(chname,"%s/port%i/line%i",
 	      device.id().toUtf8().constData(),
	      device.dioPort(),
	      c);
      if (empty) {
	strcpy(chnames,chname);
	empty = false;
      } else {
	strcat(chnames,", ");
	strcat(chnames,chname);
      }
    }
  }
  daqTry(DAQmxCreateDOChan(th,chnames,0,
			   DAQmx_Val_ChanForAllLines),
	 "DigitalStep","Create DO channel");

  dbg("digitalstep: lines: %s",chnames);

  //   daqTry(DAQmxSetSampTimingType(th,DAQmx_Val_OnDemand),
  // 	 "DigitalStep","Cannot configure step production");
  // 
  //   daqTry(DAQmxCfgOutputBuffer(th, 0),
  // 	 "DigitalStep","Cannot disable output buffer");
  //   commit();

  int32 n;
  daqTry(DAQmxWriteRaw(th,1,true,1.0,&value,&n, NULL),
	 "DigitalStep","Cannot write data");
  dbg("digitalstep: wrote %i values",n);
  
  daqTry(DAQmxStopTask(th),
	 "DigitalStep","Cannot stop task");

  daqTry(DAQmxClearTask(th),
	 "DigitalStep","Cannot clear task");
}
  
