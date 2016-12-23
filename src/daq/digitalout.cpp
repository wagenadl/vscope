// digitalout.cpp

#include <daq/digitalout.h>

#include <daq/analogout.h>
#include <base/digitaldata.h>
#include <../nidaq/NIDAQmx.h>
#include <base/dbg.h>

DigitalOut::DigitalOut(AnalogOut *master, QString id) throw(daqException):
  daqTask(id), master(master) {
  taskIsOutput = true;
  timeout = 0;
  data = 0;
  cmask = 0;
  device().demand(); // just to make sure we have one...
  if (!master)
    throw daqException("DigitalOut","Cannot work without an AnalogOut master");
  master->attachDO(this);
}

DigitalOut::~DigitalOut() {
  master->detachDO();
}

void DigitalOut::commit() throw(daqException) {
  char chname[64];

  if (committed)
    return;

  if (!data)
    throw daqException("DigitalOut","No data defined for production");
  
  daqTask::preCommit();

  uint32_t one = 1;
  char chnames[64*32];
  bool empty=true;
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
  daqTry(DAQmxCreateDOChan(th,chnames,0,
			   DAQmx_Val_ChanForAllLines),
	 "DigitalOut","Create DO channel");
  uInt32 nli = device().nDIOLines();
//  sprintf(chname,"%s/port%i",
//	  daqdev->id().toUtf8().constData(),
//	  daqdev->dioPort());
//  daqTry(DAQmxGetDONumLines(th,chnames,&nli),"DigitalOut","Count DO lines");
  Dbg() << "DigitalOut: nlines: " << nli;

  sprintf(chname,"/%s/ao/SampleClock",deviceID().toUtf8().constData());
  daqTry(DAQmxCfgSampClkTiming(th,chname,
			       freqhz,
			       DAQmx_Val_Rising,
			       DAQmx_Val_FiniteSamps,
			       data->getNumScans()),
	 "DigitalOut","Cannot configure finite-length production as slave");

  daqTry(DAQmxCfgOutputBuffer(th,data->getNumScans()),
	 "DigitalOut","Cannot create output buffer");
  
  postCommit();

  writeData();
}

void DigitalOut::writeData() throw(daqException) {
  if (!data)
    throw daqException("DigitalOut","No data defined for production");

  uint32_t const *srcptr = data->allData();
  int nscans=data->getNumScans();
  int32_t scanswritten;
  int nli = device().nDIOLines();
  void const *useptr = 0;
  if (nli>=17 && nli<=32) {
    useptr = (void const *)srcptr;
  } else if (nli>0 && nli<=8) {
    if (data8.size()<nscans)
      data8.resize(nscans);
    uint8_t *src8 = data8.data();
    for (int i=0; i<nscans; i++)
      src8[i] = srcptr[i];
    useptr = (void const *)src8;
  } else if (nli<=16) {
    if (data16.size()<nscans)
      data16.resize(nscans);
    uint16_t *src16 = data16.data();
    for (int i=0; i<nscans; i++)
      src16[i] = srcptr[i];
    useptr = (void const *)src16;
  } else {
    throw daqException("DigitalOut","Data has unacceptable bit depth");
  }
  daqTry(DAQmxWriteRaw(th,nscans,false,timeout,
         	       (void*)useptr,
		       &scanswritten, 0),
	 "DigitalOut","Write data (preparation)");
  // const uInt32 *sp = (const uInt32 *)srcptr;
  // daqTry(DAQmxWriteDigitalU32(th,nscans,false,timeout,
  //       		     DAQmx_Val_GroupByScanNumber,
  //       		     sp,&scanswritten,0),
  // 	 "DigitalOut","Write data (preparation)");
  if (scanswritten!=nscans)
    throw daqException("DigitalOut","Unexpected number of scans written","writeData");
}  

void DigitalOut::callbackDone(int status) {
  emit productionEnded(this, status ? false : true);
}

int DigitalOut::countScansSoFar() throw(daqException) {
  if (!committed)
    return 0;
  uInt64 cnt;
  daqTry(DAQmxGetWriteTotalSampPerChanGenerated(th, &cnt),
	 "DigitalOut","Cannot count scans");
  return cnt;
}

void DigitalOut::setData(DigitalData *dd) throw(daqException) {
  data = dd;
}

void DigitalOut::setChannelMask(uint32_t mask) throw(daqException) {
  cmask = mask;
}

void DigitalOut::addChannel(int lineno) throw(daqException) {
  uint32_t one = 1;
  cmask |= one<<lineno;
}

void DigitalOut::removeChannel(int lineno) throw(daqException) {
  uint32_t one = 1;
  cmask &= ~(one<<lineno);
}

bool DigitalOut::hasChannel(int lineno) throw(daqException) {
  uint32_t one = 1;
  return (cmask & (one<<lineno)) != 0;
}

char const *DigitalOut::name() const { return "DigitalOut"; }
