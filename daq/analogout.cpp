// analogout.cpp

#include <daq/analogout.h>
#include <daq/analogin.h>
#include <daq/digitalout.h>
#include <base/analogdata.h>
#include <daq/dwNIDAQmx.h>
#include <base/dbg.h>

#define AO_RANGE_V 10.0

AnalogOut::Channel::Channel(unsigned int c): chn(c) {
}

QString AnalogOut::Channel::name() const {
  return "ao" + QString::number(chn);
}


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

void AnalogOut::setMaster(AnalogIn *mstr) {
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
    Dbg() << "AnalogOut: "
	  << "Dying while DO slave still attached. Armageddon imminent.";
}

void AnalogOut::commit() {
  if (committed)
    return;

  if (!data)
    throw Exception("AnalogOut", "No data defined for production");

  if (data->getNumChannels() != channelmap.size())
    throw Exception("AnalogOut", "Channel map does not match production data");
  
  daqTask::preCommit();

  foreach (Channel c, channelmap) {
    QByteArray chname = channelName(c).toAscii();
    double range = AO_RANGE_V;
    daqTry(DAQmxCreateAOVoltageChan(th,chname.constData(),0,
				    -range,range,
				    DAQmx_Val_Volts,0),
	   "AnalogOut", "Create AO channel");
  }

  int bufsi = isCont ? dmabufsize : data->getNumScans();

  if (master) {
    QByteArray clockname = ("/" + deviceID() + "/ai/SampleClock").toAscii();
    if (isCont)
      throw Exception("AnalogOut",
		      "Continuous output not possible as slave");
    else
      daqTry(DAQmxCfgSampClkTiming(th, clockname.constData(), freqhz,
				   DAQmx_Val_Rising,
				   DAQmx_Val_FiniteSamps,
				   bufsi),
	     "AnalogOut",
	     "Cannot configure finite-length production as slave");
  } else {
    if (isCont) {
      daqTry(DAQmxCfgSampClkTiming(th, "OnboardClock",
				   freqhz,
				   DAQmx_Val_Rising,
				   DAQmx_Val_ContSamps,
				   data->getNumScans()),
	     "AnalogOut",
	     "Cannot configure continuous production as master");
      daqTry(DAQmxSetWriteAttribute(th, DAQmx_Write_RegenMode,
				    DAQmx_Val_DoNotAllowRegen),
	     "AnalogOut",
	     "Cannot unset autoregeneration");
    } else {
      daqTry(DAQmxCfgSampClkTiming(th,"OnboardClock",
				   freqhz,
				   DAQmx_Val_Rising,
				   DAQmx_Val_FiniteSamps,
				   bufsi),
	     "AnalogOut",
	     "Cannot configure finite-length production as master");
    }
  }
  daqTry(DAQmxCfgOutputBuffer(th,bufsi),
	 "AnalogOut","Cannot create output buffer");

  writeData();
  
  postCommit();

  if (doslave) {
    doslave->commit();
  }

}

void AnalogOut::writeData() {
  if (!data)
    throw Exception("AnalogOut","No data defined for production");

  int index=0;
  double const *srcptr = data->allData();
  int nscans=data->getNumScans();
  int nchans=data->getNumChannels();

  //FILE *fd = fopen("/tmp/ao.bin","w");
  //fwrite(srcptr,sizeof(double),nscans*nchans,fd);
  //fclose(fd);
  
  while (index<nscans) {
    int now = nscans-index;
    int32 scanswritten;
    daqTry(DAQmxWriteAnalogF64(th,now,false,timeout,
			       DAQmx_Val_GroupByScanNumber,
			       srcptr,
			       &scanswritten, 0),
	   "AnalogOut","Write data (preparation)");
    index+=scanswritten;
    srcptr+=scanswritten*nchans;
  }
}  

void AnalogOut::uncommit() {
  if (!committed)
    return;

  if (doslave)
    doslave->uncommit();
  
  daqTask::uncommit();
}
  
void AnalogOut::attachDO(DigitalOut *slave) {
  uncommit();
  if (doslave && doslave!=slave)
    fprintf(stderr,"AnalogOut: Warning: Attaching a second DO slave. The first slave will not be used.\n");
  doslave = slave;
}

void AnalogOut::detachDO() {
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

void AnalogOut::start() {
  preStart();

  if (doslave)
    doslave->start();

  postStart();

  uInt32 bs,obs;
  daqTry(DAQmxGetBufOutputBufSize(th,&bs),"AnalogOut","Cannot read bs");
  daqTry(DAQmxGetBufOutputOnbrdBufSize(th,&obs),"AnalogOut","Cannot read obs");
}

int AnalogOut::countScansSoFar() {
  if (!committed)
    return 0;
  uInt64 cnt;
  daqTry(DAQmxGetWriteTotalSampPerChanGenerated(th, &cnt),
	 "AnalogOut","Cannot count scans");
  return cnt;
}

void AnalogOut::stop() {
  daqTask::preStop();
  if (doslave)
    doslave->stop();
  daqTask::postStop();
}

void AnalogOut::abort() {
  daqTask::preAbort();
  if (doslave)
    doslave->abort();
  daqTask::postAbort();
}

 void AnalogOut::setData(AnalogData *ad,
			 QList<AnalogOut::Channel> const &cmap) {
  data = ad;
  channelmap = cmap;
}

void AnalogOut::setFrequency(double hz)  {
  daqTask::setFrequency(hz);
  if (doslave)
    doslave->setFrequency(hz);
}

void AnalogOut::setContinuous(int bufsi) {
  dmabufsize = bufsi;
  isCont = true;
}

void AnalogOut::setFiniteLength() {
  isCont = false;
}

int AnalogOut::countScansAvailable() {
  uInt32 cnt;
  daqTry(DAQmxGetWriteSpaceAvail(th,&cnt),
	 "AnalogOut","Cannot count scans");
  return cnt;
}

QString AnalogOut::channelName(AnalogOut::Channel const &c) const {
  return deviceID() + "/" + c.name();
}

char const *AnalogOut::name() const { return "AnalogOut"; }
