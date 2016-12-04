// pvpCamera.cpp

#include <pvp/pvpCamera.h>
#include <pvp/pvpSystem.h>
#include <stdio.h>
#include <pvp/ccdtrigmode.h>
#include <pvp/ccdbinning.h>
#include <pvp/ccdregion.h>
#include <pvp/dwpvcam.h>
#include <base/dbg.h>

pvpCamera::pvpCamera(QString camname) throw(pvpException):
  camname(camname) {
  pvpSystem(); // ensure system is initialized
  
  int16 hcam;
  if (!pl_cam_open(camname.toUtf8().data(), &hcam, OPEN_EXCLUSIVE))
    throw pvpException("pvpCamera: Could not open camera",camname);
  camh = hcam;

  camchipname = availChipName() ? getChipName() : "?";
  serno = getHeadSerNumAlpha();

  setSpdtabIndex(0);
  setReadoutPort(ReadoutPort::X1); // was Port2
  setGainIndex(1);

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

QString pvpCamera::getDriverVersion() const {
  return drivervsn;
}

QString pvpCamera::getCameraChipName() const {
  return camchipname;
}

void pvpCamera::reportStatus() throw(pvpException) {
  printf("Status report for camera %s\n",camname.toUtf8().data());
  printf("Chip: %s\n", camchipname.toUtf8().data());
  printf("Ser no: %s\n", serno.toUtf8().data());
  printf("Driver vsn: %s\n", drivervsn.toUtf8().data());
  printf("Camera handle: 0x%04x\n",camh);
  printf("Class 0 parameters:\n");
  reportClass0();
  printf("Class 2 parameters:\n");
  reportClass2();
  printf("Class 3 parameters:\n");
  reportClass3();
  printf("End of camera status report\n");
}

void pvpCamera::reportSpeeds() throw(pvpException) {
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
    }
    setSpdtabIndex(n0);
    printf("  Restored original speed: %i\n",n0);
  }
  setReadoutPort(k0);
  printf("Restored original port: %i\n",k0);
  setSpdtabIndex(n0);
  printf("  Restored original speed: %i\n",n0);
}


int pvpCamera::pvpTrigMode(CCDTrigMode const &tm) throw(pvpException) {
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
			      CCDBinning const &bin) throw(pvpException) {
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

int32 pvpCamera::pvpExposureTime(int32 t_us, int32 *reso_us_out)
  throw(pvpException) {
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
  if (!pl_exp_setup_seq(camh,
			nframes,
			1, &rgn,
			trigmode,
			exposetime,
			&strmsize))
    throw pvpException("pvpCamera: Cannot setup finite sequence");
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
  return strmsize/2;
}

void pvpCamera::startFinite(uint16_t *dest) {
  if (!pl_exp_start_seq(camh, (void*)dest))
    throw pvpException("pvpCamera: Cannot start finite acquisition");
}

void pvpCamera::startContinuous(uint16_t *destbuf, size_t npixinbuf) {
  if (!pl_exp_start_cont(camh, (void*)destbuf,2*npixinbuf))
    throw pvpException("pvpCamera: Cannot start continuous acquisition");
}

void pvpCamera::stopContinuous() {
  if (!pl_exp_stop_cont(camh,CCS_HALT))
    throw pvpException("pvpCamera: Cannot stop acquisition");
}

void pvpCamera::abort() {
  if (!pl_exp_abort(camh,CCS_HALT))
    throw pvpException("pvpCamera: Cannot abort acquisition");
}

pvpCamera::Status pvpCamera::getContinuousStatus() {
  int16 status;
  uns32 bytecount;
  uns32 bufcount;
  if (!pl_exp_check_cont_status(camh, &status, &bytecount, &bufcount))
    throw pvpException("pvpCamera: check_status failed");
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
  int16 status;
  uns32 bytecount;
  if (!pl_exp_check_status(camh, &status, &bytecount))
    throw pvpException("pvpCamera: check_status failed");
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
    throw pvpException("pvpCamera: check_status failed");
  return bytecount/2;
}

size_t pvpCamera::nPixelsSoFarContinuous(size_t npix_in_buf) {
  int16 status;
  uns32 bytecount;
  uns32 bufcount;
  if (!pl_exp_check_cont_status(camh, &status, &bytecount, &bufcount))
    throw pvpException("pvpCamera: check_status failed");
  return bytecount/2 + npix_in_buf*bufcount;
}

int pvpCamera::haveNewFrame() {
  int16 status;
  uns32 bytecount;
  uns32 bufcount;
  if (!pl_exp_check_cont_status(camh, &status, &bytecount, &bufcount))
    throw pvpException("pvpCamera: check_status failed");
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

  
