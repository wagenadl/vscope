// daqtask.cpp

#include <daq/daqtask.h>
#include <daq/dwNIDAQmx.h>
#include <base/dbg.h>

daqTask::daqTask(QString id) throw(daqException): dev(DAQDevice::find(id)) {
  pollPeriod = 0;
  dmabufsize = 0;
  timeout = 0;
  freqhz = 10000;
  committed = false;
  startTrig = false;
  taskIsOutput = false;
}

daqTask::~daqTask() {
  if (committed)
    fprintf(stderr,"daqTask destructed while still committed. Armageddon imminent.\n");
}

void daqTask::setTimeout(float secs) throw(daqException) {
  timeout = secs;
}

void daqTask::setPollPeriod(int nscans) throw(daqException) {
  pollPeriod = nscans;
}

void daqTask::setFrequency(double freq_hz) throw(daqException) {
  uncommit();
  freqhz = freq_hz;
}

void daqTask::commit() throw(daqException) {
  dbg("daqtask(%p/%s)::commit",this,name());
  if (committed)
    return;
  preCommit();
  postCommit();
}

void daqTask::preCommit() throw(daqException) {
  dbg("daqtask(%p/%s)::precommit sizeof(th)=%li\n",this,name(),sizeof(th));
  th=(TaskHandle)(-1);
  daqTry(DAQmxCreateTask("", &th),"daqTask","Task creation failed");
  dbg("  daqtask precommit: task created: %i\n",int(th));
}

void daqTask::postCommit() throw(daqException) {
  dbg("daqtask(%p/%s)::postcommit\n",this,name());
  if (startTrig) {
    char chname[64];
    sprintf(chname,"/%s/PFI0",deviceID().toAscii().constData());
    daqTry(DAQmxCfgDigEdgeStartTrig(th,chname,DAQmx_Val_Rising),
	   "daqTask","Set digital trigger");
    dbg("postcommit: trigger enabled\n");
  } else {
    dbg("postcommit: no trigger\n");
  }
  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Verify),"daqTask","Cannot verify task");
  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Reserve),"daqTask","Cannot reserve task resources");
  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Commit),"daqTask","Cannot commit task");
  dbg("  daqtask: postcommit: done");
  committed = true;
}

void daqTask::uncommit() throw(daqException) {
  dbg("daqtask(%p/%s)::uncommit",this,name());
  // daqTry(DAQmxStopTask(th),"daqTask","Cannot stop task"); // should we do this?

  daqTry(DAQmxClearTask(th),"daqTask","Cannot clear task");
  committed = false;
}

int32 CVICALLBACK daqTask_CallbackDone(TaskHandle, int32 status, void *callbackData) {
  daqTask *me = (daqTask *)callbackData;
  me->callbackDone(status);
  return 0;
}

void daqTask::callbackDone(int status) {
  try {
    daqTry(status,"SnalogIn","Acquisition ended with error");
  } catch (...) {
  }
}

int32 CVICALLBACK daqTask_CallbackEvery(TaskHandle, int32 /* type */, uInt32 nscans, void *callbackData) {
  // dbg("daqTask_CallbackEvery %p %i\n",callbackData,nscans);
  daqTask *me = (daqTask *)callbackData;
  me->callbackEvery(nscans);
  return 0;
}

void daqTask::callbackEvery(int) {
  // dbg("daqTask::callbackEvery\n");
}

void daqTask::preStart() throw(daqException) {
  dbg("daqTask(%p/%s):prestart %i",this,name(), th);
  commit();
  
  daqTry(DAQmxRegisterDoneEvent(th,0,
				&daqTask_CallbackDone,(void*)this),
	 "daqTask","Cannot register Done event");
  dbg("daqTask::prestart: pollperiod=%i dmabufsize=%i\n",
      pollPeriod,dmabufsize);
  if (pollPeriod>0) {
    int pollp = pollPeriod;
    int bufsi = dmabufsize;
    if (bufsi>1) {
      while (pollp>0) {
	int n = bufsi/pollp;
	if (n*pollp==bufsi)
	  break;
	pollp-=1;
      }
    } else {
      fprintf(stderr,"Warning: daqTask: No DMA Buffer size defined; pollPeriod may be illegal. Expect trouble from NIDAQ.\n");
    }
    daqTry(DAQmxRegisterEveryNSamplesEvent(th,
			       taskIsOutput
				 ? DAQmx_Val_Transferred_From_Buffer
				 : DAQmx_Val_Acquired_Into_Buffer,
			       pollp,0,
			       &daqTask_CallbackEvery,(void*)this),
	   "daqTask","Cannot register EveryN event");
      dbg("  daqtask::prestart: everyn registered pollp=%i",pollp);
  }
  dbg("  daqtask:prestart: done");
}

void daqTask::postStart() throw(daqException) {
  dbg("daqTask(%p/%s)::poststart %i",this,name(),th);
  daqTry(DAQmxStartTask(th),"daqTask","Cannot start task");
  dbg("  daqtask:poststart: started %i", th);
}

void daqTask::start() throw(daqException) {
  dbg("daqTask(%p/%s)::start",this,name());
  preStart();
  postStart();
  dbg("daqTask(%p/%s)::start ok",this,name());
}

bool daqTask::isRunning() throw(daqException) {
  if (!committed)
    return false;
  if (hasCompleted())
    return false;
  fprintf(stderr,"Warning: daqTask: I think we're running, because we're committed and not complete. I'm not sure if that's solid reasoning.\n");
  return true;
}

int daqTask::countScansAvailable() throw(daqException) {
  if (!committed)
    return 0;
  uInt32 cnt;
  daqTry(DAQmxGetReadAvailSampPerChan(th, &cnt),
	 "daqTask","Cannot count scans");
  return cnt;
}

bool daqTask::hasCompleted() throw(daqException) {
  if (!committed)
    return false;
  bool32 data;
  daqTry(DAQmxIsTaskDone(th,&data),
	 "daqTask","Cannot ascertain completion");
  return data;
}

int daqTask::countScansSoFar() throw(daqException) {
  throw daqException("daqTask","I don't know how to count scans abstractly");
}

void daqTask::setTriggering(bool trg) throw(daqException) {
  startTrig = trg;
}

void daqTask::preStop() throw(daqException) {
  dbg("daqtask(%p/%s)::prestop %i",this,name(), th);
  daqTry(DAQmxStopTask(th),"daqTask","Cannot stop task");
}

void daqTask::postStop() throw(daqException) {
  dbg("daqtask(%p/%s)::poststop %i",this,name(), th);
}

void daqTask::stop() throw(daqException) {
  dbg("daqtask(%p/%s)::stop",this,name());
  preStop();
  postStop();
  dbg("daqtask(%p/%s)::stop ok",this,name());
}

void daqTask::preAbort() throw(daqException) {
  dbg("daqtask(%p/%s)::preabort %i",this,name(), th);
  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Abort),"daqTask","Cannot abort task");
}

void daqTask::postAbort() throw(daqException) {
  dbg("daqtask(%p/%s)::postabort %i",this,name(), th);
  stop();
  uncommit();
}

void daqTask::abort() throw(daqException) {
  dbg("daqtask(%p/%s)::abort",this,name());
  preAbort();
  postAbort();
  dbg("  daqtask(%p/%s)::abort ok",this,name());
}

char const *daqTask::name() const { return "daqTask"; }
