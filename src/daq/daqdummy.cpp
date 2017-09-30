// daq/daqdummy.cpp - This file is part of VScope.
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

// daqdummy.cpp - Dummy version of the NIDAQmx library.
// Created by daqmx2dummy.pl. Do not edit.

#include <../nidaq/NIDAQmx.h>

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

int32 DAQmxReadAnalogF64(TaskHandle, int32, float64, bool32, float64[], uInt32, int32*, bool32*) {
  return -1;
}

int32 DAQmxReadDigitalU32(TaskHandle, int32, float64, bool32, uInt32[], uInt32, int32*, bool32*) {
  return -1;
}

int32 DAQmxReadRaw(TaskHandle, int32, float64, void*, uInt32, int32*, int32*, bool32*) {
  return -1;
}

int32 DAQmxWriteAnalogF64(TaskHandle, int32, bool32, float64, bool32, const float64[], int32*, bool32*) {
  return -1;
}

int32 DAQmxWriteDigitalU32(TaskHandle, int32, bool32, float64, bool32, const uInt32[], int32*, bool32*) {
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

int32 DAQmxGetErrorString(int32, char[], uInt32) {
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

int32 DAQmxGetSysDevNames(char*, uInt32) {
  return -1;
}

int32 DAQmxGetSysNIDAQMajorVersion(uInt32*) {
  return -1;
}

int32 DAQmxGetSysNIDAQMinorVersion(uInt32*) {
  return -1;
}

int32 DAQmxGetSysNIDAQUpdateVersion(uInt32*) {
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

