// pvp/pvpCamera.cpp - This file is part of VScope.
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

// pvpCamera.cpp

#include <pvp/pvpCamera.h>
#include <pvp/pvpSystem.h>
#include <stdio.h>
#include <pvp/ccdtrigmode.h>
#include <pvp/ccdbinning.h>
#include <pvp/ccdregion.h>
#include <pvp/dwpvcam.h>
#include <base/dbg.h>

pvpCamera::pvpCamera(QString camname):
  camname(camname) {
  pvpSystem(); // ensure system is initialized
  
  int16 hcam;
  if (!pl_cam_open(camname.toUtf8().data(), &hcam, OPEN_EXCLUSIVE))
    throw pvpException("pvpCamera: Could not open camera",camname);
  camh = hcam;

  serno = getHeadSerNumAlpha();

  try {
    setPmode(Pmode::Ft);
  } catch (pvpException const &) {
    Dbg() << "Could not set Pmode on camera";
  }
  setReadoutPort(ReadoutPort());
  setSpdtabIndex(0);
  setGainIndex(1);
  setClearCycles(2);
  setBofEofEnable(BofEofEnable::EndFrameIrqs);
  setBofEofClr(true);
  abort();
  Dbg() << "pvpcamera" << camname << "opened";
  QMap<int, QString> eomodes = getEnumeration(PARAM_EXPOSURE_MODE);
  foreach (int k, eomodes.keys()) {
    Dbg() << "mode" << k << "is" << eomodes[k];
  }

  bool haveextmode = availExposeOutMode();
  if (haveextmode) {
    Dbg() << "have ext mode";
    QMap<int, QString> eomodes = getEnumeration(PARAM_EXPOSE_OUT_MODE);
    foreach (int k, eomodes.keys()) {
      Dbg() << "mode" << k << "is" << eomodes[k];
    }
  } else {
    Dbg() << "no ext mode";
  }


  initializeResIndex();
}

void pvpCamera::initializeResIndex() {
  int N = countExpResIndex();
  expres.fill(0, N);
  for (int n=0; n<N; n++) {
    setExpResIndex(n);
    ExpRes e = getExpRes();
    switch (e) {
    case ExpRes::Millisec:
      expres[n] = 1000;
      break;
    case ExpRes::Microsec:
      expres[n] = 1;
      break;
    default:
      throw pvpException("pvpCamera (constructor): unknown exposure resolution");
    }
  }

}

pvpCamera::~pvpCamera() {
  if (!pl_cam_close(camh))
    fprintf(stderr,"pvpCamera: Close failed\n");
}

QString pvpCamera::getSerialNumber() const {
  return serno;
}

QString pvpCamera::getDriverVersion() {
  if (!availDdVersion())
    return "?";
  int v = getDdVersion();
  return QString("%1.%2.%3").arg((v>>8)&0xff).arg((v>>4)&0x0f).arg(v&0x0f);
}

QString pvpCamera::getCameraChipName() {
  return availChipName() ? getChipName() : "?";
}

void pvpCamera::reportStatus() {
  printf("Status report for camera %s\n",camname.toUtf8().data());
  printf("Chip: %s\n", getCameraChipName().toUtf8().data());
  printf("Ser no: %s\n", serno.toUtf8().data());
  printf("Driver vsn: %s\n", getDriverVersion().toUtf8().data());
  printf("Camera handle: 0x%04x\n",camh);
  printf("Class 0 parameters:\n");
  reportClass0();
  printf("Class 2 parameters:\n");
  reportClass2();
  printf("Class 3 parameters:\n");
  reportClass3();
  printf("End of camera status report\n");
}

int pvpCamera::countPorts() {
  return countReadoutPort();
}

int pvpCamera::countSpeeds(int port) {
  if (port<0 || port>=countPorts())
    return 0;
  int k0 = getReadoutPort();
  int n0 = getSpdtabIndex();
  setReadoutPort(port);
  int N = countSpdtabIndex();
  setReadoutPort(k0);
  setSpdtabIndex(n0); // this may not have been changed, but I want to be sure
  return N;
}

int pvpCamera::readoutTime(int port, int spdidx) {
  int k0 = getReadoutPort();
  int n0 = getSpdtabIndex();
  setPortAndSpeed(port, spdidx);
  int t = readoutTime();
  setPortAndSpeed(k0, n0);
  return t;
}

int pvpCamera::readoutTime() {
  return getPixTime();
}

QMap<int, QString> pvpCamera::getEnumeration(int param) {
  QMap<int, QString> res;
  uns32 N;
  if (!pl_get_param(camh, param, ATTR_COUNT, (void*)&N)) {
    Dbg() << "Could not enumerate" << param;
    return res;
  }
  for (uns32 n=0; n<N; n++) {
    char name[256];
    int32 value;
    if (pl_get_enum_param(camh, param, n, &value, name, 255)) {
      res[value] = name;
      Dbg() << "enumeration for" << param << "at" << n << ":" << value << "->" << name;
    } else {
      Dbg() << "Error enumerating" << param << "at" << n;
      return res;
    }
  }
  return res;
}

QMap<int, QString> pvpCamera::enumeratePorts() {
  return getEnumeration(PARAM_READOUT_PORT);
}

void pvpCamera::setPortAndSpeed(int port, int spdidx) {
  Dbg() << "setportandspeed" << port << spdidx;
  if (port<0 || port>=countPorts())
    throw pvpException("Nonexistent readout port requested");
  if (spdidx<0 || spdidx>=countSpeeds(port))
    throw pvpException("Nonexistent speed index requested");
  setReadoutPort(port);
  setSpdtabIndex(spdidx);
  setGainIndex(1);
}

void pvpCamera::reportSpeeds() {
  int K = countReadoutPort();
  int k0 = getReadoutPort();
  int n0 = getSpdtabIndex();
  printf("Camera %s: Number of readout ports: %i. Current: %i\n",
         qPrintable(camname), K, k0);
  for (int k = 0; k<K; k++) {
    printf("Readoutport %i:\n", k);
    setReadoutPort(k);
    printf("    ReadoutPort: %s\n",getReadoutPort().decode());
    printf("    MaxRdPorts:  %s\n",maxReadoutPort().decode());
    printf("    CntRdPorts:  %i\n",countReadoutPort());
    int N = countSpdtabIndex();
    int n0 = getSpdtabIndex();
    printf("  Camera %s, port %i: Number of speed tab values: %i\n",
           qPrintable(camname), k, N);
    for (int n=0; n<N; n++) {
      printf("  Speed %i:\n",n);
      setSpdtabIndex(n);
      printf("    BitDepth:    %i\n",getBitDepth());
      printf("    PixTime:     %i\n",getPixTime());
      printf("    GainIndex:   %i\n",getGainIndex());
      printf("    MaxGainIdx:  %i\n",maxGainIndex());
      printf("    CntGainIdx:  %i\n",countGainIndex());
      if (availGainMultEnable()) {
        printf("    GainMultEnable: %i\n", getGainMultEnable());
        printf("    Min GME: %i\n", minGainMultEnable());
        printf("    Max GME: %i\n", maxGainMultEnable());
      } else { 
        printf("    GainMultEnable: N/A\n");
      }
      printf("    GainMultFactor: %i\n", getGainMultFactor());
      printf("    Min GMF: %i\n", minGainMultFactor());
      printf("    Max GMF: %i\n", maxGainMultFactor());
    }
    setSpdtabIndex(n0);
    printf("  Restored original speed: %i\n",n0);
  }
  setReadoutPort(k0);
  printf("Restored original port: %i\n",k0);
  setSpdtabIndex(n0);
  printf("  Restored original speed: %i\n",n0);
  fflush(stdout);
}


int pvpCamera::pvpTrigMode(CCDTrigMode const &tm) {
  switch (tm) {
  case CCDTrigMode::Immediate:
    return TIMED_MODE;
  case CCDTrigMode::FirstFrame:
    return TRIGGER_FIRST_MODE;
  case CCDTrigMode::EachFrame:
    return STROBED_MODE;
  case CCDTrigMode::Bulb:
    return BULB_MODE;
  default:
    throw pvpException("pvpCamera: unknown trigger mode");
  }
}

rgn_type pvpCamera::pvpRegion(CCDRegion const &region,
			      CCDBinning const &bin) {
  rgn_type rgn;
  rgn.s1 = region.smin;
  rgn.s2 = region.smax;
  rgn.p1 = region.pmin;
  rgn.p2 = region.pmax;
  rgn.sbin = bin.sbin;
  rgn.pbin = bin.pbin;
  return rgn;
}

int32 pvpCamera::bestExposureTime(int32 t_us) {
  int32 t_ms = t_us/1000; // round down to whole milliseconds
  return 1000*t_ms;
}

int32 pvpCamera::pvpExposureTime(int32 t_us, int32 *reso_us_out) {
  int best_reso_idx = 0;
  for (int n=0; n<expres.size(); n++) {
    //if (expres[n] > t_ms/1e4 && expres[n]<expres[best_reso_idx]) {
    if (expres[n] > 100 && expres[n] < 10000) {
      // Select the one that is 1 ms; the 1 us one is broken in the pvcam
      // library.
      best_reso_idx = n;
    }
  }

  setExpResIndex(best_reso_idx);

  int32 expotime = t_us / expres[best_reso_idx];

  if (reso_us_out)
    *reso_us_out = expres[best_reso_idx];

  return expotime;
}

size_t pvpCamera::configFinite(rgn_type const &rgn, int trigmode,
			       int exposetime, int nframes) {

  uns32 strmsize;
  Dbg() <<"setup_seq" << nframes << trigmode << exposetime;
  if (!pl_exp_setup_seq(camh,
			nframes,
			1, &rgn,
			trigmode,
			exposetime,
			&strmsize))
    throw pvpException("pvpCamera: Cannot setup finite sequence");
  Dbg() << "streamsize" << strmsize;
  Dbg() << "expomode" << getExposureMode();
  int16 st;
  uns32 bt;
  if (pl_exp_check_status(camh, &st, &bt))
    Dbg() << "got status" << st << bt;
  else
    Dbg() << "didn't get status";
  Dbg() << "smart" << getSmartStreamModeEnabled() << getSmartStreamMode();
  return strmsize/2;
}

size_t pvpCamera::configContinuous(rgn_type const &rgn, int trigmode,
				   int exposetime) {
  uns32 strmsize;
  if (!pl_exp_setup_cont(camh,
			 1, &rgn,
			 trigmode,
			 exposetime,
			 &strmsize,
			 CIRC_OVERWRITE))
    throw pvpException("pvpCamera: Cannot setup continuous sequence");
  Dbg() << "cont expomode" << getExposureMode() << trigmode;
  return strmsize/2;
}

void pvpCamera::startFinite(uint16_t *dest) {  
  if (!pl_exp_start_seq(camh, (void*)dest))
    throw pvpException("pvpCamera: Cannot start finite acquisition");
  int16 st;
  uns32 bt;
  if (pl_exp_check_status(camh, &st, &bt))
    Dbg() << "got status" << st << bt;
  else
    Dbg() << "didn't get status";
}

void pvpCamera::finishFinite(uint16_t *dest) {
  if (!pl_exp_finish_seq(camh, (void*)dest, 0))
    throw pvpException("pvpCamera: Cannot finish finite acquisition");
}

void pvpCamera::startContinuous(uint16_t *destbuf, size_t npixinbuf) {
  if (!pl_exp_start_cont(camh, (void*)destbuf,2*npixinbuf))
    throw pvpException("pvpCamera: Cannot start continuous acquisition");
  Dbg() << "start cont";
}

void pvpCamera::stopContinuous() {
  if (!pl_exp_stop_cont(camh,CCS_HALT))
    throw pvpException("pvpCamera: Cannot stop acquisition");
}

void pvpCamera::abort() {
  if (!pl_exp_abort(camh,CCS_CLEAR))
    throw pvpException("pvpCamera: Cannot abort acquisition");
}

pvpCamera::Status pvpCamera::getContinuousStatus() {
  Dbg() << "getContinuousStatus" << camh;
  int16 status;
  uns32 bytecount;
  uns32 bufcount;
  if (!pl_exp_check_cont_status(camh, &status, &bytecount, &bufcount))
    throw pvpException("pvpCamera: check_status failed 1");
  Dbg() << "gotContinuousStatus" << status << bytecount << bufcount;
  switch (status) {
  case READOUT_NOT_ACTIVE: return NotActive;
  case EXPOSURE_IN_PROGRESS: return Acquiring;
  case READOUT_IN_PROGRESS: return Acquiring;
  case READOUT_COMPLETE: return Completed; // for continuous there is really no such thing as Completed.
  case READOUT_FAILED: return Failed;
  case ACQUISITION_IN_PROGRESS: return Acquiring;
  default: throw pvpException("pvpCamera: unknown acquisition status");
  }
}

pvpCamera::Status pvpCamera::getFiniteStatus() {
  Dbg() << "getFiniteStatus" << camh;
  int16 status;
  uns32 bytecount;
  if (!pl_exp_check_status(camh, &status, &bytecount))
    throw pvpException("pvpCamera: check_status failed 2");
Dbg() << "gotFiniteStatus" << status << bytecount;

  switch (status) {
  case READOUT_NOT_ACTIVE: return NotActive;
  case EXPOSURE_IN_PROGRESS: return Acquiring;
  case READOUT_IN_PROGRESS: return Acquiring;
  case READOUT_COMPLETE: return Completed;
  case READOUT_FAILED: return Failed;
  case ACQUISITION_IN_PROGRESS: return Acquiring;
  default: throw pvpException("pvpCamera: unknown acquisition status");
  }
}

size_t pvpCamera::nPixelsSoFarFinite() {
  int16 status;
  uns32 bytecount;
  if (!pl_exp_check_status(camh, &status, &bytecount))
    throw pvpException("pvpCamera: check_status failed 3");
  return bytecount/2;
}

size_t pvpCamera::nPixelsSoFarContinuous(size_t npix_in_buf) {
  int16 status;
  uns32 bytecount;
  uns32 bufcount;
  if (!pl_exp_check_cont_status(camh, &status, &bytecount, &bufcount))
    throw pvpException("pvpCamera: check_status failed 4");
  return bytecount/2 + npix_in_buf*bufcount;
}

int pvpCamera::haveNewFrame() {
  int16 status;
  uns32 bytecount;
  uns32 bufcount;
  if (!pl_exp_check_cont_status(camh, &status, &bytecount, &bufcount))
    throw pvpException("pvpCamera: check_status failed 5");
  if (status==READOUT_FAILED)
    return -1;
  else if (status==FRAME_AVAILABLE)
    return 1;
  else
    return 0;
}

uint16_t *pvpCamera::getLatestFrame() {
  void *ptr;
  if (!pl_exp_get_latest_frame(camh, &ptr))
    throw pvpException("pvpCamera: cannot get latest frame");
  return (uint16_t *)ptr;
}

  
