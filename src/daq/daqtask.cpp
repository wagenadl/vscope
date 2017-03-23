// daq/daqtask.cpp - This file is part of VScope.
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

// daqtask.cpp

#include <daq/daqtask.h>
#include <../nidaq/NIDAQmx.h>
#include <base/dbg.h>

daqTask::daqTask(QString id): dev(DAQDevice::find(id)) {
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

void daqTask::setTimeout(float secs) {
  timeout = secs;
}

void daqTask::setPollPeriod(int nscans) {
  pollPeriod = nscans;
}

void daqTask::setFrequency(double freq_hz) {
  uncommit();
  freqhz = freq_hz;
}

void daqTask::commit() {
  if (committed)
    return;
  preCommit();
  postCommit();
}

void daqTask::preCommit() {
  th=(TaskHandle)(-1);
  daqTry(DAQmxCreateTask("", &th),"daqTask","Task creation failed");
}

void daqTask::postCommit() {
  if (startTrig) {
    char chname[64];
    sprintf(chname,"/%s/PFI0",deviceID().toUtf8().constData());
    daqTry(DAQmxCfgDigEdgeStartTrig(th,chname,DAQmx_Val_Rising),
	   "daqTask","Set digital trigger");
  }
  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Verify),"daqTask","Cannot verify task");
  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Reserve),"daqTask","Cannot reserve task resources");
  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Commit),"daqTask","Cannot commit task");
  committed = true;
}

void daqTask::uncommit() {
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
  daqTask *me = (daqTask *)callbackData;
  me->callbackEvery(nscans);
  return 0;
}

void daqTask::callbackEvery(int) {
}

void daqTask::preStart() {
  commit();
  
  daqTry(DAQmxRegisterDoneEvent(th,0,
				&daqTask_CallbackDone,(void*)this),
	 "daqTask","Cannot register Done event");
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
  }
}

void daqTask::postStart() {
  daqTry(DAQmxStartTask(th),"daqTask","Cannot start task");
}

void daqTask::start() {
  preStart();
  postStart();
}

bool daqTask::isRunning() {
  if (!committed)
    return false;
  if (hasCompleted())
    return false;
  fprintf(stderr,"Warning: daqTask: I think we're running, because we're committed and not complete. I'm not sure if that's solid reasoning.\n");
  return true;
}

int daqTask::countScansAvailable() {
  if (!committed)
    return 0;
  uInt32 cnt;
  daqTry(DAQmxGetReadAvailSampPerChan(th, &cnt),
	 "daqTask","Cannot count scans");
  return cnt;
}

bool daqTask::hasCompleted() {
  if (!committed)
    return false;
  bool32 data;
  daqTry(DAQmxIsTaskDone(th,&data),
	 "daqTask","Cannot ascertain completion");
  return data;
}

int daqTask::countScansSoFar() {
  throw daqException("daqTask","I don't know how to count scans abstractly");
}

void daqTask::setTriggering(bool trg) {
  startTrig = trg;
}

void daqTask::preStop() {
  daqTry(DAQmxStopTask(th),"daqTask","Cannot stop task");
}

void daqTask::postStop() {
}

void daqTask::stop() {
  preStop();
  postStop();
}

void daqTask::preAbort() {
  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Abort),"daqTask","Cannot abort task");
}

void daqTask::postAbort() {
  stop();
  uncommit();
}

void daqTask::abort() {
  preAbort();
  postAbort();
}

char const *daqTask::name() const { return "daqTask"; }
