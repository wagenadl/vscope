// daq/digitalin.h - This file is part of VScope.
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

// digitalin.h

#ifndef DIGITALIN_H

#define DIGITALIN_H

#include <daq/daqtask.h>
#include <base/digitaldata.h>
#include <base/types.h>

class DigitalIn: public daqTask {
  Q_OBJECT;
signals:
  void dataAvailable(DigitalIn *src, int nscans);
  void acquisitionEnded(DigitalIn *src, bool ok);
public:  
  DigitalIn(class AnalogIn *master, QString id="") /*throw(daqException)*/;
  virtual ~DigitalIn();
  void setAcqLength(int nscans); // set to zero for undefined length (cont. acq.)
  int getAcqLength() const;
  virtual void commit() /*throw(daqException)*/;
  virtual int countScansSoFar() /*throw(daqException)*/;
  int read(DigitalData *dest) /*throw(daqException)*/; // Returns number of scans actually read.
  /* This will wait until data available, except of course at end of acq. */
  void setChannelMask(uint32_t mask) /*throw(daqException)*/;
  void addChannel(int lineno) /*throw(daqException)*/;
  void removeChannel(int lineno) /*throw(daqException)*/;
  uint32_t getChannelMask() const { return cmask; }
  bool hasChannel(int lineno) /*throw(daqException)*/;
protected:
  uint32_t cmask;
  int nscans;
  // uint32_t *buffer;
  // int bufferLength;
private:
  class AnalogIn *master;
public: // for internal use
  virtual void callbackDone(int status);
  virtual void callbackEvery(int nscans);
  virtual char const *name() const;
};

#endif
