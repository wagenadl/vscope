// cohdata.cpp

#include "cohdata.h"
#include <base/roidata3set.h>
#include <math/cohest.h>
#include <math/psdest.h>
#include <base/roiset.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <math/taperbank.h>
#include <base/types.h>
#include <base/analogdata.h>
#include <base/digitaldata.h>

//#define COH_STRIP_START 1
//#define COH_STRIP_END   2
#define COH_STRIP_START 2
#define COH_STRIP_END   0

QSet<QString> CohData::warned;

CohData::CohData() {
  roiset=0;
  rs3d=0;
  adata=0;
  ddata=0;
  data.valid=false;
  cohest = new CohEst;
  psdest = new PSDEst;
}

CohData::~CohData() {
  delete cohest;
  delete psdest;
}

void CohData::setROISet(ROISet const *r) {
  roiset = r;
  invalidate();
  emit newData();
}

void CohData::setCCDData(class ROIData3Set /*const*/ *dat) {
  /* Conceptually const, but we may be the one to trigger recalculation */
  if (rs3d && rs3d!=dat) {
    disconnect(rs3d, SIGNAL(newAll()), this, SLOT(updateROIs()));
    disconnect(rs3d, SIGNAL(newDatum(int)), this, SLOT(updateROIs()));
  }
  if (dat && rs3d!=dat) {
    connect(dat, SIGNAL(newAll()), this, SLOT(updateROIs()));
    connect(dat, SIGNAL(newDatum(int)), this, SLOT(updateROIs()));
  }
  rs3d = dat;
  invalidate();
  emit newData();
}

void CohData::updateROIs() {
  invalidate();
  emit newData();
}

void CohData::setEPhys(AnalogData const *ad, DigitalData const *dd) {
  dbg("cohdata::newephys");
  adata = ad;
  ddata = dd;
  invalidate();
  emit newData();
}

void CohData::invalidate() {
  data.valid=false;
}

QMap<int, double> const &CohData::magnitudes() const {
  if (!validate())
    dbg("CohData::magnitudes: Could not validate; returning null values");
  return data.coh_mag;
}

QMap<int, double> const &CohData::phases() const {
  if (!validate())
    dbg("CohData::phases: Could not validate; returning null values");
  return data.coh_pha;
}

double CohData::magnitude(int id) const {
  if (validate() && data.coh_mag.contains(id))
    return data.coh_mag[id];
  else
    return 0;
}

double CohData::phase(int id) const {
  if (validate() && data.coh_pha.contains(id))
    return data.coh_pha[id];
  else
    return 0;
}

bool CohData::validate() const {
  if (data.valid)
    return true;

  data.coh_mag.clear();
  data.coh_pha.clear();

  if (!roiset) {
    dbg("cohdata::validate: no roiset");
    data.valid = true; // so we won't repeat this warning inf'ly many times
    return false;
  }
  if (!rs3d) {
    dbg("cohdata::validate: no rs3d");
    data.valid = true; // so we won't repeat this warning inf'ly many times
    return false;
  }

  recalcTiming();
  recalcReference();

  QSet<int> const &ids = roiset->ids();

  rvec trc;
  foreach (int id, ids) {
    data.coh_mag[id] = 0;
    data.coh_pha[id] = 0;
    if (rs3d->haveData(id)) {
      CamPair const &cp = rs3d->getCam(id);
      CCDTiming const &t = data.timing[cp];
      double dt_s = t.dt_ms()/1000;
      double df_hz = 2./3;
      dbg("cohdata:recalc: nfr=%i dt=%g df=%g",t.nframes(),dt_s,df_hz);
      int N = t.nframes() - COH_STRIP_START - COH_STRIP_END;
      if (N>0) {
	TaperID tid(N, dt_s, df_hz);
	trc.resize(N);
	if (Taperbank::bank().canProvide(tid)) {
	  /* We should be using the prepare() interface to do this
	     in the background. Later.
	  */
	  Tapers const &tapers = Taperbank::bank().find(tid);
	  double const *src = rs3d->getData(id)->dataRatio();
	  for (int t=0; t<N; t++)
	    trc[t]=src[t+COH_STRIP_START];
	  cohest->compute(data.refs[cp], trc, dt_s, df_hz,
			  tapers, data.fstar_hz[cp]);
	  data.coh_mag[id] = cohest->magnitude[0];
	  data.coh_pha[id] = cohest->phase[0];
	  dbg("cohdata %i: %4.2f %3.0f",id,
	      data.coh_mag[id],data.coh_pha[id]*180/3.1415);
	} else if (Taperbank::bank().couldExist(tid) &&
		   !warned.contains(tid.name())) {
	  Warning()
	    << QString("Missing tapers '%1'.")
	       .arg(tid.name())
	       +QString("Please supply using vscope_preptaper('%1') in matlab.")
	       .arg(tid.name());
	  warned.insert(tid.name());
	}
      }
    }
  }
  data.valid = true;
  return true;
}

void CohData::recalcTiming() const {
  data.timing.clear();

  /* Our plan is to calculate the precise timing for each camera pair based
     on actual frame times.
     In the end, this should be made more sophisticated to deal with cameras
     with unequal timing, but for now, I am going to assume that the timing
     of the ratio trace is what we must use.
     Note that if the two cameras are different, then ROI3Data is actually
     the first class to notice the difference. But in any case, I am not
     worrying about that now.
   */

  if (!rs3d)
    return; // without that, we don't know about cameras
  if (!adata)
    return; // without that, we don't know sampling frequency

  double fs_hz = adata->getSamplingFrequency();

  // First, let's find our complement of camera pairs
  QHash<CamPair, int> campair_exemplars;
  foreach (int id, rs3d->allIDs())
    campair_exemplars[rs3d->getCam(id)] = id;

  // Now, for each camera pair, let's get the basic timing
  foreach (CamPair const &cp, campair_exemplars.keys()) {
    ROI3Data const *example = rs3d->getData(campair_exemplars[cp]);
    data.timing[cp].setRate(fs_hz);
    if (example->getDonorNFrames())
      data.timing[cp]
	.setFrames(example->getDonorNFrames())
	.setTiming(example->getDonorT0ms(), example->getDonorDTms());
    else
      data.timing[cp]
	.setFrames(example->getAcceptorNFrames())
	.setTiming(example->getAcceptorT0ms(), example->getAcceptorDTms());
    Dbg() << "timing: " << data.timing[cp].t0_ms() << "+" << data.timing[cp].dt_ms() << "x" << data.timing[cp].nframes();
  }

  // let's see if and what we can refine
  if (!ddata)
    return;

  QMap<CamPair, int> framelines;
  foreach (CamPair const &cp, campair_exemplars.keys()) {
    int donorLine = digilines.contains(cp.donor)
      ? digilines[cp.donor]
      : -1;
    int acceptorLine = digilines.contains(cp.acceptor)
      ? digilines[cp.acceptor]
      : -1;

    int frameline = (donorLine>=0 && ddata->hasLine(donorLine))
      ? donorLine
      : (acceptorLine>=0 && ddata->hasLine(acceptorLine))
      ? acceptorLine
      : -1;

    if (frameline<0)
      continue;

    // let's refine!
    DigitalData::DataType const *src = ddata->allData();
    int K = ddata->getNumScans();
    DigitalData::DataType mask=1; mask<<=frameline;
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
    if (istart>=0 && count==data.timing[cp].nframes()-1) {
      data.timing[cp].setTiming(istart*1000.0/data.timing[cp].fs_hz(),
			   (ilatest-istart)*1000.0/count
			   /data.timing[cp].fs_hz());
    } else {
      dbg("istart=%i ilatest=%i count=%i nfr=%i mask=0x%08x",istart,ilatest,
	  count,data.timing[cp].nframes(),mask);   
      dbg("CohData: WARNING: Could not count CCD frames");
      // I could give a GUI warning, but this probably only ever happens
      // when there are no cameras. Even otherwise, the problem will be minor.
    }
  }
}  

void CohData::recalcReference() const {
  /* We are going to reinterpolate the reference signal at the times of the
     ccd frames. We will also recalculate fstar_hz. */

  data.refs.clear();
  foreach (CamPair const &cp, data.timing.keys()) {
    CCDTiming const &t(data.timing[cp]);
    int N = t.nframes()-COH_STRIP_START-COH_STRIP_END;
    if (N<=0)
      continue;
    rvec &ref(data.refs[cp]);
    ref.resize(N);
    
    bool ok = false;
    switch (refType) {
    case RT_Analog: ok = adata!=0; break;
    case RT_Digital: ok = ddata!=0; break;
    case RT_Frequency: ok = ref_hz>0; break;
    }
    dbg("CohData::recalcReference: reftype=%i ok=%i",refType,ok);
    if (!ok) {
      for (int k=0; k<N; k++)
	ref[k]=0;
      data.fstar_hz[cp] = 1;
      continue;
    }
    
    DigitalData::DataType const *dsrc = refType==RT_Digital
      ? ddata->allData() : 0;
    DigitalData::DataType dmask = refType==RT_Digital
      ? ddata->maskForLine(ref_chn) : 0;
    double const *asrc = refType==RT_Analog
      ? adata->channelData(ref_chn) : 0;
    int astep = asrc
      ? adata->getNumChannels() : 0;
    int ephyslen =
      refType==RT_Digital ? ddata->getNumScans()
      : refType==RT_Analog ? adata->getNumScans()
      : refType==RT_Frequency ? 1000000000
      : 0;
    double fs_hz =
      refType==RT_Digital ? ddata->getSamplingFrequency()
      : refType==RT_Analog ? adata->getSamplingFrequency()
      : refType==RT_Frequency ? 1e6
      : 0;
    dbg("CohData: adata=%p adata->data=%p asrc=%p ref_chn=%s",
	adata,adata?adata->allData():0,asrc,qPrintable(ref_chn));
    
    if (refType==RT_Digital)
      ok = dsrc!=0;
    else if (refType==RT_Analog)
      ok = asrc!=0;
    
    if (!ok) {
      for (int k=0; k<N; k++)
	ref[k]=0;
      data.fstar_hz[cp] = 1;
      continue;
    }

    for (int k=0; k<N; k++) {
      double tstart = t.t0_ms()+(k+COH_STRIP_START)*t.dt_ms();
      double tend = tstart+t.dt_ms()*t.duty_percent()/100;
      int istart = int(tstart/1e3*fs_hz);
      int iend = int(tend/1e3*fs_hz);
      if (istart<0)
	istart=0;
      if (iend>ephyslen)
	iend=ephyslen;
      if (iend<istart)
	iend=istart;
      if (iend>istart) {
	double v=0;
	switch (refType) {
	case RT_Digital:
	  for (int t=istart; t<iend; t++) 
	    v += (dsrc[t]&dmask)>0;
	  ref[k] = v/(iend-istart);
	  break;
	case RT_Analog:
	  for (int t=istart; t<iend; t++)
	    v += asrc[t*astep];
	  ref[k] = v/(iend-istart);
	  break;
	case RT_Frequency:
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
    double b = sumx/t.nframes();
    double a = sumtx / sumtt;
    for (int k=0; k<N; k++) {
      double t = k-t0;
      ref[k] -= a*t+b;
    }
    
    // run a psdest to find spectral peak
    double dt_s = t.dt_ms()/1000;
    double df_hz = 1./3;
    //dbg("coherence:recalcref: nfr=%i dt=%g df=%g",data.timing.nframes,dt_s,df_hz);
    //dbg("coherence:recalcref: isdigi=%i chn=%i",ref_is_digital,ref_chn);
    TaperID tid(N, dt_s, df_hz);
    data.taperIDs[cp] = tid;
    if (Taperbank::bank().canProvide(tid)) {
      Tapers const &tapers = Taperbank::bank().find(tid);
      psdest->compute(ref, dt_s, df_hz, tapers);
      double max=0;
      double df2_best = numbers.inf;
      int N = psdest->psd.size();
      for (int n=0; n<N; n++) { // skip DC
	bool best;
	if (refType==RT_Frequency) {
	  double f = psdest->freqbase[n];
	  double df = f-ref_hz;
	  double df2 = df*df;
	  best = df2<df2_best;
	  if (best)
	    df2_best = df2;
	} else {
	  double y = psdest->psd[n];
	  best = n>0 && y>max;
	  if (best)
	    max = y;
	}
	if (best)
	  data.fstar_hz[cp] = psdest->freqbase[n];
      }
    } else {
      data.fstar_hz[cp] = 1;
      if (Taperbank::bank().couldExist(tid) &&
	  !warned.contains(tid.name())) {
	Warning()
	  << QString("Missing tapers '%1'.")
	  .arg(tid.name())
	  +QString("Please supply using vscope_preptaper('%1') in matlab.")
	  .arg(tid.name());
	warned.insert(tid.name());
      }
    }
  }
}

void CohData::setRefDigi(QString digiline) {
  refType = RT_Digital;
  ref_chn = digiline;
  invalidate();
}

void CohData::setRefTrace(QString ach) {
  refType = RT_Analog;
  ref_chn = ach;
  invalidate();
}

void CohData::setRefFreq(double fref_hz) {
  dbg("coherence setRefFreq: %g",fref_hz);
  refType = RT_Frequency;
  ref_hz = fref_hz;
  invalidate();
}

double CohData::getFStarHz(int id) const {
  validate();
  if (!rs3d || !rs3d->haveData(id))
    return 1;
  CamPair const &cp = rs3d->getCam(id);
  return data.fstar_hz[cp];
}

double CohData::getTypicalFStarHz() const {
  validate();
  int n=0;
  double fstar = 0;
  foreach (double d, data.fstar_hz) {
    n++;
    fstar += d;
  }
  if (n)
    return fstar/n;
  else
    return 1;
}

void CohData::setFrameLine(QString cam, int line) {
  digilines[cam] = line;
}
