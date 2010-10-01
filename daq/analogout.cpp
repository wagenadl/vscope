// analogout.cpp

#include <daq/analogout.h>
#include <daq/analogin.h>
#include <daq/digitalout.h>
#include <base/analogdata.h>
#include <daq/dwNIDAQmx.h>
#include <base/dbg.h>

AnalogOut::AnalogOut(AnalogIn *master, QString id): daqTask(id),
						    master(master) {
  timeout = 0;
  doslave = 0;
  data = 0;
  taskIsOutput = true;
  isCont = false;
  device().demand(); // just to make sure we have one...
  if (master)
    master->attachAO(this);
}

void AnalogOut::setMaster(AnalogIn *mstr) throw(daqException) {
  if (master)
    master->detachAO();
  master=mstr;
  if (master)
    master->attachAO(this);
}

AnalogOut::~AnalogOut() {
  if (master)
    master->detachAO();
  if (doslave)
    fprintf(stderr,"AnalogOut: Dying while DO slave still attached. Armageddon imminent.\n");
}

void AnalogOut::commit() throw(daqException) {
  if (committed)
    return;

  if (!data)
    throw daqException("AnalogOut","No data defined for production");

  dbg("AnalogOut(%p):commit",this);
  daqTask::preCommit();
  dbg("  aout:precommitted");

  for (int n=0; n<data->getNumChannels(); n++) {
    int aout = data->getChannelAtIndex(n);
    double range = AO_RANGE_V;
    char chname[64];
    sprintf(chname,"%s/ao%i",deviceID().toAscii().constData(),aout);
    daqTry(DAQmxCreateAOVoltageChan(th,chname,0,
				    -range,range,
				    DAQmx_Val_Volts,0),
	   "AnalogOut","Create AO channel");
    dbg("aout Creating channel %i: %s",n,chname);
  }

  int bufsi = isCont ? dmabufsize : data->getNumScans();

  dbg("  aout:channels created. bufsi=%i",bufsi);
  
  if (master) {
    char chname[64];
    sprintf(chname,"/%s/ai/SampleClock",deviceID().toAscii().constData());
    if (isCont)
      throw Exception("AnalogOut","Continuous output not possible as slave");
    else
      daqTry(DAQmxCfgSampClkTiming(th,chname,
				   freqhz,
				   DAQmx_Val_Rising,
				   DAQmx_Val_FiniteSamps,
				   bufsi),
	     "AnalogOut","Cannot configure finite-length production as slave");
  } else {
    if (isCont) {
      daqTry(DAQmxCfgSampClkTiming(th,"OnboardClock",
				   freqhz,
				   DAQmx_Val_Rising,
				   DAQmx_Val_ContSamps,
				   data->getNumScans()),
	     "AnalogOut","Cannot configure continuous production as master");
      daqTry(DAQmxSetWriteAttribute(th, DAQmx_Write_RegenMode,
				    DAQmx_Val_DoNotAllowRegen),
	     "AnalogOut","Cannot unset autoregeneration");
    } else {
      daqTry(DAQmxCfgSampClkTiming(th,"OnboardClock",
				   freqhz,
				   DAQmx_Val_Rising,
				   DAQmx_Val_FiniteSamps,
				   bufsi),
	     "AnalogOut","Cannot configure finite-length production as master");
    }
  }
  dbg("aout: dmabufsize=%i iscont=%i",dmabufsize,isCont);

  daqTry(DAQmxCfgOutputBuffer(th,bufsi),
	 "AnalogOut","Cannot create output buffer");

  writeData();
  dbg("  aout:datawritten");
  dbg("  bufsi=%i",bufsi);
  
  postCommit();
  dbg("  aout:postcommitted");

  if (doslave) {
    dbg("  aout:slavecommit");
    doslave->commit();
    dbg("  aout:slavecommitted");
  }

}

void AnalogOut::writeData() throw(daqException) {
  if (!data)
    throw daqException("AnalogOut","No data defined for production");

  int index=0;
  double *srcptr = data->allData();
  int nscans=data->getNumScans();
  int nchans=data->getNumChannels();

  FILE *fd = fopen("/tmp/ao.bin","w");
  fwrite(srcptr,sizeof(double),nscans*nchans,fd);
  fclose(fd);
  
  while (index<nscans) {
    int now = nscans-index;
    int32 scanswritten;
    dbg("aout: index=%i nscans=%i nchans=%i now=%i",
	index,nscans,nchans,now);
    daqTry(DAQmxWriteAnalogF64(th,now,false,timeout,
			       DAQmx_Val_GroupByScanNumber,
			       srcptr,
			       &scanswritten, 0),
	   "AnalogOut","Write data (preparation)");
    dbg("aout: index=%i nscans=%i nchans=%i now=%i scanswr=%i",
	index,nscans,nchans,now,scanswritten);
    index+=scanswritten;
    srcptr+=scanswritten*nchans;
  }
}  

void AnalogOut::uncommit() throw(daqException) {
  if (!committed)
    return;

  if (doslave)
    doslave->uncommit();
  
  daqTask::uncommit();
}
  
void AnalogOut::attachDO(DigitalOut *slave) throw(daqException) {
  uncommit();
  if (doslave && doslave!=slave)
    fprintf(stderr,"AnalogOut: Warning: Attaching a second DO slave. The first slave will not be used.\n");
  doslave = slave;
}

void AnalogOut::detachDO() throw(daqException) {
  uncommit();
  doslave = 0;
}

void AnalogOut::callbackDone(int status) {
  emit productionEnded(this, status ? false : true);
}

void AnalogOut::callbackEvery(int nscans) {
  if (isCont)
    emit dataNeeded(this,nscans);
}

void AnalogOut::start() throw(daqException) {
  dbg("AnalogOut(%p)::start",this);
  preStart();

  if (doslave)
    doslave->start();

  postStart();
  dbg("  AnalogOut::start: started");

  uInt32 bs,obs;
  daqTry(DAQmxGetBufOutputBufSize(th,&bs),"AnalogOut","Cannot read bs");
  daqTry(DAQmxGetBufOutputOnbrdBufSize(th,&obs),"AnalogOut","Cannot read obs");
  dbg("aout bs=%i obs=%i",bs,obs);
}

int AnalogOut::countScansSoFar() throw(daqException) {
  if (!committed)
    return 0;
  uInt64 cnt;
  daqTry(DAQmxGetWriteTotalSampPerChanGenerated(th, &cnt),
	 "AnalogOut","Cannot count scans");
  return cnt;
}

void AnalogOut::stop() throw(daqException) {
  dbg("analogout(%p)::stop (doslave=%p)",this,doslave);
  daqTask::preStop();
  if (doslave)
    doslave->stop();
  daqTask::postStop();
}

void AnalogOut::abort() throw(daqException) {
  daqTask::preAbort();
  if (doslave)
    doslave->abort();
  daqTask::postAbort();
}

void AnalogOut::setData(AnalogData *ad) throw(daqException) {
  data = ad;
}

void AnalogOut::setFrequency(double hz)  throw(daqException) {
  daqTask::setFrequency(hz);
  if (doslave)
    doslave->setFrequency(hz);
}

void AnalogOut::setContinuous(int bufsi) throw(daqException) {
  dmabufsize = bufsi;
  isCont = true;
}

void AnalogOut::setFiniteLength() throw(daqException) {
  isCont = false;
}

int AnalogOut::countScansAvailable() throw(daqException) {
  uInt32 cnt;
  daqTry(DAQmxGetWriteSpaceAvail(th,&cnt),
	 "AnalogOut","Cannot count scans");
  dbg("aout: space avail: %i",cnt);
  return cnt;
}

char const *AnalogOut::name() const { return "AnalogOut"; }
