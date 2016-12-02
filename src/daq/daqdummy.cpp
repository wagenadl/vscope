// daqdummy.cpp - Dummy version of the NIDAQmx library.
// Created by daqmx2dummy.pl. Do not edit.

#include <daq/dwNIDAQmx.h>

int32 __CFUNC DAQmxLoadTask (const char[], TaskHandle*) {
  return -1;
}

int32 __CFUNC DAQmxCreateTask (const char[], TaskHandle*) {
  return -1;
}

int32 __CFUNC DAQmxAddGlobalChansToTask(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxStartTask (TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxStopTask (TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxClearTask (TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxWaitUntilTaskDone (TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxIsTaskDone (TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxTaskControl (TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxGetNthTaskChannel (TaskHandle, uInt32, char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxGetNthTaskDevice (TaskHandle, uInt32, char[], int32) {
  return -1;
}

int32 __CFUNC_C DAQmxGetTaskAttribute (TaskHandle, int32, void*, ...) {
  return -1;
}

int32 __CFUNC DAQmxRegisterEveryNSamplesEvent (TaskHandle, int32, uInt32, uInt32, DAQmxEveryNSamplesEventCallbackPtr, void*) {
  return -1;
}

int32 __CFUNC DAQmxRegisterDoneEvent (TaskHandle, uInt32, DAQmxDoneEventCallbackPtr, void*) {
  return -1;
}

int32 __CFUNC DAQmxRegisterSignalEvent (TaskHandle, int32, uInt32, DAQmxSignalEventCallbackPtr, void*) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIVoltageChan (TaskHandle, const char[], const char[], int32, float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAICurrentChan (TaskHandle, const char[], const char[], int32, float64, float64, int32, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIVoltageRMSChan (TaskHandle, const char[], const char[], int32, float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAICurrentRMSChan (TaskHandle, const char[], const char[], int32, float64, float64, int32, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIThrmcplChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIRTDChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, int32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIThrmstrChanIex (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64, float64, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIThrmstrChanVex (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64, float64, float64, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIFreqVoltageChan (TaskHandle, const char[], const char[], float64, float64, int32, float64, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIResistanceChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIStrainGageChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64, float64, float64, float64, float64, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIVoltageChanWithExcit (TaskHandle, const char[], const char[], int32, float64, float64, int32, int32, int32, float64, bool32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAITempBuiltInSensorChan(TaskHandle, const char[], const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIAccelChan (TaskHandle, const char[], const char[], int32, float64, float64, int32, float64, int32, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIMicrophoneChan (TaskHandle, const char[], const char[], int32, int32, float64, float64, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIPosLVDTChan (TaskHandle, const char[], const char[], float64, float64, int32, float64, int32, int32, float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIPosRVDTChan (TaskHandle, const char[], const char[], float64, float64, int32, float64, int32, int32, float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAIDeviceTempChan (TaskHandle, const char[], const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIVoltageChan (TaskHandle, const char[], const char[], int32, float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAICurrentChan (TaskHandle, const char[], const char[], int32, float64, float64, int32, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIThrmcplChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIRTDChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIThrmstrChanIex (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIThrmstrChanVex (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIResistanceChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIStrainGageChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, float64, float64, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIVoltageChanWithExcit (TaskHandle, const char[], const char[], int32, float64, float64, int32, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIAccelChan (TaskHandle, const char[], const char[], int32, float64, float64, int32, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIMicrophoneChan (TaskHandle, const char[], const char[], int32, int32, float64, int32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIPosLVDTChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateTEDSAIPosRVDTChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAOVoltageChan (TaskHandle, const char[], const char[], float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAOCurrentChan (TaskHandle, const char[], const char[], float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateAOFuncGenChan (TaskHandle, const char[], const char[], int32, float64, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxCreateDIChan (TaskHandle, const char[], const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxCreateDOChan (TaskHandle, const char[], const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxCreateCIFreqChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64, uInt32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateCIPeriodChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, float64, uInt32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateCICountEdgesChan (TaskHandle, const char[], const char[], int32, uInt32, int32) {
  return -1;
}

int32 __CFUNC DAQmxCreateCIPulseWidthChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateCISemiPeriodChan (TaskHandle, const char[], const char[], float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateCITwoEdgeSepChan (TaskHandle, const char[], const char[], float64, float64, int32, int32, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateCILinEncoderChan (TaskHandle, const char[], const char[], int32, bool32, float64, int32, int32, float64, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateCIAngEncoderChan (TaskHandle, const char[], const char[], int32, bool32, float64, int32, int32, uInt32, float64, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateCIGPSTimestampChan (TaskHandle, const char[], const char[], int32, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateCOPulseChanFreq (TaskHandle, const char[], const char[], int32, int32, float64, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxCreateCOPulseChanTime (TaskHandle, const char[], const char[], int32, int32, float64, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxCreateCOPulseChanTicks (TaskHandle, const char[], const char[], const char[], int32, int32, int32, int32) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalCalDate(TaskHandle, const char[], uInt32*, uInt32*, uInt32*, uInt32*, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalCalDate(TaskHandle, const char[], uInt32, uInt32, uInt32, uInt32, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalExpDate(TaskHandle, const char[], uInt32*, uInt32*, uInt32*, uInt32*, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalExpDate(TaskHandle, const char[], uInt32, uInt32, uInt32, uInt32, uInt32) {
  return -1;
}

int32 __CFUNC_C DAQmxGetChanAttribute (TaskHandle, const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetChanAttribute (TaskHandle, const char[], int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetChanAttribute (TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxCfgSampClkTiming (TaskHandle, const char[], float64, int32, int32, uInt64) {
  return -1;
}

int32 __CFUNC DAQmxCfgHandshakingTiming (TaskHandle, int32, uInt64) {
  return -1;
}

int32 __CFUNC DAQmxCfgBurstHandshakingTimingImportClock(TaskHandle, int32, uInt64, float64, const char[], int32, int32, int32) {
  return -1;
}

int32 __CFUNC DAQmxCfgBurstHandshakingTimingExportClock(TaskHandle, int32, uInt64, float64, const char[], int32, int32, int32) {
  return -1;
}

int32 __CFUNC DAQmxCfgChangeDetectionTiming (TaskHandle, const char[], const char[], int32, uInt64) {
  return -1;
}

int32 __CFUNC DAQmxCfgImplicitTiming (TaskHandle, int32, uInt64) {
  return -1;
}

int32 __CFUNC DAQmxCfgPipelinedSampClkTiming (TaskHandle, const char[], float64, int32, int32, uInt64) {
  return -1;
}

int32 __CFUNC_C DAQmxGetTimingAttribute (TaskHandle, int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetTimingAttribute (TaskHandle, int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetTimingAttribute (TaskHandle, int32) {
  return -1;
}

int32 __CFUNC_C DAQmxGetTimingAttributeEx (TaskHandle, const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetTimingAttributeEx (TaskHandle, const char[], int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetTimingAttributeEx (TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxDisableStartTrig (TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxCfgDigEdgeStartTrig (TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxCfgAnlgEdgeStartTrig (TaskHandle, const char[], int32, float64) {
  return -1;
}

int32 __CFUNC DAQmxCfgAnlgWindowStartTrig(TaskHandle, const char[], int32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxCfgDigPatternStartTrig(TaskHandle, const char[], const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxDisableRefTrig (TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxCfgDigEdgeRefTrig (TaskHandle, const char[], int32, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxCfgAnlgEdgeRefTrig (TaskHandle, const char[], int32, float64, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxCfgAnlgWindowRefTrig (TaskHandle, const char[], int32, float64, float64, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxCfgDigPatternRefTrig (TaskHandle, const char[], const char[], int32, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxDisableAdvTrig (TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxCfgDigEdgeAdvTrig (TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC_C DAQmxGetTrigAttribute (TaskHandle, int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetTrigAttribute (TaskHandle, int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetTrigAttribute (TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxSendSoftwareTrigger (TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxReadAnalogF64 (TaskHandle, int32, float64, bool32, float64[], uInt32, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadAnalogScalarF64 (TaskHandle, float64, float64*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadBinaryI16 (TaskHandle, int32, float64, bool32, int16[], uInt32, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadBinaryU16 (TaskHandle, int32, float64, bool32, uInt16[], uInt32, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadBinaryI32 (TaskHandle, int32, float64, bool32, int32[], uInt32, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadBinaryU32 (TaskHandle, int32, float64, bool32, uInt32[], uInt32, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadDigitalU8 (TaskHandle, int32, float64, bool32, uInt8[], uInt32, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadDigitalU16 (TaskHandle, int32, float64, bool32, uInt16[], uInt32, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadDigitalU32 (TaskHandle, int32, float64, bool32, uInt32[], uInt32, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadDigitalScalarU32 (TaskHandle, float64, uInt32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadDigitalLines (TaskHandle, int32, float64, bool32, uInt8[], uInt32, int32*, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadCounterF64 (TaskHandle, int32, float64, float64[], uInt32, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadCounterU32 (TaskHandle, int32, float64, uInt32[], uInt32, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadCounterScalarF64 (TaskHandle, float64, float64*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadCounterScalarU32 (TaskHandle, float64, uInt32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxReadRaw (TaskHandle, int32, float64, void*, uInt32, int32*, int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetNthTaskReadChannel (TaskHandle, uInt32, char[], int32) {
  return -1;
}

int32 __CFUNC_C DAQmxGetReadAttribute (TaskHandle, int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetReadAttribute (TaskHandle, int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetReadAttribute (TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxWriteAnalogF64 (TaskHandle, int32, bool32, float64, bool32, const float64[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteAnalogScalarF64 (TaskHandle, bool32, float64, float64, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteBinaryI16 (TaskHandle, int32, bool32, float64, bool32, const int16[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteBinaryU16 (TaskHandle, int32, bool32, float64, bool32, const uInt16[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteBinaryI32 (TaskHandle, int32, bool32, float64, bool32, const int32[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteBinaryU32 (TaskHandle, int32, bool32, float64, bool32, const uInt32[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteDigitalU8 (TaskHandle, int32, bool32, float64, bool32, const uInt8[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteDigitalU16 (TaskHandle, int32, bool32, float64, bool32, const uInt16[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteDigitalU32 (TaskHandle, int32, bool32, float64, bool32, const uInt32[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteDigitalScalarU32 (TaskHandle, bool32, float64, uInt32, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteDigitalLines (TaskHandle, int32, bool32, float64, bool32, const uInt8[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteCtrFreq (TaskHandle, int32, bool32, float64, bool32, const float64[], const float64[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteCtrFreqScalar (TaskHandle, bool32, float64, float64, float64, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteCtrTime (TaskHandle, int32, bool32, float64, bool32, const float64[], const float64[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteCtrTimeScalar (TaskHandle, bool32, float64, float64, float64, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteCtrTicks (TaskHandle, int32, bool32, float64, bool32, const uInt32[], const uInt32[], int32*, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteCtrTicksScalar (TaskHandle, bool32, float64, uInt32, uInt32, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxWriteRaw (TaskHandle, int32, bool32, float64, const void*, int32*, bool32*) {
  return -1;
}

int32 __CFUNC_C DAQmxGetWriteAttribute (TaskHandle, int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetWriteAttribute (TaskHandle, int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetWriteAttribute (TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxExportSignal (TaskHandle, int32, const char[]) {
  return -1;
}

int32 __CFUNC_C DAQmxGetExportedSignalAttribute (TaskHandle, int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetExportedSignalAttribute (TaskHandle, int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedSignalAttribute(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxCreateLinScale (const char[], float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateMapScale (const char[], float64, float64, float64, float64, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreatePolynomialScale (const char[], const float64[], uInt32, const float64[], uInt32, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCreateTableScale (const char[], const float64[], uInt32, const float64[], uInt32, int32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxCalculateReversePolyCoeff (const float64[], uInt32, float64, float64, int32, int32, float64[]) {
  return -1;
}

int32 __CFUNC_C DAQmxGetScaleAttribute (const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetScaleAttribute (const char[], int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxCfgInputBuffer (TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxCfgOutputBuffer (TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC_C DAQmxGetBufferAttribute (TaskHandle, int32, void*) {
  return -1;
}

int32 __CFUNC_C DAQmxSetBufferAttribute (TaskHandle, int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetBufferAttribute(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxSwitchCreateScanList (const char[], TaskHandle*) {
  return -1;
}

int32 __CFUNC DAQmxSwitchConnect (const char[], const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxSwitchConnectMulti (const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxSwitchDisconnect (const char[], const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxSwitchDisconnectMulti (const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxSwitchDisconnectAll (const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxSwitchSetTopologyAndReset (const char[], const char[]) {
  return -1;
}

int32 __CFUNC DAQmxSwitchFindPath (const char[], const char[], char[], uInt32, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSwitchOpenRelays (const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxSwitchCloseRelays (const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxSwitchGetSingleRelayCount (const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSwitchGetMultiRelayCount (const char[], uInt32[], uInt32, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSwitchGetSingleRelayPos (const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSwitchGetMultiRelayPos (const char[], uInt32[], uInt32, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSwitchWaitForSettling (const char[]) {
  return -1;
}

int32 __CFUNC_C DAQmxGetSwitchChanAttribute (const char[], int32, void*) {
  return -1;
}

int32 __CFUNC_C DAQmxSetSwitchChanAttribute (const char[], int32, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxGetSwitchDeviceAttribute (const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetSwitchDeviceAttribute (const char[], int32, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxGetSwitchScanAttribute (TaskHandle, int32, void*) {
  return -1;
}

int32 __CFUNC_C DAQmxSetSwitchScanAttribute (TaskHandle, int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetSwitchScanAttribute (TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxConnectTerms (const char[], const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxDisconnectTerms (const char[], const char[]) {
  return -1;
}

int32 __CFUNC DAQmxTristateOutputTerm (const char[]) {
  return -1;
}

int32 __CFUNC DAQmxResetDevice (const char[]) {
  return -1;
}

int32 __CFUNC_C DAQmxGetDeviceAttribute (const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxCreateWatchdogTimerTask (const char[], const char[], TaskHandle*, float64, const char[], int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxControlWatchdogTask (TaskHandle, int32) {
  return -1;
}

int32 __CFUNC_C DAQmxGetWatchdogAttribute (TaskHandle, const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetWatchdogAttribute (TaskHandle, const char[], int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetWatchdogAttribute (TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxSelfCal (const char[]) {
  return -1;
}

int32 __CFUNC DAQmxPerformBridgeOffsetNullingCal(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxPerformBridgeOffsetNullingCalEx(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxPerformStrainShuntCal (TaskHandle, const char[], float64, int32, bool32) {
  return -1;
}

int32 __CFUNC DAQmxPerformBridgeShuntCal (TaskHandle, const char[], float64, int32, float64, bool32) {
  return -1;
}

int32 __CFUNC DAQmxGetSelfCalLastDateAndTime (const char[], uInt32*, uInt32*, uInt32*, uInt32*, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetExtCalLastDateAndTime (const char[], uInt32*, uInt32*, uInt32*, uInt32*, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxRestoreLastExtCalConst (const char[]) {
  return -1;
}

int32 __CFUNC DAQmxESeriesCalAdjust (uInt32, float64) {
  return -1;
}

int32 __CFUNC DAQmxMSeriesCalAdjust (uInt32, float64) {
  return -1;
}

int32 __CFUNC DAQmxSSeriesCalAdjust (uInt32, float64) {
  return -1;
}

int32 __CFUNC DAQmxSCBaseboardCalAdjust (uInt32, float64) {
  return -1;
}

int32 __CFUNC DAQmxAOSeriesCalAdjust (uInt32, float64) {
  return -1;
}

int32 __CFUNC DAQmxDeviceSupportsCal (const char[], bool32*) {
  return -1;
}

int32 __CFUNC_C DAQmxGetCalInfoAttribute (const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetCalInfoAttribute (const char[], int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxInitExtCal (const char[], const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxCloseExtCal (uInt32, int32) {
  return -1;
}

int32 __CFUNC DAQmxChangeExtCalPassword (const char[], const char[], const char[]) {
  return -1;
}

int32 __CFUNC DAQmxAdjustDSAAICal (uInt32, float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjustDSAAOCal (uInt32, uInt32, float64, float64, float64, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjustDSATimebaseCal (uInt32, float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust4204Cal (uInt32, const char[], float64, bool32, float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust4220Cal (uInt32, const char[], float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust4224Cal (uInt32, const char[], float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust4225Cal (uInt32, const char[], float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1102Cal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1102Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1104Cal (uInt32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1104Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1112Cal (uInt32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1112Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1122Cal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1122Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1124Cal (uInt32, const char[], int32, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1124Cal (uInt32, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1125Cal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1125Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1126Cal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1126Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1141Cal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1141Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1142Cal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1142Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1143Cal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1143Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1502Cal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1502Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1503Cal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1503Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1503CurrentCal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1520Cal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1520Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1521Cal (uInt32, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1521Cal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup153xCal (uInt32, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust153xCal (uInt32, float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxSetup1540Cal (uInt32, const char[], float64, float64) {
  return -1;
}

int32 __CFUNC DAQmxAdjust1540Cal (uInt32, float64, float64, int32) {
  return -1;
}

int32 __CFUNC DAQmxConfigureTEDS (const char[], const char[]) {
  return -1;
}

int32 __CFUNC DAQmxClearTEDS (const char[]) {
  return -1;
}

int32 __CFUNC DAQmxWriteToTEDSFromArray (const char[], const uInt8[], uInt32, int32) {
  return -1;
}

int32 __CFUNC DAQmxWriteToTEDSFromFile (const char[], const char[], int32) {
  return -1;
}

int32 __CFUNC_C DAQmxGetPhysicalChanAttribute (const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC DAQmxWaitForNextSampleClock(TaskHandle, float64, bool32*) {
  return -1;
}

int32 __CFUNC_C DAQmxGetRealTimeAttribute (TaskHandle, int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetRealTimeAttribute (TaskHandle, int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxResetRealTimeAttribute(TaskHandle, int32) {
  return -1;
}

bool32 __CFUNC DAQmxIsReadOrWriteLate (int32) {
  return -1;
}

int32 __CFUNC DAQmxSaveTask (TaskHandle, const char[], const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSaveGlobalChan (TaskHandle, const char[], const char[], const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSaveScale (const char[], const char[], const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxDeleteSavedTask (const char[]) {
  return -1;
}

int32 __CFUNC DAQmxDeleteSavedGlobalChan (const char[]) {
  return -1;
}

int32 __CFUNC DAQmxDeleteSavedScale (const char[]) {
  return -1;
}

int32 __CFUNC_C DAQmxGetPersistedTaskAttribute (const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxGetPersistedChanAttribute (const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxGetPersistedScaleAttribute (const char[], int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxGetSystemInfoAttribute (int32, void*, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetDigitalPowerUpStates(const char[], const char[], int32, ...) {
  return -1;
}

int32 __CFUNC_C DAQmxSetAnalogPowerUpStates(const char[], const char[], float64, int32, ...) {
  return -1;
}

int32 __CFUNC DAQmxSetDigitalLogicFamilyPowerUpState(const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxGetErrorString (int32, char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetExtendedErrorInfo (char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetBufInputBufSize(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetBufInputBufSize(TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetBufInputBufSize(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetBufInputOnbrdBufSize(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetBufOutputBufSize(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetBufOutputBufSize(TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetBufOutputBufSize(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetBufOutputOnbrdBufSize(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetBufOutputOnbrdBufSize(TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetBufOutputOnbrdBufSize(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSelfCalSupported(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetSelfCalLastTemp(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetExtCalRecommendedInterval(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetExtCalLastTemp(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetCalUserDefinedInfo(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCalUserDefinedInfo(const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxGetCalUserDefinedInfoMaxSize(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetCalDevTemp(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetAIMax(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIMax(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIMax(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIMin(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIMin(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIMin(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAICustomScaleName(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAICustomScaleName(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAICustomScaleName(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIMeasType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetAIVoltageUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIVoltageUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIVoltageUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIVoltagedBRef(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIVoltagedBRef(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIVoltagedBRef(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIVoltageACRMSUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIVoltageACRMSUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIVoltageACRMSUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAITempUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAITempUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAITempUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIThrmcplType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIThrmcplType(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIThrmcplType(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIThrmcplScaleType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIThrmcplScaleType(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIThrmcplScaleType(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIThrmcplCJCSrc(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetAIThrmcplCJCVal(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIThrmcplCJCVal(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIThrmcplCJCVal(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIThrmcplCJCChan(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRTDType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRTDType(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRTDType(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRTDR0(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRTDR0(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRTDR0(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRTDA(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRTDA(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRTDA(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRTDB(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRTDB(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRTDB(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRTDC(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRTDC(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRTDC(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIThrmstrA(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIThrmstrA(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIThrmstrA(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIThrmstrB(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIThrmstrB(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIThrmstrB(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIThrmstrC(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIThrmstrC(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIThrmstrC(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIThrmstrR1(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIThrmstrR1(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIThrmstrR1(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIForceReadFromChan(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIForceReadFromChan(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIForceReadFromChan(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAICurrentUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAICurrentUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAICurrentUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAICurrentACRMSUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAICurrentACRMSUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAICurrentACRMSUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIStrainUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIStrainUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIStrainUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIStrainGageGageFactor(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIStrainGageGageFactor(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIStrainGageGageFactor(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIStrainGagePoissonRatio(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIStrainGagePoissonRatio(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIStrainGagePoissonRatio(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIStrainGageCfg(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIStrainGageCfg(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIStrainGageCfg(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIResistanceUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIResistanceUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIResistanceUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIFreqUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIFreqUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIFreqUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIFreqThreshVoltage(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIFreqThreshVoltage(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIFreqThreshVoltage(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIFreqHyst(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIFreqHyst(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIFreqHyst(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILVDTUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILVDTUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAILVDTUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILVDTSensitivity(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILVDTSensitivity(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAILVDTSensitivity(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILVDTSensitivityUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILVDTSensitivityUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAILVDTSensitivityUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRVDTUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRVDTUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRVDTUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRVDTSensitivity(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRVDTSensitivity(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRVDTSensitivity(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRVDTSensitivityUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRVDTSensitivityUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRVDTSensitivityUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAISoundPressureMaxSoundPressureLvl(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAISoundPressureMaxSoundPressureLvl(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAISoundPressureMaxSoundPressureLvl(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAISoundPressureUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAISoundPressureUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAISoundPressureUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAISoundPressuredBRef(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAISoundPressuredBRef(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAISoundPressuredBRef(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIMicrophoneSensitivity(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIMicrophoneSensitivity(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIMicrophoneSensitivity(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIAccelUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIAccelUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIAccelUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIAcceldBRef(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIAcceldBRef(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIAcceldBRef(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIAccelSensitivity(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIAccelSensitivity(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIAccelSensitivity(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIAccelSensitivityUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIAccelSensitivityUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIAccelSensitivityUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIIsTEDS(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetAITEDSUnits(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetAICoupling(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAICoupling(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAICoupling(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIImpedance(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIImpedance(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIImpedance(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAITermCfg(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAITermCfg(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAITermCfg(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIInputSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIInputSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAIInputSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIResistanceCfg(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIResistanceCfg(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIResistanceCfg(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILeadWireResistance(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILeadWireResistance(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAILeadWireResistance(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIBridgeCfg(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIBridgeCfg(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIBridgeCfg(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIBridgeNomResistance(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIBridgeNomResistance(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIBridgeNomResistance(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIBridgeInitialVoltage(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIBridgeInitialVoltage(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIBridgeInitialVoltage(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIBridgeShuntCalEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIBridgeShuntCalEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIBridgeShuntCalEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIBridgeShuntCalSelect(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIBridgeShuntCalSelect(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIBridgeShuntCalSelect(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIBridgeShuntCalGainAdjust(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIBridgeShuntCalGainAdjust(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIBridgeShuntCalGainAdjust(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIBridgeBalanceCoarsePot(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIBridgeBalanceCoarsePot(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIBridgeBalanceCoarsePot(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIBridgeBalanceFinePot(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIBridgeBalanceFinePot(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIBridgeBalanceFinePot(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAICurrentShuntLoc(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAICurrentShuntLoc(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAICurrentShuntLoc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAICurrentShuntResistance(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAICurrentShuntResistance(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAICurrentShuntResistance(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIExcitSrc(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIExcitSrc(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIExcitSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIExcitVal(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIExcitVal(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIExcitVal(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIExcitUseForScaling(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIExcitUseForScaling(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIExcitUseForScaling(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIExcitUseMultiplexed(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIExcitUseMultiplexed(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIExcitUseMultiplexed(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIExcitActualVal(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIExcitActualVal(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIExcitActualVal(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIExcitDCorAC(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIExcitDCorAC(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIExcitDCorAC(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIExcitVoltageOrCurrent(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIExcitVoltageOrCurrent(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIExcitVoltageOrCurrent(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIACExcitFreq(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIACExcitFreq(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIACExcitFreq(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIACExcitSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIACExcitSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIACExcitSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIACExcitWireMode(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIACExcitWireMode(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIACExcitWireMode(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIAtten(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIAtten(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIAtten(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILowpassEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILowpassEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAILowpassEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILowpassCutoffFreq(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILowpassCutoffFreq(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAILowpassCutoffFreq(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILowpassSwitchCapClkSrc(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILowpassSwitchCapClkSrc(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAILowpassSwitchCapClkSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILowpassSwitchCapExtClkFreq(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILowpassSwitchCapExtClkFreq(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAILowpassSwitchCapExtClkFreq(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILowpassSwitchCapExtClkDiv(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILowpassSwitchCapExtClkDiv(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAILowpassSwitchCapExtClkDiv(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILowpassSwitchCapOutClkDiv(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILowpassSwitchCapOutClkDiv(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAILowpassSwitchCapOutClkDiv(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIResolutionUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetAIResolution(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRawSampSize(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRawSampJustification(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetAIADCTimingMode(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIADCTimingMode(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIADCTimingMode(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIDitherEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIDitherEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIDitherEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalHasValidCalInfo(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalEnableCal(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalEnableCal(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalEnableCal(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalApplyCalIfExp(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalApplyCalIfExp(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalApplyCalIfExp(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalScaleType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalScaleType(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalScaleType(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalTablePreScaledVals(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalTablePreScaledVals(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalTablePreScaledVals(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalTableScaledVals(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalTableScaledVals(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalTableScaledVals(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalPolyForwardCoeff(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalPolyForwardCoeff(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalPolyForwardCoeff(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalPolyReverseCoeff(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalPolyReverseCoeff(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalPolyReverseCoeff(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalOperatorName(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalOperatorName(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalOperatorName(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalDesc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalDesc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalDesc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalVerifRefVals(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalVerifRefVals(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalVerifRefVals(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIChanCalVerifAcqVals(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIChanCalVerifAcqVals(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIChanCalVerifAcqVals(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRngHigh(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRngHigh(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRngHigh(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRngLow(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRngLow(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRngLow(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIGain(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIGain(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIGain(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAISampAndHoldEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAISampAndHoldEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAISampAndHoldEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIAutoZeroMode(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIAutoZeroMode(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIAutoZeroMode(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIDataXferMech(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIDataXferMech(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIDataXferMech(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIDataXferReqCond(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIDataXferReqCond(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIDataXferReqCond(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIDataXferCustomThreshold(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIDataXferCustomThreshold(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIDataXferCustomThreshold(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIMemMapEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIMemMapEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIMemMapEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIRawDataCompressionType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIRawDataCompressionType(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIRawDataCompressionType(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAILossyLSBRemovalCompressedSampSize(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAILossyLSBRemovalCompressedSampSize(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAILossyLSBRemovalCompressedSampSize(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIDevScalingCoeff(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetAIEnhancedAliasRejectionEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIEnhancedAliasRejectionEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIEnhancedAliasRejectionEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOMax(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOMax(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOMax(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOMin(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOMin(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOMin(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOCustomScaleName(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAOCustomScaleName(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAOCustomScaleName(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOOutputType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetAOVoltageUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOVoltageUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOVoltageUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOVoltageCurrentLimit(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOVoltageCurrentLimit(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOVoltageCurrentLimit(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOCurrentUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOCurrentUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOCurrentUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOFuncGenType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOFuncGenType(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOFuncGenType(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOFuncGenFreq(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOFuncGenFreq(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOFuncGenFreq(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOFuncGenAmplitude(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOFuncGenAmplitude(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOFuncGenAmplitude(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOFuncGenOffset(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOFuncGenOffset(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOFuncGenOffset(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOFuncGenSquareDutyCycle(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOFuncGenSquareDutyCycle(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOFuncGenSquareDutyCycle(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOFuncGenModulationType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOFuncGenModulationType(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOFuncGenModulationType(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOFuncGenFMDeviation(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOFuncGenFMDeviation(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOFuncGenFMDeviation(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOOutputImpedance(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOOutputImpedance(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOOutputImpedance(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOLoadImpedance(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOLoadImpedance(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOLoadImpedance(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOIdleOutputBehavior(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOIdleOutputBehavior(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOIdleOutputBehavior(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOTermCfg(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOTermCfg(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOTermCfg(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOResolutionUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOResolutionUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOResolutionUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOResolution(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetAODACRngHigh(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAODACRngHigh(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAODACRngHigh(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODACRngLow(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAODACRngLow(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAODACRngLow(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODACRefConnToGnd(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAODACRefConnToGnd(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAODACRefConnToGnd(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODACRefAllowConnToGnd(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAODACRefAllowConnToGnd(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAODACRefAllowConnToGnd(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODACRefSrc(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAODACRefSrc(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAODACRefSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODACRefExtSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAODACRefExtSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAODACRefExtSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODACRefVal(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAODACRefVal(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAODACRefVal(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODACOffsetSrc(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAODACOffsetSrc(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAODACOffsetSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODACOffsetExtSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAODACOffsetExtSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAODACOffsetExtSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODACOffsetVal(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAODACOffsetVal(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAODACOffsetVal(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOReglitchEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOReglitchEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOReglitchEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOGain(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOGain(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAOGain(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOUseOnlyOnBrdMem(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOUseOnlyOnBrdMem(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOUseOnlyOnBrdMem(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODataXferMech(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAODataXferMech(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAODataXferMech(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODataXferReqCond(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAODataXferReqCond(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAODataXferReqCond(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAOMemMapEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOMemMapEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOMemMapEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAODevScalingCoeff(TaskHandle, const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetAOEnhancedImageRejectionEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAOEnhancedImageRejectionEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetAOEnhancedImageRejectionEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDIInvertLines(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDIInvertLines(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDIInvertLines(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDINumLines(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDIDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDIDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDIDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDIDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetDIDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetDIDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDITristate(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDITristate(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDITristate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDILogicFamily(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDILogicFamily(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDILogicFamily(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDIDataXferMech(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDIDataXferMech(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDIDataXferMech(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDIDataXferReqCond(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDIDataXferReqCond(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDIDataXferReqCond(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDIMemMapEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDIMemMapEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDIMemMapEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDIAcquireOn(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDIAcquireOn(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDIAcquireOn(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDOOutputDriveType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDOOutputDriveType(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDOOutputDriveType(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDOInvertLines(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDOInvertLines(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDOInvertLines(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDONumLines(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDOTristate(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDOTristate(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDOTristate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDOLineStatesStartState(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDOLineStatesStartState(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDOLineStatesStartState(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDOLineStatesPausedState(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDOLineStatesPausedState(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDOLineStatesPausedState(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDOLineStatesDoneState(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDOLineStatesDoneState(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDOLineStatesDoneState(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDOLogicFamily(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDOLogicFamily(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDOLogicFamily(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDOUseOnlyOnBrdMem(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDOUseOnlyOnBrdMem(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDOUseOnlyOnBrdMem(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDODataXferMech(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDODataXferMech(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDODataXferMech(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDODataXferReqCond(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDODataXferReqCond(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDODataXferReqCond(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDOMemMapEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDOMemMapEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDOMemMapEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDOGenerateOn(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDOGenerateOn(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDOGenerateOn(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIMax(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIMax(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIMax(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIMin(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIMin(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIMin(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICustomScaleName(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCICustomScaleName(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCICustomScaleName(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIMeasType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqStartingEdge(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqStartingEdge(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqStartingEdge(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqMeasMeth(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqMeasMeth(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqMeasMeth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqMeasTime(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqMeasTime(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqMeasTime(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqDiv(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqDiv(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqDiv(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIFreqDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIFreqDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIFreqDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodStartingEdge(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodStartingEdge(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodStartingEdge(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodMeasMeth(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodMeasMeth(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodMeasMeth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodMeasTime(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodMeasTime(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodMeasTime(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodDiv(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodDiv(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodDiv(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPeriodDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPeriodDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPeriodDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesDir(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesDir(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesDir(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesDirTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesDirTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesDirTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesCountDirDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesCountDirDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesCountDirDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesCountDirDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesCountDirDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesCountDirDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesCountDirDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesCountDirDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesCountDirDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesCountDirDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesCountDirDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesCountDirDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesCountDirDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesCountDirDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesCountDirDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesInitialCnt(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesInitialCnt(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesInitialCnt(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesActiveEdge(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesActiveEdge(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesActiveEdge(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICountEdgesDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICountEdgesDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICountEdgesDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIAngEncoderUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIAngEncoderUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIAngEncoderUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIAngEncoderPulsesPerRev(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIAngEncoderPulsesPerRev(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIAngEncoderPulsesPerRev(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIAngEncoderInitialAngle(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIAngEncoderInitialAngle(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIAngEncoderInitialAngle(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCILinEncoderUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCILinEncoderUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCILinEncoderUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCILinEncoderDistPerPulse(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCILinEncoderDistPerPulse(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCILinEncoderDistPerPulse(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCILinEncoderInitialPos(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCILinEncoderInitialPos(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCILinEncoderInitialPos(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderDecodingType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderDecodingType(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderDecodingType(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderAInputTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderAInputTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderAInputTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderAInputDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderAInputDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderAInputDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderAInputDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderAInputDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderAInputDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderAInputDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderAInputDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderAInputDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderAInputDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderAInputDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderAInputDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderAInputDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderAInputDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderAInputDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderBInputTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderBInputTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderBInputTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderBInputDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderBInputDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderBInputDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderBInputDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderBInputDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderBInputDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderBInputDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderBInputDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderBInputDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderBInputDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderBInputDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderBInputDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderBInputDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderBInputDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderBInputDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderZInputTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderZInputTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderZInputTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderZInputDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderZInputDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderZInputDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderZInputDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderZInputDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderZInputDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderZInputDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderZInputDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderZInputDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderZInputDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderZInputDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderZInputDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderZInputDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderZInputDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderZInputDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderZIndexEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderZIndexEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderZIndexEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderZIndexVal(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderZIndexVal(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderZIndexVal(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIEncoderZIndexPhase(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIEncoderZIndexPhase(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIEncoderZIndexPhase(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPulseWidthUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPulseWidthUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPulseWidthUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPulseWidthTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPulseWidthTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPulseWidthTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPulseWidthStartingEdge(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPulseWidthStartingEdge(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPulseWidthStartingEdge(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPulseWidthDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPulseWidthDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPulseWidthDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPulseWidthDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPulseWidthDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPulseWidthDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPulseWidthDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPulseWidthDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPulseWidthDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPulseWidthDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPulseWidthDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPulseWidthDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPulseWidthDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPulseWidthDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPulseWidthDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepFirstTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepFirstTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepFirstTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepFirstEdge(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepFirstEdge(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepFirstEdge(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepFirstDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepFirstDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepFirstDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepFirstDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepFirstDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepFirstDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepFirstDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepFirstDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepFirstDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepFirstDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepFirstDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepFirstDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepFirstDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepFirstDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepFirstDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepSecondTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepSecondTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepSecondTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepSecondEdge(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepSecondEdge(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepSecondEdge(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepSecondDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepSecondDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepSecondDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepSecondDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepSecondDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepSecondDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepSecondDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepSecondDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepSecondDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepSecondDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepSecondDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepSecondDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITwoEdgeSepSecondDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITwoEdgeSepSecondDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCITwoEdgeSepSecondDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCISemiPeriodUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCISemiPeriodUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCISemiPeriodUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCISemiPeriodTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCISemiPeriodTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCISemiPeriodTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCISemiPeriodStartingEdge(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCISemiPeriodStartingEdge(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCISemiPeriodStartingEdge(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCISemiPeriodDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCISemiPeriodDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCISemiPeriodDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCISemiPeriodDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCISemiPeriodDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCISemiPeriodDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCISemiPeriodDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCISemiPeriodDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCISemiPeriodDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCISemiPeriodDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCISemiPeriodDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCISemiPeriodDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCISemiPeriodDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCISemiPeriodDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCISemiPeriodDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITimestampUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITimestampUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCITimestampUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCITimestampInitialSeconds(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCITimestampInitialSeconds(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCITimestampInitialSeconds(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIGPSSyncMethod(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIGPSSyncMethod(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIGPSSyncMethod(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIGPSSyncSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCIGPSSyncSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCIGPSSyncSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICtrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCICtrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCICtrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICtrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICtrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCICtrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICtrTimebaseActiveEdge(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICtrTimebaseActiveEdge(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICtrTimebaseActiveEdge(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICtrTimebaseDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICtrTimebaseDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICtrTimebaseDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICtrTimebaseDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICtrTimebaseDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCICtrTimebaseDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICtrTimebaseDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCICtrTimebaseDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCICtrTimebaseDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICtrTimebaseDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICtrTimebaseDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCICtrTimebaseDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICtrTimebaseDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICtrTimebaseDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICtrTimebaseDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCICount(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetCIOutputState(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetCITCReached(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetCICtrTimebaseMasterTimebaseDiv(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCICtrTimebaseMasterTimebaseDiv(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCICtrTimebaseMasterTimebaseDiv(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIDataXferMech(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIDataXferMech(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIDataXferMech(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCINumPossiblyInvalidSamps(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetCIDupCountPrevent(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIDupCountPrevent(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIDupCountPrevent(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCIPrescaler(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCIPrescaler(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCIPrescaler(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOOutputType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseIdleState(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseIdleState(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseIdleState(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseTerm(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseTerm(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseTerm(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseTimeUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseTimeUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseTimeUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseHighTime(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseHighTime(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseHighTime(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseLowTime(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseLowTime(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseLowTime(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseTimeInitialDelay(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseTimeInitialDelay(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseTimeInitialDelay(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseDutyCyc(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseDutyCyc(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseDutyCyc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseFreqUnits(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseFreqUnits(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseFreqUnits(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseFreq(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseFreq(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseFreq(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseFreqInitialDelay(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseFreqInitialDelay(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseFreqInitialDelay(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseHighTicks(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseHighTicks(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseHighTicks(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseLowTicks(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseLowTicks(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseLowTicks(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseTicksInitialDelay(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPulseTicksInitialDelay(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPulseTicksInitialDelay(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOCtrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCOCtrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCOCtrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOCtrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOCtrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCOCtrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOCtrTimebaseActiveEdge(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOCtrTimebaseActiveEdge(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOCtrTimebaseActiveEdge(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOCtrTimebaseDigFltrEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOCtrTimebaseDigFltrEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOCtrTimebaseDigFltrEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOCtrTimebaseDigFltrMinPulseWidth(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOCtrTimebaseDigFltrMinPulseWidth(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCOCtrTimebaseDigFltrMinPulseWidth(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOCtrTimebaseDigFltrTimebaseSrc(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetCOCtrTimebaseDigFltrTimebaseSrc(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetCOCtrTimebaseDigFltrTimebaseSrc(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOCtrTimebaseDigFltrTimebaseRate(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOCtrTimebaseDigFltrTimebaseRate(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetCOCtrTimebaseDigFltrTimebaseRate(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOCtrTimebaseDigSyncEnable(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOCtrTimebaseDigSyncEnable(TaskHandle, const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOCtrTimebaseDigSyncEnable(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOCount(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetCOOutputState(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetCOAutoIncrCnt(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOAutoIncrCnt(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOAutoIncrCnt(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOCtrTimebaseMasterTimebaseDiv(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOCtrTimebaseMasterTimebaseDiv(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOCtrTimebaseMasterTimebaseDiv(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPulseDone(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetCOConstrainedGenMode(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOConstrainedGenMode(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOConstrainedGenMode(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCOPrescaler(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetCOPrescaler(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetCOPrescaler(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetCORdyForNewVal(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetChanType(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanName(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetPhysicalChanName(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxGetChanDescr(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetChanDescr(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetChanDescr(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetChanIsGlobal(TaskHandle, const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAIConvClkOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedAIConvClkOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedAIConvClkOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAIConvClkPulsePolarity(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetExported10MHzRefClkOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExported10MHzRefClkOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExported10MHzRefClkOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExported20MHzTimebaseOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExported20MHzTimebaseOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExported20MHzTimebaseOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedSampClkOutputBehavior(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedSampClkOutputBehavior(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedSampClkOutputBehavior(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedSampClkOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedSampClkOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedSampClkOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedSampClkDelayOffset(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedSampClkDelayOffset(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedSampClkDelayOffset(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedSampClkPulsePolarity(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedSampClkPulsePolarity(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedSampClkPulsePolarity(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedSampClkTimebaseOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedSampClkTimebaseOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedSampClkTimebaseOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedDividedSampClkTimebaseOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedDividedSampClkTimebaseOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedDividedSampClkTimebaseOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAdvTrigOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedAdvTrigOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedAdvTrigOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAdvTrigPulsePolarity(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAdvTrigPulseWidthUnits(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedAdvTrigPulseWidthUnits(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedAdvTrigPulseWidthUnits(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAdvTrigPulseWidth(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedAdvTrigPulseWidth(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedAdvTrigPulseWidth(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedPauseTrigOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedPauseTrigOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedPauseTrigOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedPauseTrigLvlActiveLvl(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedPauseTrigLvlActiveLvl(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedPauseTrigLvlActiveLvl(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedRefTrigOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedRefTrigOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedRefTrigOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedRefTrigPulsePolarity(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedRefTrigPulsePolarity(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedRefTrigPulsePolarity(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedStartTrigOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedStartTrigOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedStartTrigOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedStartTrigPulsePolarity(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedStartTrigPulsePolarity(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedStartTrigPulsePolarity(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAdvCmpltEventOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedAdvCmpltEventOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedAdvCmpltEventOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAdvCmpltEventDelay(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedAdvCmpltEventDelay(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedAdvCmpltEventDelay(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAdvCmpltEventPulsePolarity(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedAdvCmpltEventPulsePolarity(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedAdvCmpltEventPulsePolarity(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAdvCmpltEventPulseWidth(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedAdvCmpltEventPulseWidth(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedAdvCmpltEventPulseWidth(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAIHoldCmpltEventOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedAIHoldCmpltEventOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedAIHoldCmpltEventOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedAIHoldCmpltEventPulsePolarity(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedAIHoldCmpltEventPulsePolarity(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedAIHoldCmpltEventPulsePolarity(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedChangeDetectEventOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedChangeDetectEventOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedChangeDetectEventOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedChangeDetectEventPulsePolarity(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedChangeDetectEventPulsePolarity(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedChangeDetectEventPulsePolarity(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedCtrOutEventOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedCtrOutEventOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedCtrOutEventOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedCtrOutEventOutputBehavior(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedCtrOutEventOutputBehavior(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedCtrOutEventOutputBehavior(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedCtrOutEventPulsePolarity(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedCtrOutEventPulsePolarity(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedCtrOutEventPulsePolarity(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedCtrOutEventToggleIdleState(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedCtrOutEventToggleIdleState(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedCtrOutEventToggleIdleState(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedHshkEventOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedHshkEventOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedHshkEventOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedHshkEventOutputBehavior(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedHshkEventOutputBehavior(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedHshkEventOutputBehavior(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedHshkEventDelay(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedHshkEventDelay(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedHshkEventDelay(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedHshkEventInterlockedAssertedLvl(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedHshkEventInterlockedAssertedLvl(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedHshkEventInterlockedAssertedLvl(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedHshkEventInterlockedAssertOnStart(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedHshkEventInterlockedAssertOnStart(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedHshkEventInterlockedAssertOnStart(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedHshkEventInterlockedDeassertDelay(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedHshkEventInterlockedDeassertDelay(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedHshkEventInterlockedDeassertDelay(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedHshkEventPulsePolarity(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedHshkEventPulsePolarity(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedHshkEventPulsePolarity(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedHshkEventPulseWidth(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedHshkEventPulseWidth(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedHshkEventPulseWidth(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedRdyForXferEventOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedRdyForXferEventOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedRdyForXferEventOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedRdyForXferEventLvlActiveLvl(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedRdyForXferEventLvlActiveLvl(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedRdyForXferEventLvlActiveLvl(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedRdyForXferEventDeassertCond(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedRdyForXferEventDeassertCond(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedRdyForXferEventDeassertCond(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedRdyForXferEventDeassertCondCustomThreshold(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedRdyForXferEventDeassertCondCustomThreshold(TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedRdyForXferEventDeassertCondCustomThreshold(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedDataActiveEventOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedDataActiveEventOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedDataActiveEventOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedDataActiveEventLvlActiveLvl(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedDataActiveEventLvlActiveLvl(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedDataActiveEventLvlActiveLvl(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedRdyForStartEventOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedRdyForStartEventOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedRdyForStartEventOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedRdyForStartEventLvlActiveLvl(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedRdyForStartEventLvlActiveLvl(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedRdyForStartEventLvlActiveLvl(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedSyncPulseEventOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedSyncPulseEventOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedSyncPulseEventOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetExportedWatchdogExpiredEventOutputTerm(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetExportedWatchdogExpiredEventOutputTerm(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetExportedWatchdogExpiredEventOutputTerm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDevIsSimulated(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevProductCategory(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevProductType(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevProductNum(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevSerialNum(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevChassisModuleDevNames(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAnlgTrigSupported(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevDigTrigSupported(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAIPhysicalChans(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAIMaxSingleChanRate(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAIMaxMultiChanRate(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAIMinRate(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAISimultaneousSamplingSupported(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAITrigUsage(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAIVoltageRngs(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAIVoltageIntExcitDiscreteVals(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAIVoltageIntExcitRangeVals(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAICurrentRngs(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAICurrentIntExcitDiscreteVals(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAIFreqRngs(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAIGains(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAICouplings(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAILowpassCutoffFreqDiscreteVals(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAILowpassCutoffFreqRangeVals(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAOPhysicalChans(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAOSampClkSupported(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAOMaxRate(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAOMinRate(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAOTrigUsage(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAOVoltageRngs(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAOCurrentRngs(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevAOGains(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevDILines(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevDIPorts(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevDIMaxRate(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevDITrigUsage(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevDOLines(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevDOPorts(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevDOMaxRate(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevDOTrigUsage(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCIPhysicalChans(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCITrigUsage(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCISampClkSupported(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCIMaxSize(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCIMaxTimebase(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCOPhysicalChans(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCOTrigUsage(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCOMaxSize(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCOMaxTimebase(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevBusType(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevNumDMAChans(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevPCIBusNum(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevPCIDevNum(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevPXIChassisNum(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevPXISlotNum(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCompactDAQChassisDevName(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetDevCompactDAQSlotNum(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetDevTerminals(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetReadRelativeTo(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetReadRelativeTo(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetReadRelativeTo(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetReadOffset(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetReadOffset(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetReadOffset(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetReadChannelsToRead(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetReadChannelsToRead(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetReadChannelsToRead(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetReadReadAllAvailSamp(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetReadReadAllAvailSamp(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetReadReadAllAvailSamp(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetReadAutoStart(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetReadAutoStart(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetReadAutoStart(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetReadOverWrite(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetReadOverWrite(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetReadOverWrite(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetReadCurrReadPos(TaskHandle, uInt64*) {
  return -1;
}

int32 __CFUNC DAQmxGetReadAvailSampPerChan(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetReadTotalSampPerChanAcquired(TaskHandle, uInt64*) {
  return -1;
}

int32 __CFUNC DAQmxGetReadOvercurrentChansExist(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetReadOvercurrentChans(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetReadOpenCurrentLoopChansExist(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetReadOpenCurrentLoopChans(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetReadOverloadedChansExist(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetReadOverloadedChans(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetReadChangeDetectHasOverflowed(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetReadRawDataWidth(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetReadNumChans(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetReadDigitalLinesBytesPerChan(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetReadWaitMode(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetReadWaitMode(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetReadWaitMode(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetReadSleepTime(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetReadSleepTime(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetReadSleepTime(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetRealTimeConvLateErrorsToWarnings(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetRealTimeConvLateErrorsToWarnings(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetRealTimeConvLateErrorsToWarnings(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetRealTimeNumOfWarmupIters(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetRealTimeNumOfWarmupIters(TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetRealTimeNumOfWarmupIters(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetRealTimeWaitForNextSampClkWaitMode(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetRealTimeWaitForNextSampClkWaitMode(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetRealTimeWaitForNextSampClkWaitMode(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetRealTimeReportMissedSamp(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetRealTimeReportMissedSamp(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetRealTimeReportMissedSamp(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetRealTimeWriteRecoveryMode(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetRealTimeWriteRecoveryMode(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetRealTimeWriteRecoveryMode(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanUsage(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSwitchChanUsage(const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanMaxACCarryCurrent(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanMaxACSwitchCurrent(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanMaxACCarryPwr(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanMaxACSwitchPwr(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanMaxDCCarryCurrent(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanMaxDCSwitchCurrent(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanMaxDCCarryPwr(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanMaxDCSwitchPwr(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanMaxACVoltage(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanMaxDCVoltage(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanWireMode(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanBandwidth(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchChanImpedance(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevSettlingTime(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetSwitchDevSettlingTime(const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevAutoConnAnlgBus(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSwitchDevAutoConnAnlgBus(const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevPwrDownLatchRelaysAfterSettling(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSwitchDevPwrDownLatchRelaysAfterSettling(const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevSettled(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevRelayList(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevNumRelays(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevSwitchChanList(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevNumSwitchChans(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevNumRows(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevNumColumns(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchDevTopology(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchScanBreakMode(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSwitchScanBreakMode(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetSwitchScanBreakMode(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchScanRepeatMode(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSwitchScanRepeatMode(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetSwitchScanRepeatMode(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSwitchScanWaitingForAdv(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleDescr(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetScaleDescr(const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleScaledUnits(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetScaleScaledUnits(const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxGetScalePreScaledUnits(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetScalePreScaledUnits(const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleType(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleLinSlope(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetScaleLinSlope(const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleLinYIntercept(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetScaleLinYIntercept(const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleMapScaledMax(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetScaleMapScaledMax(const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleMapPreScaledMax(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetScaleMapPreScaledMax(const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleMapScaledMin(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetScaleMapScaledMin(const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleMapPreScaledMin(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetScaleMapPreScaledMin(const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxGetScalePolyForwardCoeff(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetScalePolyForwardCoeff(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetScalePolyReverseCoeff(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetScalePolyReverseCoeff(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleTableScaledVals(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetScaleTableScaledVals(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetScaleTablePreScaledVals(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetScaleTablePreScaledVals(const char[], float64*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetSysGlobalChans(char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetSysScales(char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetSysTasks(char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetSysDevNames(char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetSysNIDAQMajorVersion(uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetSysNIDAQMinorVersion(uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetTaskName(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetTaskChannels(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetTaskNumChans(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetTaskDevices(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetTaskNumDevices(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetTaskComplete(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetSampQuantSampMode(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampQuantSampMode(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampQuantSampMode(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampQuantSampPerChan(TaskHandle, uInt64*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampQuantSampPerChan(TaskHandle, uInt64) {
  return -1;
}

int32 __CFUNC DAQmxResetSampQuantSampPerChan(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampTimingType(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampTimingType(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampTimingType(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkRate(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkRate(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkMaxRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkActiveEdge(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkActiveEdge(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkActiveEdge(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkUnderflowBehavior(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkUnderflowBehavior(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkUnderflowBehavior(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkTimebaseDiv(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkTimebaseDiv(TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkTimebaseDiv(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkTimebaseRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkTimebaseRate(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkTimebaseRate(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkTimebaseSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkTimebaseSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkTimebaseSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkTimebaseActiveEdge(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkTimebaseActiveEdge(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkTimebaseActiveEdge(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkTimebaseMasterTimebaseDiv(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkTimebaseMasterTimebaseDiv(TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkTimebaseMasterTimebaseDiv(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkDigFltrEnable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkDigFltrEnable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkDigFltrEnable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkDigFltrMinPulseWidth(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkDigFltrMinPulseWidth(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkDigFltrMinPulseWidth(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkDigFltrTimebaseSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkDigFltrTimebaseSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkDigFltrTimebaseSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkDigFltrTimebaseRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkDigFltrTimebaseRate(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkDigFltrTimebaseRate(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkDigSyncEnable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkDigSyncEnable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkDigSyncEnable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetHshkDelayAfterXfer(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetHshkDelayAfterXfer(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetHshkDelayAfterXfer(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetHshkStartCond(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetHshkStartCond(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetHshkStartCond(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetHshkSampleInputDataWhen(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetHshkSampleInputDataWhen(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetHshkSampleInputDataWhen(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetChangeDetectDIRisingEdgePhysicalChans(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetChangeDetectDIRisingEdgePhysicalChans(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetChangeDetectDIRisingEdgePhysicalChans(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetChangeDetectDIFallingEdgePhysicalChans(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetChangeDetectDIFallingEdgePhysicalChans(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetChangeDetectDIFallingEdgePhysicalChans(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetOnDemandSimultaneousAOEnable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetOnDemandSimultaneousAOEnable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetOnDemandSimultaneousAOEnable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIConvRate(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIConvRate(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvRateEx(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIConvRateEx(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAIConvRateEx(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvMaxRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvMaxRateEx(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIConvSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAIConvSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvSrcEx(TaskHandle, const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAIConvSrcEx(TaskHandle, const char[], const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAIConvSrcEx(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvActiveEdge(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIConvActiveEdge(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIConvActiveEdge(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvActiveEdgeEx(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIConvActiveEdgeEx(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIConvActiveEdgeEx(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvTimebaseDiv(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIConvTimebaseDiv(TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIConvTimebaseDiv(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvTimebaseDivEx(TaskHandle, const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIConvTimebaseDivEx(TaskHandle, const char[], uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIConvTimebaseDivEx(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvTimebaseSrc(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIConvTimebaseSrc(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIConvTimebaseSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAIConvTimebaseSrcEx(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAIConvTimebaseSrcEx(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAIConvTimebaseSrcEx(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDelayFromSampClkDelayUnits(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDelayFromSampClkDelayUnits(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDelayFromSampClkDelayUnits(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDelayFromSampClkDelayUnitsEx(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDelayFromSampClkDelayUnitsEx(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDelayFromSampClkDelayUnitsEx(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetDelayFromSampClkDelay(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetDelayFromSampClkDelay(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetDelayFromSampClkDelay(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDelayFromSampClkDelayEx(TaskHandle, const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetDelayFromSampClkDelayEx(TaskHandle, const char[], float64) {
  return -1;
}

int32 __CFUNC DAQmxResetDelayFromSampClkDelayEx(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetMasterTimebaseRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetMasterTimebaseRate(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetMasterTimebaseRate(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetMasterTimebaseSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetMasterTimebaseSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetMasterTimebaseSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetRefClkRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetRefClkRate(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetRefClkRate(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetRefClkSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetRefClkSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetRefClkSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSyncPulseSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetSyncPulseSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetSyncPulseSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSyncPulseSyncTime(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetSyncPulseMinDelayToStart(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetSyncPulseMinDelayToStart(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetSyncPulseMinDelayToStart(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetSampTimingEngine(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampTimingEngine(TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampTimingEngine(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetStartTrigType(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetStartTrigType(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetStartTrigType(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeStartTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeStartTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeStartTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeStartTrigEdge(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeStartTrigEdge(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeStartTrigEdge(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeStartTrigDigFltrEnable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeStartTrigDigFltrEnable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeStartTrigDigFltrEnable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeStartTrigDigFltrMinPulseWidth(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeStartTrigDigFltrMinPulseWidth(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeStartTrigDigFltrMinPulseWidth(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeStartTrigDigFltrTimebaseSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeStartTrigDigFltrTimebaseSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeStartTrigDigFltrTimebaseSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeStartTrigDigFltrTimebaseRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeStartTrigDigFltrTimebaseRate(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeStartTrigDigFltrTimebaseRate(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeStartTrigDigSyncEnable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeStartTrigDigSyncEnable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeStartTrigDigSyncEnable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigPatternStartTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigPatternStartTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigPatternStartTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigPatternStartTrigPattern(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigPatternStartTrigPattern(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigPatternStartTrigPattern(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigPatternStartTrigWhen(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigPatternStartTrigWhen(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigPatternStartTrigWhen(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgEdgeStartTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgEdgeStartTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgEdgeStartTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgEdgeStartTrigSlope(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgEdgeStartTrigSlope(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgEdgeStartTrigSlope(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgEdgeStartTrigLvl(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgEdgeStartTrigLvl(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgEdgeStartTrigLvl(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgEdgeStartTrigHyst(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgEdgeStartTrigHyst(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgEdgeStartTrigHyst(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgEdgeStartTrigCoupling(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgEdgeStartTrigCoupling(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgEdgeStartTrigCoupling(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinStartTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinStartTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinStartTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinStartTrigWhen(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinStartTrigWhen(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinStartTrigWhen(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinStartTrigTop(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinStartTrigTop(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinStartTrigTop(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinStartTrigBtm(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinStartTrigBtm(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinStartTrigBtm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinStartTrigCoupling(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinStartTrigCoupling(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinStartTrigCoupling(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetStartTrigDelay(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetStartTrigDelay(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetStartTrigDelay(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetStartTrigDelayUnits(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetStartTrigDelayUnits(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetStartTrigDelayUnits(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetStartTrigRetriggerable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetStartTrigRetriggerable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetStartTrigRetriggerable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetRefTrigType(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetRefTrigType(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetRefTrigType(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetRefTrigPretrigSamples(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxSetRefTrigPretrigSamples(TaskHandle, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxResetRefTrigPretrigSamples(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeRefTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeRefTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeRefTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeRefTrigEdge(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeRefTrigEdge(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeRefTrigEdge(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigPatternRefTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigPatternRefTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigPatternRefTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigPatternRefTrigPattern(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigPatternRefTrigPattern(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigPatternRefTrigPattern(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigPatternRefTrigWhen(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigPatternRefTrigWhen(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigPatternRefTrigWhen(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgEdgeRefTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgEdgeRefTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgEdgeRefTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgEdgeRefTrigSlope(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgEdgeRefTrigSlope(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgEdgeRefTrigSlope(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgEdgeRefTrigLvl(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgEdgeRefTrigLvl(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgEdgeRefTrigLvl(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgEdgeRefTrigHyst(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgEdgeRefTrigHyst(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgEdgeRefTrigHyst(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgEdgeRefTrigCoupling(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgEdgeRefTrigCoupling(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgEdgeRefTrigCoupling(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinRefTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinRefTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinRefTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinRefTrigWhen(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinRefTrigWhen(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinRefTrigWhen(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinRefTrigTop(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinRefTrigTop(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinRefTrigTop(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinRefTrigBtm(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinRefTrigBtm(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinRefTrigBtm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinRefTrigCoupling(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinRefTrigCoupling(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinRefTrigCoupling(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAdvTrigType(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAdvTrigType(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAdvTrigType(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeAdvTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeAdvTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeAdvTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeAdvTrigEdge(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeAdvTrigEdge(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeAdvTrigEdge(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeAdvTrigDigFltrEnable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeAdvTrigDigFltrEnable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeAdvTrigDigFltrEnable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetHshkTrigType(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetHshkTrigType(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetHshkTrigType(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetInterlockedHshkTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetInterlockedHshkTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetInterlockedHshkTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetInterlockedHshkTrigAssertedLvl(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetInterlockedHshkTrigAssertedLvl(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetInterlockedHshkTrigAssertedLvl(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetPauseTrigType(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetPauseTrigType(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetPauseTrigType(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgLvlPauseTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgLvlPauseTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgLvlPauseTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgLvlPauseTrigWhen(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgLvlPauseTrigWhen(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgLvlPauseTrigWhen(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgLvlPauseTrigLvl(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgLvlPauseTrigLvl(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgLvlPauseTrigLvl(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgLvlPauseTrigHyst(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgLvlPauseTrigHyst(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgLvlPauseTrigHyst(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgLvlPauseTrigCoupling(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgLvlPauseTrigCoupling(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgLvlPauseTrigCoupling(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinPauseTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinPauseTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinPauseTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinPauseTrigWhen(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinPauseTrigWhen(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinPauseTrigWhen(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinPauseTrigTop(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinPauseTrigTop(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinPauseTrigTop(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinPauseTrigBtm(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinPauseTrigBtm(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinPauseTrigBtm(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetAnlgWinPauseTrigCoupling(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetAnlgWinPauseTrigCoupling(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetAnlgWinPauseTrigCoupling(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigLvlPauseTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigLvlPauseTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigLvlPauseTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigLvlPauseTrigWhen(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigLvlPauseTrigWhen(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigLvlPauseTrigWhen(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigLvlPauseTrigDigFltrEnable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigLvlPauseTrigDigFltrEnable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigLvlPauseTrigDigFltrEnable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigLvlPauseTrigDigFltrMinPulseWidth(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigLvlPauseTrigDigFltrMinPulseWidth(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetDigLvlPauseTrigDigFltrMinPulseWidth(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigLvlPauseTrigDigFltrTimebaseSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigLvlPauseTrigDigFltrTimebaseSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigLvlPauseTrigDigFltrTimebaseSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigLvlPauseTrigDigFltrTimebaseRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigLvlPauseTrigDigFltrTimebaseRate(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetDigLvlPauseTrigDigFltrTimebaseRate(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigLvlPauseTrigDigSyncEnable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigLvlPauseTrigDigSyncEnable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigLvlPauseTrigDigSyncEnable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigPatternPauseTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigPatternPauseTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigPatternPauseTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigPatternPauseTrigPattern(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigPatternPauseTrigPattern(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigPatternPauseTrigPattern(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigPatternPauseTrigWhen(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigPatternPauseTrigWhen(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigPatternPauseTrigWhen(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetArmStartTrigType(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetArmStartTrigType(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetArmStartTrigType(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeArmStartTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeArmStartTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeArmStartTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeArmStartTrigEdge(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeArmStartTrigEdge(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeArmStartTrigEdge(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeArmStartTrigDigFltrEnable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeArmStartTrigDigFltrEnable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeArmStartTrigDigFltrEnable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeArmStartTrigDigFltrMinPulseWidth(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeArmStartTrigDigFltrMinPulseWidth(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeArmStartTrigDigFltrMinPulseWidth(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeArmStartTrigDigFltrTimebaseSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeArmStartTrigDigFltrTimebaseSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeArmStartTrigDigFltrTimebaseSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeArmStartTrigDigFltrTimebaseRate(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeArmStartTrigDigFltrTimebaseRate(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeArmStartTrigDigFltrTimebaseRate(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeArmStartTrigDigSyncEnable(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeArmStartTrigDigSyncEnable(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeArmStartTrigDigSyncEnable(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetWatchdogTimeout(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetWatchdogTimeout(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetWatchdogTimeout(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetWatchdogExpirTrigType(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetWatchdogExpirTrigType(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetWatchdogExpirTrigType(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeWatchdogExpirTrigSrc(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeWatchdogExpirTrigSrc(TaskHandle, const char*) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeWatchdogExpirTrigSrc(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetDigEdgeWatchdogExpirTrigEdge(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetDigEdgeWatchdogExpirTrigEdge(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetDigEdgeWatchdogExpirTrigEdge(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetWatchdogDOExpirState(TaskHandle, const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetWatchdogDOExpirState(TaskHandle, const char[], int32) {
  return -1;
}

int32 __CFUNC DAQmxResetWatchdogDOExpirState(TaskHandle, const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetWatchdogHasExpired(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteRelativeTo(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetWriteRelativeTo(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetWriteRelativeTo(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteOffset(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetWriteOffset(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetWriteOffset(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteRegenMode(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetWriteRegenMode(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetWriteRegenMode(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteCurrWritePos(TaskHandle, uInt64*) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteOvercurrentChansExist(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteOvercurrentChans(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteOpenCurrentLoopChansExist(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteOpenCurrentLoopChans(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetWritePowerSupplyFaultChansExist(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetWritePowerSupplyFaultChans(TaskHandle, char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteSpaceAvail(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteTotalSampPerChanGenerated(TaskHandle, uInt64*) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteRawDataWidth(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteNumChans(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteWaitMode(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetWriteWaitMode(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetWriteWaitMode(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteSleepTime(TaskHandle, float64*) {
  return -1;
}

int32 __CFUNC DAQmxSetWriteSleepTime(TaskHandle, float64) {
  return -1;
}

int32 __CFUNC DAQmxResetWriteSleepTime(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteNextWriteIsLast(TaskHandle, bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetWriteNextWriteIsLast(TaskHandle, bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetWriteNextWriteIsLast(TaskHandle) {
  return -1;
}

int32 __CFUNC DAQmxGetWriteDigitalLinesBytesPerChan(TaskHandle, uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanAITermCfgs(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanAOTermCfgs(const char[], int32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanAOManualControlEnable(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxSetPhysicalChanAOManualControlEnable(const char[], bool32) {
  return -1;
}

int32 __CFUNC DAQmxResetPhysicalChanAOManualControlEnable(const char[]) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanAOManualControlAmplitude(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanAOManualControlFreq(const char[], float64*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanDIPortWidth(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanDISampClkSupported(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanDIChangeDetectSupported(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanDOPortWidth(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanDOSampClkSupported(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanTEDSMfgID(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanTEDSModelNum(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanTEDSSerialNum(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanTEDSVersionNum(const char[], uInt32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanTEDSVersionLetter(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanTEDSBitStream(const char[], uInt8*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetPhysicalChanTEDSTemplateIDs(const char[], uInt32*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetPersistedTaskAuthor(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetPersistedTaskAllowInteractiveEditing(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPersistedTaskAllowInteractiveDeletion(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPersistedChanAuthor(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetPersistedChanAllowInteractiveEditing(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPersistedChanAllowInteractiveDeletion(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPersistedScaleAuthor(const char[], char*, uInt32) {
  return -1;
}

int32 __CFUNC DAQmxGetPersistedScaleAllowInteractiveEditing(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetPersistedScaleAllowInteractiveDeletion(const char[], bool32*) {
  return -1;
}

int32 __CFUNC DAQmxGetSampClkTimingResponseMode(TaskHandle, int32*) {
  return -1;
}

int32 __CFUNC DAQmxSetSampClkTimingResponseMode(TaskHandle, int32) {
  return -1;
}

int32 __CFUNC DAQmxResetSampClkTimingResponseMode(TaskHandle) {
  return -1;
}

