// cohdata.cpp

#include "cohdata.h"
#include <base/roiset3data.h>
#include <math/cohest.h>
#include <math/psdest.h>
#include <base/roiset.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <math/taperbank.h>
#include <base/types.h>
#include <base/analogdata.h>
#include <base/digitaldata.h>

#define COH_STRIP_START 1
#define COH_STRIP_END   2

int CohData::frameline = 0;

void CohData::setFrameLine(int n) {
  frameline = n;
}


CohData::CohData() {
  cohest=0;
  psdest=0;
  roiset=0;
  rs3d=0;
  adata=0;
  ddata=0;
  valid=false;
}

CohData::~CohData() {
  if (cohest)
    delete cohest;
  if (psdest)
    delete psdest;
}

void CohData::newTiming(CCDTiming const &t) {
  dbg("cohdata::newtiming");
  timing = t;
  invalidate();
}

void CohData::newROISet(ROISet const *r) {
  roiset = r;
  invalidate();
}

void CohData::newCCDData(class ROISet3Data /*const*/ *dat) {
  /* Conceptually const, but we may be the one to trigger recalculation */
  rs3d = dat;
  invalidate();
}

void CohData::newEPhys(AnalogData const *ad, DigitalData const *dd) {
  dbg("cohdata::newephys");
  adata = ad;
  ddata = dd;
  invalidate();
}

void CohData::invalidate() {
  valid=false;
}

QMap<int, double> const &CohData::magnitudes() {
  if (!validate())
    dbg("CohData::magnitudes: Could not validate; returning null values");
  return coh_mag;
}

QMap<int, double> const &CohData::phases() {
  if (!validate())
    dbg("CohData::phases: Could not validate; returning null values");
  return coh_pha;
}

double CohData::magnitude(int id) {
  if (!valid)
    magnitudes();
  QMap<int, double>::const_iterator i = coh_mag.find(id);
  if (i!=coh_mag.end())
    return *i;
  return 0;
}

double CohData::phase(int id) {
  if (!valid)
    phases();
  QMap<int, double>::const_iterator i = coh_pha.find(id);
  if (i!=coh_pha.end())
    return *i;
  return 0;
}

bool CohData::validate() {
  if (valid)
    return true;

  coh_mag.clear();
  coh_pha.clear();

  if (timing.nframes<=1) {
    dbg("cohdata::validate: not enough frames: n=%i",timing.nframes);
    valid = true; // so we won't repeat this warning inf'ly many times
    return false;
  }
  if (!roiset) {
    dbg("cohdata::validate: no roiset");
    valid = true; // so we won't repeat this warning inf'ly many times
    return false;
  }
  if (!rs3d) {
    dbg("cohdata::validate: no rs3d");
    valid = true; // so we won't repeat this warning inf'ly many times
    return false;
  }

  refineTiming();
  recalcReference();

  QSet<int> const &ids = roiset->ids();

  if (!cohest)
    cohest = new CohEst;
  
  double dt_s = timing.dt_ms/1000;
  double df_hz = 2./3;
  dbg("cohdata:recalc: nfr=%i dt=%g df=%g",timing.nframes,dt_s,df_hz);
  int N = timing.nframes - COH_STRIP_START - COH_STRIP_END;
  TaperID tid(N, dt_s, df_hz);
  rvec trc; trc.resize(N);
  if (Taperbank::hasTaper(tid)) {
    Tapers const &tapers = Taperbank::tapers(tid);
    for (QSet<int>::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
      int id = *i;
      if (rs3d->haveData(id)) {
	double const *src = rs3d->getData(id)->dataRatio();
	for (int t=0; t<N; t++)
	  trc[t]=src[t+COH_STRIP_START];
	cohest->compute(ref, trc, dt_s, df_hz, tapers, fstar_hz);
	coh_mag[id] = cohest->magnitude[0];
	coh_pha[id] = cohest->phase[0];
	dbg("cohdata %i: %4.2f %3.0f",id,coh_mag[id],coh_pha[id]*180/3.14159265);
      } else {
	dbg("cohdata %i: found no data");
	coh_mag[id]=0;
	coh_pha[id]=0;
      }
    }
  } else {
    for (QSet<int>::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
      int id = *i;
      coh_mag[id] = 0;
      coh_pha[id] = 0;
    }
    if (!warned.contains(tid.name())) {
      warn(QString("Missing tapers '%1'. Please supply using preptaper('%2') in matlab.").arg(tid.name()).arg(tid.name()));
      warned.insert(tid.name());
    }
  }
  
  valid = true;
  return true;
}

void CohData::refineTiming() {
  if (!ddata)
    return;

  uint32_t const *src = ddata->allData();
  int K = ddata->getNumScans();
  uint32_t mask=1; mask<<=frameline;
  bool ready = true;
  int istart=-1;
  int count=0;
  int ilatest=-1;
  for (int k=0; k<K; k++) {
    if (ready) {
      if (src[k] & mask) {
	if (istart<0)
	  istart=k;
	else
	  count++;
	ilatest=k;
	ready=false;
      }
    } else {
      if (!(src[k] & mask)) {
	ready = true;
      }
    }
  }
  if (istart>=0 && count==timing.nframes-1) {
    timing.t0_ms = istart*1000.0/timing.fs_hz;
    timing.dt_ms = (ilatest-istart)*1000.0/count/timing.fs_hz;
    ///* Why is the following happening? I am not sure I buy its correctness. */
    //Param *p = Globals::ptree->findp("analysis/vsdDelay");
    //if (p) 
    //  timing.t0_ms -= p->toDouble();
  } else {
    dbg("istart=%i ilatest=%i count=%i nfr=%i mask=0x%08x",istart,ilatest,
	count,timing.nframes,mask);   
    dbg("CohData: WARNING: Could not count CCD frames");
    // I could give a GUI warning, but this probably only ever happens
    // when there are no cameras. Even otherwise, the problem will be minor.
  }
}  

void CohData::recalcReference() {
  /* We are going to reinterpolate the reference signal at the times of the
     ccd frames. We will also recalculate fstar_hz. */
  
  int N = timing.nframes-COH_STRIP_START-COH_STRIP_END;
  if (N<0)
    return;
  
  ref.resize(N);
  bool ok = false;
  switch (refType) {
  case refANALOG: ok = adata!=0; break;
  case refDIGITAL: ok = ddata!=0; break;
  case refFIXED: ok = ref_hz>0; break;
  }
  dbg("CohData::recalcReference: reftype=%i ok=%i",refType,ok);
  if (!ok) {
    for (int k=0; k<N; k++)
      ref[k]=0;
    fstar_hz = 1;
    return;
  }
  
  uint32_t *dsrc = refType==refDIGITAL ? ddata->allData() : 0;
  uint32_t dmask = 1<<ref_chn;
  double const *asrc = refType==refANALOG ? adata->channelData(ref_chn) : 0;
  int astep = asrc ? adata->getNumChannels() : 0;
  int ephyslen = refType==refDIGITAL ? ddata->getNumScans()
    : refType==refANALOG ? adata->getNumScans()
    : refType==refFIXED ? 1000000000
    : 0;
  dbg("CohData: adata=%p adata->data=%p asrc=%p ref_chn=%i",
      adata,adata?adata->allData():0,asrc,ref_chn);

  if (refType==refDIGITAL)
    ok = dsrc!=0;
  else if (refType==refANALOG)
    ok = asrc!=0;
    
  if (!ok) {
    for (int k=0; k<N; k++)
      ref[k]=0;
    fstar_hz = 1;
    return;
  }
  /* I am using the t0_ms and dt_ms from the protocol. It might be better
     to use the actual values from the FrameCc and FrameOx monitors. */

  dbg("CohData:recRef: N=%i dt_ms=%g fs_hz=%g ephl=%i",
      N,timing.dt_ms,timing.fs_hz,ephyslen);
  for (int k=0; k<N; k++) {
    double tstart = timing.t0_ms+(k+COH_STRIP_START)*timing.dt_ms;
    double tend = tstart+timing.dt_ms;
    int istart = int(tstart/1e3*timing.fs_hz);
    int iend = int(tend/1e3*timing.fs_hz);
    if (istart<0)
      istart=0;
    if (iend>ephyslen)
      iend=ephyslen;
    if (iend<istart)
      iend=istart;
    dbg("CohData:recRef: k=%i istart=%i iend=%i",k,istart,iend);
    if (iend>istart) {
      double v=0;
      switch (refType) {
      case refDIGITAL:
	for (int t=istart; t<iend; t++) 
	  v += (dsrc[t]&dmask)>0;
	ref[k] = v/(iend-istart);
	break;
      case refANALOG:
	for (int t=istart; t<iend; t++)
	  v += asrc[t*astep];
	ref[k] = v/(iend-istart);
	break;
      case refFIXED:
	ref[k] = -1. + 2.*(fmod((tstart+tend)/2/1000 * ref_hz, 1) < 0.5);
	dbg("coherence: ref[%03i]=%g",k,ref[k]);
	break;
      }
    } else {
      ref[k] = 0;
    }
  }

  /* Linear detrend of the reference channel.
     Fit y(t) = a t + b.
       dChi^2/da = 0 <=> sum(t*(at + b - x)) = 0.
       dChi^2/db = 0 <=> sum(at + b - x) = 0.
     i.e.
       a sum t^2 + b sum t - sum tx = 0
       a sum t   + b sum 1 - sum x = 0
     i.e.
       (a sum t^2 sum 1 - sum tx sum 1) - (a sum t sum t - sum x sum t) = 0
       (b sum t sum t - sum tx sum t) - (b sum 1 sum t^2 - sum x sum t^2) = 0
     i.e.
       b = (sum tx sum t - sum x sum t^2) / (sum t sum t - sum 1 sum t^2)
       a = (sum tx sum 1 - sum x sum t) / (sum t^2 sum 1 - sum t sum t) = 0
     If I shift s.t. sum t = 0, things are easier:
       b = sum x / sum 1
       a = sum tx / sum t^2
   */
  double t0 = (N-1.)/2.;
  double sumx=0, sumtx=0, sumtt=0;
  for (int k=0; k<N; k++) {
    double t = k-t0;
    sumx += ref[k];
    sumtx += t*ref[k];
    sumtt += t*t;
  }
  double b = sumx/timing.nframes;
  double a = sumtx / sumtt;
  for (int k=0; k<N; k++) {
    double t = k-t0;
    ref[k] -= a*t+b;
  }

  // run a psdest to find spectral peak
  double dt_s = timing.dt_ms/1000;
  double df_hz = 1./3;
  //dbg("coherence:recalcref: nfr=%i dt=%g df=%g",timing.nframes,dt_s,df_hz);
  //dbg("coherence:recalcref: isdigi=%i chn=%i",ref_is_digital,ref_chn);
  TaperID tid(N, dt_s, df_hz);
  if (Taperbank::hasTaper(tid)) {
    Tapers const &tapers = Taperbank::tapers(tid);
    if (!psdest)
      psdest = new PSDEst;
    psdest->compute(ref, dt_s, df_hz, tapers);
    double max=0;
    int N = psdest->psd.size();
    for (int n=0; n<N; n++) { // skip DC
      dbg("psd at %5.2f Hz: %5.2f",psdest->freqbase[n],psdest->psd[n]);
      if (n>0 && psdest->psd[n]>max) {
	max = psdest->psd[n];
	fstar_hz = psdest->freqbase[n];
      }
    }
  } else {
    fstar_hz = 1;
    if (!warned.contains(tid.name())) {
      warn(QString("Missing tapers '%1'. Please supply using preptaper('%2') in matlab.").arg(tid.name()).arg(tid.name()));
      warned.insert(tid.name());
    }
  }    
}

CohData::CohData(CohData const &other): CohData_(other) {
  copy(other);
}

void CohData::copy(CohData const &other) {
  if (cohest)
    cohest = new CohEst(*other.cohest);
  if (psdest)
    psdest = new PSDEst(*other.psdest);
}

CohData &CohData::operator=(CohData const &other) {
  if (cohest)
    delete cohest;
  if (psdest)
    delete psdest;
  *(CohData_*)this = other;
  copy(other);
  return *this;
}

void CohData::setRefDigi(int digiline) {
  refType = refDIGITAL;
  ref_chn = digiline;
  invalidate();
}

void CohData::setRefTrace(int ach) {
  refType = refANALOG;
  ref_chn = ach;
  invalidate();
}

void CohData::setRefFreq(double fref_hz) {
  dbg("coherence setRefFreq: %g",fref_hz);
  refType = refFIXED;
  ref_hz = fref_hz;
  invalidate();
}

