// acq/vco.cpp - This file is part of VScope.
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

// vco.cpp

#include "vco.h"
#include <xml/paramtree.h>
#include <base/minmax.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <daq/analogout.h>
#include <base/analogdata.h>
#include <xml/connections.h>

VCO::VCO() {
  f0_hz = 130.8128; // C below middle C is 130.8 Hz.
  dfdv_oct10mV = 0.5; // -40 mV will be C above middle C if dfdv = 0.5.
  Vpp=1;
  tau_s = 0.01;
  buflen = 2048;
  fsamp_hz = 160e3;

  Vm = 0;
  fnow_hz = f0_hz;
  phi = 0;
  isActive = false;

  data=0;
  aout=0;
  ptree=0;
}

VCO::~VCO() {
  if (aout)
    delete aout;
  if (data)
    delete data;
}

void VCO::setVoltage(double Vm_mV, double dt) {
  if (dt) {
    double a = exp(-dt/tau_s);
    Vm = a*Vm + (1-a)*Vm_mV;
  } else {
    Vm = Vm_mV;
  }
  fnow_hz = f0_hz*pow(2,-(Vm/10)*dfdv_oct10mV);
  //  dbg("VCO::setVoltage Vm_mV=%g dt=%g Vm=%g fnow=%g",Vm_mV,dt,Vm,fnow_hz);
}

void VCO::reconfig() {
  if (!ptree)
    return;

  // we should reread our parameter values
  aoc = ptree->find("acqEphys/vco/Dest").toString();
  f0_hz = ptree->find("acqEphys/vco/F0").toDouble();
  dfdv_oct10mV = ptree->find("acqEphys/vco/dfdv_oct10mV").toDouble();
  Vpp = ptree->find("acqEphys/vco/Vpp").toDouble() / 1000;
  tau_s = ptree->find("acqEphys/vco/tau").toDouble() / 1000;

  chn.clear();
  chn.append(AnalogOut::Channel(Connections::findAO(aoc).line));
  
}

void VCO::activate(ParamTree *p) {
  if (isActive)
    throw Exception("VCO","Attemp to activate while already active");

  ptree = p;
  reconfig();

  dbg("vco::activate p=%p\n",p);
  try {
    aout = new AnalogOut();
  } catch (Exception e) {
    return;
  }
    dbg("vco: ain created");
  connect(aout,SIGNAL(dataNeeded(AnalogOut*,int)),
	  this,SLOT(dataNeeded()));

  aout->setFrequency(fsamp_hz);
  aout->setTriggering(false);
  aout->setContinuous(buflen*4);
  aout->setPollPeriod(buflen);
  aout->setTimeout(1); // hmmm...

  data = new AnalogData(buflen, 1, fsamp_hz);
  data->defineChannel(0, aoc);

  phi=0;
  makeData();
  aout->setData(data, chn);
  aout->commit();
  makeData();
  aout->writeData();
  makeData();
  aout->writeData();
  makeData();
  aout->writeData();
  aout->start();

  isActive = true;
}

void VCO::deactivate() {
  if (!isActive) {
    fprintf(stderr,"VCO: Deactivated while not active\n");
    return;
  }

  isActive = false;
  aout->stop();
  aout->uncommit();

  delete aout;
  aout=0;

  delete data;
  data=0;
}

  
void VCO::makeData() {
  KeyGuard guard(*data);
  double *d = data->allData(guard.key());
  for (int k=0; k<buflen; k++) {
    *d++ = sin(2*3.14159265*phi)*Vpp;
    phi += fnow_hz/fsamp_hz;
  }
  //  dbg("VCO::makedata: phi=%g",phi);
  //while (phi>1)
  //phi-=1;
}

void VCO::dataNeeded() {
  bool first=false;
  //  dbg("VCO::dataneeded buflen=%i av=%i",buflen,aout->countScansAvailable());
  while (first || aout->countScansAvailable() >= buflen) {
    //dbg("VCO: making data");
    makeData();
    aout->setData(data, chn);
    aout->writeData();
    first = false;
  }
}
