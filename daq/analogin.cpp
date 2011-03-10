// analogin.cpp

#include <daq/analogin.h>
#include <daq/digitalin.h>
#include <daq/analogout.h>
#include <daq/dwNIDAQmx.h>
#include <math.h>
#include <base/dbg.h>


AnalogIn::Channel::Channel(unsigned int c): chn(c) {
}

QString AnalogIn::Channel::name() const {
  return "ai" + QString::number(chn);
}

AnalogIn::AnalogIn(QString id) throw(daqException): daqTask(id) {
  dislave = 0;
  aoslave = 0;
  aborting = false;
  device().demand(); // just to make sure we have a real one...
}

AnalogIn::~AnalogIn() {
  if (dislave)
    fprintf(stderr,"AnalogIn: Dying while DI slave still attached. Armageddon imminent.\n");
  if (aoslave)
    fprintf(stderr,"AnalogIn: Dying while AO slave still attached. Armageddon imminent.\n");
}

void AnalogIn::removeChannel(int ain) throw (daqException) {
  uncommit();
  if (ai_channel2index.contains(ain)) {
    int idx = ai_channel2index[ain];
    ai_channel2index.remove(ain);
    ai_range_map.remove(ain);
    ai_ground_map.remove(ain);
    ai_channel_list.remove(idx);
  }
}

void AnalogIn::clearChannels() throw (daqException) {
  uncommit();
  ai_channel_list.clear();
  ai_channel2index.clear();
  ai_range_map.clear();
  ai_ground_map.clear();
}

void AnalogIn::addChannel(int ain) throw(daqException) {
  uncommit();
  
  if (ain<0)
    throw daqException("AnalogIn", "Bad AI channel number");
  int index = ai_channel_list.size();
  ai_channel_list.push_back(ain);
  ai_range_map[ain] = 1.0; // default assume 1 V
  ai_ground_map[ain] = NRSE; // default assume NRSE grounding
  ai_channel2index[ain] = index;
}

void AnalogIn::setRange(int ain, double range_V) {
  uncommit();
  ai_range_map[ain] = range_V;
}

double AnalogIn::getRange(int ain) throw(daqException) {
  if (ai_range_map.contains(ain))
    return ai_range_map[ain];
  throw daqException("AnalogIn","Channel not in task","getRange");
}

void AnalogIn::setGround(int ain, enum Ground ground) {
  uncommit();
  ai_ground_map[ain] = ground;
}

enum AnalogIn::Ground AnalogIn::getGround(int ain) throw(daqException) {
  if (ai_ground_map.contains(ain))
    return ai_ground_map[ain];
  throw daqException("AnalogIn","Channel not in task","getGround");
}

int AnalogIn::getChannelAt(int idx) const throw(daqException) {
  if (idx>=0 && idx<ai_channel_list.size())
    return ai_channel_list[idx];
  throw daqException("AnalogIn","Channel not in task","getChannelAt");
}

void AnalogIn::setAcqLength(int nscans_) {
  uncommit();
  nscans = nscans_;
}

void AnalogIn::commit() throw(daqException) {
  //  dbg("analogin(%p)::commit\n",this);
  if (committed)
    return;

  daqTask::preCommit();
  //  dbg("analogin::precommit ok\n");

  for (int n=0; n<ai_channel_list.size(); n++) {
    Channel ain = ai_channel_list[n];
    enum Ground ground = ai_ground_map[ain];
    double range = ai_range_map[ain];
    QByteArray chname = channelName(ain).toAscii();
    daqTry(DAQmxCreateAIVoltageChan(th,chname.constData(),"",
				    ground==NRSE ? DAQmx_Val_NRSE :
				    ground==RSE ? DAQmx_Val_RSE :
				    ground==DIFF ? DAQmx_Val_Diff :
				    DAQmx_Val_Cfg_Default,
				    -range,range,
				    DAQmx_Val_Volts,0),
	   "AnalogIn","Create AI channel");
    //dbg("analogin: created %i: %i: '%s'\n",n,ain,chname);
  }
  //  dbg("analogin: voltage channels created\n");

  if (nscans) {
    dmabufsize = nscans;
    daqTry(DAQmxCfgSampClkTiming(th,"OnboardClock",
                                 freqhz,
                                 DAQmx_Val_Rising,
                                 DAQmx_Val_FiniteSamps,
                                 nscans),
           "AnalogIn","Cannot configure finite-length acquisition");
  } else {
    dmabufsize = 8192*32;
    daqTry(DAQmxCfgSampClkTiming(th,"OnboardClock",
                                 freqhz,
                                 DAQmx_Val_Rising,
                                 DAQmx_Val_ContSamps,
                                 dmabufsize),
           "AnalogIn","Cannot configure continuous acquisition");
  }
  dbg("analogin(%p): sampclktiming set. dmabufsize=%i nscans=%i freqhz=%g\n",
      this,dmabufsize,nscans,freqhz);

  if (dislave) 
    dislave->commit();

  if (aoslave) 
    aoslave->commit();

  postCommit();
  //dbg("analogin: postcommit ok\n");
}

void AnalogIn::uncommit() throw(daqException) {
  if (!committed)
    return;

  if (dislave) 
    dislave->uncommit();

  if (aoslave) 
    aoslave->uncommit();

  daqTask::uncommit();
}

void AnalogIn::attachDI(DigitalIn *digin) throw(daqException) {
  uncommit();
  if (dislave && dislave!=digin)
    fprintf(stderr,"AnalogIn: Warning: Attaching a second DI slave. The first slave will not be used.\n");
  dislave = digin;
}

void AnalogIn::attachAO(AnalogOut *analin) throw(daqException) {
  uncommit();
  if (aoslave && aoslave!=analin)
    fprintf(stderr,"AnalogIn: Warning: Attaching a second AO slave. The first slave will not be used.\n");
  aoslave = analin;
}

void AnalogIn::detachDI() throw(daqException) {
  uncommit();
  dislave = 0;
}

void AnalogIn::detachAO() throw(daqException) {
  uncommit();
  aoslave = 0;
}

void AnalogIn::callbackDone(int status) {
  emit acquisitionEnded(this, status ? false : true);
}

void AnalogIn::callbackEvery(int nscans) {
  // dbg("AnalogIn::callbackEvery nscans=%i pollperiod=%i\n",
  //    nscans,pollPeriod);
  if (pollPeriod>0)
    emit dataAvailable(this,nscans);
}

void AnalogIn::start() throw(daqException) {
  dbg("AnalogIn(%p)::start",this);
  preStart();

  if (dislave)
    dislave->start();
  if (aoslave)
    aoslave->start();

  postStart();
}

int AnalogIn::countScansSoFar() throw(daqException) {
  if (!committed)
    return 0;
  uInt64 data;
  daqTry(DAQmxGetReadTotalSampPerChanAcquired(th, &data),
	 "AnalogIn","Cannot count scans");
  return data;
}
  

int AnalogIn::read(AnalogData *dest) {
  KeyGuard guard(*dest);
  int donescans = read(dest,0,0);
  dest->setNumScans(donescans);
  return donescans;
}

int AnalogIn::read(AnalogData *dest, int offset, int maxscans) {
  if (!committed)
    throw Exception("AnalogIn","Cannot read when not committed");
  int nchans = dest->getNumChannels();
  if (nchans != ai_channel_list.size())
    throw Exception("AnalogIn",
		    "Cannot read into structure with wrong channel count");
  for (int n=0; n<nchans; n++)
    if (dest->getChannelAtIndex(n) != ai_channel_list[n])
      throw Exception("AnalogIn",
		      "Cannot read into structure with wrong channel map");

  int nscans = dest->getNumScans();

  if (maxscans==0)
    maxscans=nscans;
  if (maxscans>nscans-offset)
    maxscans=nscans-offset;

  KeyGuard guard(*dest);
  
  double *destdata = dest->allData(guard.key());
  destdata += nchans*offset;

  int donescans = 0;
  while (donescans<maxscans) {
    int maxnow = maxscans-donescans;
    int32 scansread;
    double mytimeout;
    if (timeout>0) {
      mytimeout = timeout;
    } else if (nscans) {
      mytimeout = 5.0 + nscans/freqhz;
    } else {
      mytimeout = 5.0; // for continuous acq.
    }
    daqTry(DAQmxReadAnalogF64(th, maxnow, mytimeout,
			      DAQmx_Val_GroupByScanNumber,
			      destdata,maxnow*nchans,
			      &scansread, 0),
	   "AnalogIn","Read failure");
    if (scansread<0)
      throw Exception("AnalogIn", "Read fewer than zero scans!?");
    if (scansread==0)
      break;

    for (int c=0; c<nchans; c++) {
      double sum=0;
      double sumsum=0;
      double min=1e6, max=-1e6;
      for (int i=0; i<scansread; i++) {
	double v = destdata[c+nchans*i];
	if (v<min)
	  min=v;
	if (v>max)
	  max=v;
	sum+=v;
	sumsum+=v*v;
      }
      // fprintf(stderr,"  channel %i: mean=%g rms=%g min=%g max=%g\n",c,sum/scansread,sqrt(sumsum/scansread),min,max);
    }
    donescans += scansread;
    destdata += scansread*nchans;
  }
  return donescans;
}

void AnalogIn::stop() throw(daqException) {
  dbg("analogin(%p)::stop (aoslave=%p, dislave=%p)",this,aoslave,dislave);
  daqTask::preStop();
  if (!aborting) {
    if (aoslave)
      aoslave->stop();
    if (dislave)
      dislave->stop();
  }
  daqTask::postStop();
}

void AnalogIn::abort() throw(daqException) {
  aborting = true;
  dbg("analogin(%p)::abort",this);
  try {
    daqTask::preAbort();
    if (aoslave)
      aoslave->abort();
    if (dislave)
      dislave->abort();
    daqTask::postAbort();
    dbg("  analogin(%p)::abort done",this);
    aborting = false;
  } catch (...) {
    aborting = false;
    fprintf(stderr,"AnalogIn: caught exception while aborting.\n");
    throw;
  }
}

void AnalogIn::setFrequency(double hz)  throw(daqException) {
  daqTask::setFrequency(hz);
  if (aoslave)
    aoslave->setFrequency(hz);
  if (dislave)
    dislave->setFrequency(hz);
}

int AnalogIn::getNumChannels() const {
  return ai_channel_list.size();
}

QString AnalogIn::channelName(AnalogIn::Channel const &c) const {
  return deviceID() + "/" + ain.name();
}

char const *AnalogIn::name() const { return "AnalogIn"; }
