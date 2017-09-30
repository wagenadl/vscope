// daq/digitalin.cpp - This file is part of VScope.
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

// digitalin.cpp

#include <daq/digitalin.h>
#include <daq/analogin.h>
#include <../nidaq/NIDAQmx.h>
#include <base/dbg.h>

DigitalIn::DigitalIn(AnalogIn *master, QString id) /*throw(daqException)*/:
  daqTask(id), master(master) {
  cmask=0;
  device().demand(); // just to make sure we have one...
  if (!master)
    throw daqException("DigitalIn","Cannot work without an AnalogIn master");
  master->attachDI(this);
}

DigitalIn::~DigitalIn() {
  master->detachDI();
}

void DigitalIn::setAcqLength(int nscans_) {
  uncommit();
  nscans = nscans_;
}

void DigitalIn::commit() /*throw(daqException)*/ {
  if (committed)
    return;

  daqTask::preCommit();

  char chname[64];

  uint32_t one = 1;
  char chnames[64*32];
  bool empty=true;
  chnames[0]=0;
  for (int c=0; c<32; c++) {
    if (cmask & (one<<c)) {
      sprintf(chname,"%s/port%i/line%i",
	      deviceID().toUtf8().constData(),
	      device().dioPort(),
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

  daqTry(DAQmxCreateDIChan(th,chnames,0,
			   DAQmx_Val_ChanForAllLines),
	 "DigitalIn","Create DI channel");

  sprintf(chname,"/%s/ai/SampleClock",deviceID().toUtf8().constData());
  if (nscans) {
    dmabufsize = nscans;
    daqTry(DAQmxCfgSampClkTiming(th,chname,
				 freqhz,
				 DAQmx_Val_Rising,
				 DAQmx_Val_FiniteSamps,
				 nscans),
	   "DigitalIn","Cannot configure finite-length acquisition as slave");
  } else {
    dmabufsize = 8192*32;
    daqTry(DAQmxCfgSampClkTiming(th,chname,
				 freqhz,
				 DAQmx_Val_Rising,
				 DAQmx_Val_ContSamps,
				 8192*32),
	   "DigitalIn","Cannot configure continuous acquisition as slave");
  }
  postCommit();
}


void DigitalIn::callbackDone(int status) {
  emit acquisitionEnded(this, status ? false : true);
}

void DigitalIn::callbackEvery(int nscans) {
  if (pollPeriod>0)
    emit dataAvailable(this,nscans);
}

int DigitalIn::countScansSoFar() /*throw(daqException)*/ {
  if (!committed)
    return 0;
  uInt64 data;
  daqTry(DAQmxGetReadTotalSampPerChanAcquired(th, &data),
	 "DigitalIn","Cannot count scans");
  return data;
}
  
int DigitalIn::read(DigitalData *dest) /*throw(daqException)*/ {
  if (!committed)
    throw daqException("DigitalIn","Cannot read when not committed");

  KeyGuard guard(*dest);
  
  int maxscans = dest->getNumScans();
  uint32_t *destdata = dest->allData(guard.key());

  int donescans = 0;
  while (donescans<maxscans) {
    int maxnow = maxscans-donescans;
    int32 scansread;
    int32 bytesperscan;
    float mytimeout;
    if (timeout>0) {
      mytimeout = timeout;
    } else if (nscans) {
      mytimeout = 5.0 + nscans/freqhz;
    } else {
      mytimeout = 5.0; // for continuous acq.
    }
    daqTry(DAQmxReadRaw(th, maxnow, mytimeout,
     			(void*)destdata,maxnow*4,
			&scansread, &bytesperscan,0),
	   "DigitalIn","Read failure");
    //uInt32 *dd = (uInt32*)destdata;
    //daqTry(DAQmxReadDigitalU32(th, maxnow, mytimeout,
    //    		       DAQmx_Val_GroupByScanNumber,
    //    		       dd, maxnow,
    //    		       &scansread,0),
    //       "DigitalIn","Read failure");
    //bytesperscan=4;
    if (scansread<0)
      throw daqException("DigitalIn","Read fewer than zero scans!?");
    if (scansread==0)
      break;
    switch (bytesperscan) {
    case 4:
      break;
    case 1: {
      uInt8 *dd = (uInt8*)destdata;
      for (int i=scansread-1; i>0; --i) 
	destdata[i] = dd[i];
    } break;
    case 2: {
      uInt16 *dd = (uInt16*)destdata;
      for (int i=scansread-1; i>0; --i) 
	destdata[i] = dd[i];
    } break;
    default:
      throw daqException("DigitalIn",QString("Confusing scan size (not 4 bytes but %1)!?").arg(bytesperscan));
    }
    donescans += scansread;
    destdata += scansread;
  }
  dest->setNumScans(donescans);
  return donescans;
}

void DigitalIn::setChannelMask(uint32_t mask) /*throw(daqException)*/ {
  cmask = mask;
}

void DigitalIn::addChannel(int lineno) /*throw(daqException)*/ {
  uint32_t one = 1;
  cmask |= one<<lineno;
}

void DigitalIn::removeChannel(int lineno) /*throw(daqException)*/ {
  uint32_t one = 1;
  cmask &= ~(one<<lineno);
}

bool DigitalIn::hasChannel(int lineno) /*throw(daqException)*/ {
  uint32_t one = 1;
  return (cmask & (one<<lineno)) != 0;
}

char const *DigitalIn::name() const { return "DigitalIn"; }
