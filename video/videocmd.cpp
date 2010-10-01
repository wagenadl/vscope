// videocmd.cpp

#include "videocmd.h"

#include <base/exception.h>
#include <base/dbg.h>

#include <base/analogdata.h>
#include <base/digitaldata.h>

#include <daq/analogout.h>
#include <daq/digitalout.h>
 
// #define wakeUpPeriod_scans    6
// #define bitOnePeriod_scans   24
// #define bitZeroPeriod_scans  15


VideoCmd::VideoCmd(int dline): dline(dline) {
  dbg("VideoCmd::VideoCmd dline=%i",dline);
  nBytes=0;
  adata=0;
  ddata=0;
}

VideoCmd::~VideoCmd() {
  dbg("VideoCmd::~VideoCmd");
  if (adata)
    delete adata;
  if (ddata)
    delete ddata;
}

int VideoCmd::overheadScans() {
  return STARTSCANS*2*STARTWAKEUPS;
}

int VideoCmd::byteScans() {
  return STARTSCANS*2*INTERWAKEUPS + BITMAXSCANS*2*8;
}

void VideoCmd::reset(int nb) {
  dbg("VideoCmd::reset nb=%i",nb);
  
  if (nb!=nBytes) {
    delete adata;
    adata=0;
    delete ddata;
    ddata=0;
  }

  nBytes = nb;
  nscans = nb*byteScans() + overheadScans() + 64;
  nscans = (nscans & (~63)) + 64; // some amount of padding
  
  adata = new AnalogData(nscans, 1);
  adata->defineChannel(0,0);
  double *ad = adata->allData();
  for (int n=0; n<nscans; n++)
    *ad++=0;
  
  ddata = new DigitalData(nscans);
  uint32_t *dd = ddata->allData();
  uint32_t one=1; one<<=dline;
  
  doffset=0;
  for (int n=0; n<nscans; n++)
    dd[doffset++]=0;

  doffset=0;
  for (int n=0; n<STARTWAKEUPS; n++) {
    for (int k=0; k<STARTSCANS; k++) 
      dd[doffset++]=one;
    doffset += STARTSCANS;
  }

  checksum=0;

  dbg("  VideoCmd::reset nBytes=%i nscans=%i doffset=%i",
      nBytes,nscans,doffset);
}

void VideoCmd::addByte(unsigned char value) {
  dbg("VideoCmd::addByte v=%i doffset=%i checksum=%i",value,doffset,checksum);
  if (doffset+byteScans() > nscans)
    throw Exception("VideoCmd","Output buffer overflow","addByte");
  checksum = 0x17b*checksum + value + (checksum>>11);
  uint32_t *dd = ddata->allData();
  uint32_t one=1; one<<=dline;

  for (int n=0; n<8; n++) {
    if (value & 1) {
      for (int k=0; k<ONESCANS; k++)
	dd[doffset++] = one;
      doffset += ONESCANS;
    } else {
      for (int k=0; k<ZEROSCANS; k++)
	dd[doffset++] = one;
       doffset += ZEROSCANS;
    }
    value >>= 1;
  }
  for (int n=0; n<INTERWAKEUPS; n++) {
    for (int k=0; k<STARTSCANS; k++)
      dd[doffset++] = one;
    doffset += STARTSCANS;
  }
  dbg("  VideoCmd::addByte -> doffset=%i nscans=%i checksum=%i",doffset,nscans,checksum);
  if (doffset>nscans)
    throw Exception("VideoCmd","Output buffer overflow","addByte");
}

void VideoCmd::addBytes(unsigned char *values, int cnt) {
  while (cnt-->0)
    addByte(*values++);
}

void VideoCmd::addChecksum() {
  addByte(checksum);
}

void VideoCmd::sendMessage(unsigned char *data, int length) {
  createMessage(length);
  addBytes(data,length);
  checkAndSend();
}

void VideoCmd::createMessage(int length) {
  reset(length+2);
  addByte(length+2);
}

void VideoCmd::checkAndSend() {
  dbg("VideoCmd::checkAndSend");
  addChecksum();
  send();
}


void VideoCmd::send() {
  dbg("VideoCmd::send");
  AnalogOut aout;
  DigitalOut dout(&aout);

  aout.setData(adata);
  aout.setFiniteLength();
  aout.setFrequency(FREQHZ);
  
  dout.setData(ddata);
  dout.addChannel(dline);

  dbg("  VideoCmd::send: starting");
  aout.start();
  dbg("  VideoCmd::send: started");
  
  double maxWait_ms = 10*nBytes+20;
  int sofar;
  while ((sofar=aout.countScansSoFar())<nscans && maxWait_ms>0) {
    dbg("  VideoCmd::send: waiting: done %i of %i, maxWait=%f ms",
	sofar,nscans,maxWait_ms);
    usleep(1000);
    maxWait_ms -= 1.0;
  }
  dbg("  VideoCmd::send: stopping");
  aout.stop();
  sofar = aout.countScansSoFar();
  
  dbg("  VideoCmd::send: alldone. final count: %i of %i",sofar,nscans);

  if (sofar!=nscans)
    throw Exception("VideoCmd","Transmission failed (timeout)");
}
