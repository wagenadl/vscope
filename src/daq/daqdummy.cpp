// daqdummy.cpp - Dummy version of the NIDAQmx library.
// Created by daqmx2dummy.pl. Do not edit.

#include <../nidaq/NIDAQmx.h>

int32 DAQmxCreateTask(const char taskName[], TaskHandle *taskHandle) {
  return -1;
}

int32 DAQmxStartTask(TaskHandle taskHandle) {
  return -1;
}

int32 DAQmxStopTask(TaskHandle taskHandle) {
  return -1;
}

int32 DAQmxClearTask(TaskHandle taskHandle) {
  return -1;
}

int32 DAQmxIsTaskDone(TaskHandle taskHandle, bool32 *isTaskDone) {
  return -1;
}

int32 DAQmxTaskControl(TaskHandle taskHandle, int32 action) {
  return -1;
}

int32 DAQmxRegisterEveryNSamplesEvent(TaskHandle task, int32 everyNsamplesEventType, uInt32 nSamples, uInt32 options, DAQmxEveryNSamplesEventCallbackPtr callbackFunction, void *callbackData) {
  return -1;
}

int32 DAQmxRegisterDoneEvent(TaskHandle task, uInt32 options, DAQmxDoneEventCallbackPtr callbackFunction, void *callbackData) {
  return -1;
}

int32 DAQmxCreateAIVoltageChan(TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], int32 terminalConfig, float64 minVal, float64 maxVal, int32 units, const char customScaleName[]) {
  return -1;
}

int32 DAQmxCreateAOVoltageChan(TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], float64 minVal, float64 maxVal, int32 units, const char customScaleName[]) {
  return -1;
}

int32 DAQmxCreateDIChan(TaskHandle taskHandle, const char lines[], const char nameToAssignToLines[], int32 lineGrouping) {
  return -1;
}

int32 DAQmxCreateDOChan(TaskHandle taskHandle, const char lines[], const char nameToAssignToLines[], int32 lineGrouping) {
  return -1;
}

int32 DAQmxCfgSampClkTiming(TaskHandle taskHandle, const char source[], float64 rate, int32 activeEdge, int32 sampleMode, uInt64 sampsPerChan) {
  return -1;
}

int32 DAQmxCfgDigEdgeStartTrig(TaskHandle taskHandle, const char triggerSource[], int32 triggerEdge) {
  return -1;
}

int32 DAQmxReadAnalogF64(TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, float64 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved) {
  return -1;
}

int32 DAQmxReadDigitalU32(TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, uInt32 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved) {
  return -1;
}

int32 DAQmxReadRaw(TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, void *readArray, uInt32 arraySizeInBytes, int32 *sampsRead, int32 *numBytesPerSamp, bool32 *reserved) {
  return -1;
}

int32 DAQmxWriteAnalogF64(TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, const float64 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved) {
  return -1;
}

int32 DAQmxWriteDigitalU32(TaskHandle taskHandle, int32 numSampsPerChan, bool32 autoStart, float64 timeout, bool32 dataLayout, const uInt32 writeArray[], int32 *sampsPerChanWritten, bool32 *reserved) {
  return -1;
}

int32 DAQmxWriteRaw(TaskHandle taskHandle, int32 numSamps, bool32 autoStart, float64 timeout, const void *writeArray, int32 *sampsPerChanWritten, bool32 *reserved) {
  return -1;
}

int32 DAQmxSetWriteAttribute(TaskHandle taskHandle, int32 attribute, ...) {
  return -1;
}

int32 DAQmxCfgOutputBuffer(TaskHandle taskHandle, uInt32 numSampsPerChan) {
  return -1;
}

int32 DAQmxGetErrorString(int32 errorCode, char errorString[], uInt32 bufferSize) {
  return -1;
}

int32 DAQmxGetBufOutputBufSize(TaskHandle taskHandle, uInt32 *data) {
  return -1;
}

int32 DAQmxGetBufOutputOnbrdBufSize(TaskHandle taskHandle, uInt32 *data) {
  return -1;
}

int32 DAQmxGetDONumLines(TaskHandle taskHandle, const char channel[], uInt32 *data) {
  return -1;
}

int32 DAQmxGetDevProductType(const char device[], char *data, uInt32 bufferSize) {
  return -1;
}

int32 DAQmxGetDevProductNum(const char device[], uInt32 *data) {
  return -1;
}

int32 DAQmxGetDevSerialNum(const char device[], uInt32 *data) {
  return -1;
}

int32 DAQmxGetDevAIPhysicalChans(const char device[], char *data, uInt32 bufferSize) {
  return -1;
}

int32 DAQmxGetDevAOPhysicalChans(const char device[], char *data, uInt32 bufferSize) {
  return -1;
}

int32 DAQmxGetDevDILines(const char device[], char *data, uInt32 bufferSize) {
  return -1;
}

int32 DAQmxGetReadAvailSampPerChan(TaskHandle taskHandle, uInt32 *data) {
  return -1;
}

int32 DAQmxGetReadTotalSampPerChanAcquired(TaskHandle taskHandle, uInt64 *data) {
  return -1;
}

int32 DAQmxGetSysDevNames(char *data, uInt32 bufferSize) {
  return -1;
}

int32 DAQmxGetSysNIDAQMajorVersion(uInt32 *data) {
  return -1;
}

int32 DAQmxGetSysNIDAQMinorVersion(uInt32 *data) {
  return -1;
}

int32 DAQmxGetSysNIDAQUpdateVersion(uInt32 *data) {
  return -1;
}

int32 DAQmxSetSampTimingType(TaskHandle taskHandle, int32 data) {
  return -1;
}

int32 DAQmxGetWriteSpaceAvail(TaskHandle taskHandle, uInt32 *data) {
  return -1;
}

int32 DAQmxGetWriteTotalSampPerChanGenerated(TaskHandle taskHandle, uInt64 *data) {
  return -1;
}

