// daqtest.cpp

#include <NIDAQmx.h>
#include <QApplication>
#include <QString>

void daqTry(int errcode, QString is, QString ms, QString au="") {
  if (errcode==0)
    return;
  int n = DAQmxGetErrorString(errcode,0,0);
  if (n<0 || n>1000)
    n=1000;

  char *data = new char[n];
  DAQmxGetErrorString(errcode,data,n);
  fprintf(stderr,"DAQ Problem #%i: '%s'\n",errcode,data);
  fprintf(stderr,"Reported by: %s (%s): %s\n",
	  qPrintable(is),qPrintable(au),qPrintable(ms));
  QApplication::quit();
}

int32 CVICALLBACK daqTask_CallbackEvery(TaskHandle th,
					int32 type,
					uInt32 nscans,
					void *data) {
  fprintf(stderr,"callback th=%li type=%li nscans=%uli, data=%p\n",
	 th,type,nscans,data);
  return 0;
}

int main(int argc, char **argv) {
  QApplication qap(argc, argv);

  TaskHandle th;
  fprintf(stderr,"daqtest.\n");
  daqTry(DAQmxCreateTask("", &th),"daqTask","Task creation failed");
  fprintf(stderr,"task created\n");

  daqTry(DAQmxCreateAIVoltageChan(th,"Dev1/ai0","",
				  DAQmx_Val_NRSE,
				  -5,5,
				  DAQmx_Val_Volts,0),
	 "AnalogIn","Create AI channel");
  fprintf(stderr,"dev1/ai0 added\n");

  daqTry(DAQmxCreateAIVoltageChan(th,"Dev1/ai1","",
				  DAQmx_Val_NRSE,
				  -5,5,
				  DAQmx_Val_Volts,0),
	 "AnalogIn","Create AI channel");
  fprintf(stderr,"dev1/ai1 added\n");

  daqTry(DAQmxCfgSampClkTiming(th,"OnboardClock",
			       10000,
			       DAQmx_Val_Rising,
			       DAQmx_Val_ContSamps,
			       65536),
	 "AnalogIn","Cannot configure continuous acquisition");

  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Verify),"daqTask","Cannot verify task");
  fprintf(stderr,"task verified\n");

  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Reserve),"daqTask","Cannot reserve task resources");
  fprintf(stderr,"task reserved\n");

  daqTry(DAQmxTaskControl(th,DAQmx_Val_Task_Commit),"daqTask","Cannot commit task");
  fprintf(stderr,"task committed\n");

  daqTry(DAQmxRegisterEveryNSamplesEvent(th,DAQmx_Val_Acquired_Into_Buffer,
					 10000,0,
					 &daqTask_CallbackEvery,0),
	 "daqTask","Cannot register EveryN event");
  fprintf(stderr,"callback registered\n");
  
  daqTry(DAQmxStartTask(th),"AnalogIn","Cannot start task");
  fprintf(stderr,"task started\n");

  for (int k=0; k<10; k++) {
    sleep(1);
    fprintf(stderr,"slept %i s\n",k);
  }
  
  daqTry(DAQmxStopTask(th),"AnalogIn","Cannot stop task");
  fprintf(stderr,"task stopped\n");

  daqTry(DAQmxClearTask(th),"daqTask","Cannot clear task");
  fprintf(stderr,"task cleared\n");

  qap.quit();
  fprintf(stderr,"application quit\n");
  return 0;
}
