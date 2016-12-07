// daqdummy.cpp - Dummy version of the NIDAQmx library.
// Created by daqmx2dummy.pl. Do not edit.

#include <daq/dwNIDAQmx.h>

int32 DAQmxCreateTask(const char[], TaskHandle*) {
  return -1;
}

int32 DAQmxStartTask(TaskHandle) {
  return -1;
}

int32 DAQmxStopTask(TaskHandle) {
  return -1;
}

int32 DAQmxClearTask(TaskHandle) {
  return -1;
}

int32 DAQmxIsTaskDone(TaskHandle, bool32*) {
  return -1;
}

int32 DAQmxTaskControl(TaskHandle, int32) {
  return -1;
}

int32 DAQmxRegisterEveryNSamplesEvent(TaskHandle, int32, uInt32, uInt32, DAQmxEveryNSamplesEventCallbackPtr, void*) {
  return -1;
}

int32 DAQmxRegisterDoneEvent(TaskHandle, uInt32, DAQmxDoneEventCallbackPtr, void*) {
  return -1;
}

int32 DAQmxCreateAIVoltageChan(TaskHandle, const char[], const char[], int32, float64, float64, int32, const char[]) {
  return -1;
}

int32 DAQmxCreateAOVoltageChan(TaskHandle, const char[], const char[], float64, float64, int32, const char[]) {
  return -1;
}

int32 DAQmxCreateDIChan(TaskHandle, const char[], const char[], int32) {
  return -1;
}

int32 DAQmxCreateDOChan(TaskHandle, const char[], const char[], int32) {
  return -1;
}

int32 DAQmxCfgSampClkTiming(TaskHandle, const char[], float64, int32, int32, uInt64) {
  return -1;
}

int32 DAQmxCfgDigEdgeStartTrig(TaskHandle, const char[], int32) {
  return -1;
}

int32 DAQmxReadRaw(TaskHandle, int32, float64, void*, uInt32, int32*, int32*, bool32*) {
  return -1;
}

int32 DAQmxWriteRaw(TaskHandle, int32, bool32, float64, const void*, int32*, bool32*) {
  return -1;
}

int32 DAQmxSetWriteAttribute(TaskHandle, int32, ...) {
  return -1;
}

int32 DAQmxCfgOutputBuffer(TaskHandle, uInt32) {
  return -1;
}

int32 DAQmxGetBufOutputBufSize(TaskHandle, uInt32*) {
  return -1;
}

int32 DAQmxGetBufOutputOnbrdBufSize(TaskHandle, uInt32*) {
  return -1;
}

int32 DAQmxGetDONumLines(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 DAQmxGetDevProductType(const char[], char*, uInt32) {
  return -1;
}

int32 DAQmxGetDevProductNum(const char[], uInt32*) {
  return -1;
}

int32 DAQmxGetDevSerialNum(const char[], uInt32*) {
  return -1;
}

int32 DAQmxGetDevAIPhysicalChans(const char[], char*, uInt32) {
  return -1;
}

int32 DAQmxGetDevAOPhysicalChans(const char[], char*, uInt32) {
  return -1;
}

int32 DAQmxGetDevDILines(const char[], char*, uInt32) {
  return -1;
}

int32 DAQmxGetReadAvailSampPerChan(TaskHandle, uInt32*) {
  return -1;
}

int32 DAQmxGetReadTotalSampPerChanAcquired(TaskHandle, uInt64*) {
  return -1;
}

int32 DAQmxSetSampTimingType(TaskHandle, int32) {
  return -1;
}

int32 DAQmxGetWriteSpaceAvail(TaskHandle, uInt32*) {
  return -1;
}

int32 DAQmxGetWriteTotalSampPerChanGenerated(TaskHandle, uInt64*) {
  return -1;
}

