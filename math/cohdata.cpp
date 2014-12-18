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
      double dt_s = t.dt_us()/1e6;
      double df_hz = 2./3;
      int N = t.nframes() - COH_STRIP_START - COH_STRIP_END;
      if (N>0) {
	TaperID tid(N, dt_s, df_hz);
	trc.resize(N);
	if (Taperbank::bank().canProvide(tid, true)) {
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
	//Dbg() << "CohData " << id << " / " << tid.name() << ": " << N 
	//	<< dt_s << " @ " <<  df_hz << " @ " << data.fstar_hz[cp];
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
     on actual frame times. (We can now get that straight from the ROIData.)
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
    data.timing[cp].reset(fs_hz);
    if (example->getDonorNFrames())
      data.timing[cp]
	.setFrames(example->getDonorNFrames())
	.setTimingI(1000*example->getDonorT0ms(),
		    1000*example->getDonorDTms(),
		    example->getDonorDurms()*100.0/example->getDonorDTms());
    else
      data.timing[cp]
	.setFrames(example->getAcceptorNFrames())
	.setTimingI(1000*example->getAcceptorT0ms(),
		    1000*example->getAcceptorDTms(),
		    example->getAcceptorDurms()
		    *100.0/example->getAcceptorDTms());
    Dbg() << "CohData: timing for " << cp << ": "
	  << data.timing[cp].t0_us() << " + "
	  << data.timing[cp].dt_us() << " / " 
	  << data.timing[cp].duty_percent();
  }
}

static void getTStartAndTEnd(CCDTiming const &t, int k,
                             double *tStart_out, double *tEnd_out) {
  double tstart = (t.t0_us()/1e3)+(k+COH_STRIP_START)*(t.dt_us()/1e3);
  double tend = tstart+t.dt_us()/1e3*t.duty_percent()/100;
  *tStart_out = tstart;
  *tEnd_out = tend;
}

static void getIStartAndIEnd(CCDTiming const &t, int k,
                             int ephyslen, double fs_hz,
                             int *iStart_out, int *iEnd_out) {
  double tstart, tend;
  getTStartAndTEnd(t, k, &tstart, &tend);
  int istart = int(tstart/1e3*fs_hz);
  int iend = int(tend/1e3*fs_hz);
  if (istart<0)
    istart=0;
  if (iend>ephyslen)
    iend=ephyslen;
  if (iend<istart)
    iend=istart;
  *iStart_out = istart;
  *iEnd_out = iend;
}

void CohData::recalcReference() const {
  /* We are going to reinterpolate the reference signal at the times of the
     ccd frames. We will also recalculate fstar_hz. */

  bool ok = false;
  switch (refType) {
  case RT_Analog: ok = adata!=0; break;
  case RT_Digital: ok = ddata!=0; break;
  case RT_Frequency: ok = refID.freq_Hz>0; break;
  case RT_Train: case RT_Pulses: ok = true; break;
  }
  
  data.refs.clear();
  foreach (CamPair const &cp, data.timing.keys()) {
    CCDTiming const &t(data.timing[cp]);
    int N = t.nframes()-COH_STRIP_START-COH_STRIP_END;
    if (N<=0)
      continue;
    rvec &ref(data.refs[cp]);
    ref.resize(N);
    memset((void*)ref.data(), 0, N*sizeof(double));
    data.fstar_hz[cp] = 1;
    if (!ok)
      continue;

    switch (refType) {
    case RT_Digital: {
      DigitalData::DataType const *dsrc = ddata->allData();
      if (!dsrc)
        continue;
      DigitalData::DataType dmask = ddata->maskForLine(refID.chn);
      int ephyslen = ddata->getNumScans();
      double fs_hz = ddata->getSamplingFrequency();
      for (int k=0; k<N; k++) {
        int istart, iend;
        getIStartAndIEnd(t, k, ephyslen, fs_hz, &istart, &iend);
        if (iend>istart) {
          double v = 0;
	  for (int t=istart; t<iend; t++) 
	    v += (dsrc[t]&dmask)>0;
	  ref[k] = v/(iend-istart);
        }
      }
      detrend(ref);
    } break;
    case RT_Analog: {
      double const *asrc = adata->channelData(refID.chn);
      if (!asrc)
        continue;
      int astep = adata->getNumChannels();
      int ephyslen = adata->getNumScans();
      double fs_hz = adata->getSamplingFrequency();
      for (int k=0; k<N; k++) {
        int istart, iend;
        getIStartAndIEnd(t, k, ephyslen, fs_hz, &istart, &iend);
        if (iend>istart) {
          double v = 0;
          for (int t=istart; t<iend; t++)
            v += asrc[t*astep];
          ref[k] = v/(iend-istart);
        }
      }
      detrend(ref);
    } break;
    case RT_Frequency: {
      for (int k=0; k<N; k++) {
        double tstart, tend;
        getTStartAndTEnd(t, k, &tstart, &tend);
        ref[k] = 2*(fmod((tstart+tend)/2/1000 * refID.freq_Hz, 1) < 0.5) - 1;
      }
    } break;
    case RT_Train: {
      refID.stim.instantiateTrainReference(ref.data(), ref.size(),
                             t.t0_us()/1e3+COH_STRIP_START*t.dt_us()/1e3,
                             t.dt_us()/1e3);
      detrend0(ref);
    } break;
    case RT_Pulses: {
      refID.stim.instantiatePulseReference(ref.data(), ref.size(),
                             t.t0_us()/1e3+COH_STRIP_START*t.dt_us()/1e3,
                             t.dt_us()/1e3);
      detrend0(ref);
    } break;
    }

    // run a psdest to find spectral peak
    double dt_s = t.dt_us()/1e6;
    double df_hz = 1./3;
    TaperID tid(N, dt_s, df_hz);
    data.taperIDs[cp] = tid;
    if (Taperbank::bank().canProvide(tid, true)) {
      Tapers const &tapers = Taperbank::bank().find(tid);
      psdest->compute(ref, dt_s, df_hz, tapers);
      if (refType==RT_Analog)
	crazyFilter(psdest->psd, psdest->freqbase[1]-psdest->freqbase[0]);
      int N = psdest->psd.size();
      int nbest = 0;
      if (refType==RT_Frequency || refType==RT_Train || refType==RT_Pulses) {
        double f0 = refType==RT_Frequency ? refID.freq_Hz
          : refType==RT_Train ? (1e3/refID.stim.trainPeriod_ms)
	  : (1e3/refID.stim.pulsePeriod_ms);
        double df2_best = numbers.inf;
        for (int n=1; n<N; n++) { // skip DC
	  double f = psdest->freqbase[n];
	  double df = f - f0;
	  double df2 = df*df;
	  if (df2<df2_best) {
            df2_best = df2;
	    nbest = n;
	  }
        }
      } else {
        double max = 0;
        for (int n=1; n<N; n++) { // skip DC
	  double y = psdest->psd[n];
	  if (y>max) {
	    max = y;
	    nbest = n;
	  }
	}
      }
      data.fstar_hz[cp] = psdest->freqbase[nbest];
    } else {
      data.fstar_hz[cp] = 1;
      if (Taperbank::bank().couldExist(tid) && !warned.contains(tid.name())) {
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
  refID.chn = digiline;
  invalidate();
}

void CohData::setRefTrace(QString ach) {
  refType = RT_Analog;
  refID.chn = ach;
  invalidate();
}

void CohData::setRefStim(StimulusDef const &s, bool pulse_not_train) {
  refType = pulse_not_train ? RT_Pulses: RT_Train;
  refID.stim = s;
  invalidate();
}

void CohData::setRefFreq(double fref_hz) {
  dbg("coherence setRefFreq: %g",fref_hz);
  refType = RT_Frequency;
  refID.freq_Hz = fref_hz;
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

void CohData::detrend0(rvec &ref) {
  /* Subtract mean from the reference channel.
   */
  int N = ref.size();
  double sumx=0;
  for (int k=0; k<N; k++) 
    sumx += ref[k];
  double a = sumx/N;
  for (int k=0; k<N; k++) 
    ref[k] -= a;
}

void CohData::detrend(rvec &ref) {
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
  int N = ref.size();
  double t0 = (N-1.)/2.;
  double sumx=0, sumtx=0, sumtt=0;
  for (int k=0; k<N; k++) {
    double t = k-t0;
    sumx += ref[k];
    sumtx += t*ref[k];
    sumtt += t*t;
  }
  double b = sumx/N;
  double a = sumtx / sumtt;
  for (int k=0; k<N; k++) {
    double t = k-t0;
    ref[k] -= a*t+b;
  }
}

void CohData::crazyFilter(rvec &psd, double df_hz) {
  // filter each point in the psd to resolution f.
  rvec psd0 = psd;
  int N = psd.size();
  for (int n=1; n<N; n++) {
    double f = n*df_hz;
    double df02 = .25*f + .5;
    double y = 0;
    double x = 0;
    for (int k=-N; k<2*N; k++) {
      double df = (k-n)*df_hz;
      double g = exp(-.5*df*df/df02);
      x += g;
      if (k>=0 && k<N)
	y += psd0[k]*g;
    }
    psd[n] = y/x;
  }
}
