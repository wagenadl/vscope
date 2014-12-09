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
    case RT_Train: ok = adata!=0; break;
    }
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
    double const *asrc = (refType==RT_Analog || refType==RT_Train)
      ? adata->channelData(ref_chn) : 0;
    int astep = asrc
      ? adata->getNumChannels() : 0;
    int ephyslen =
      refType==RT_Digital ? ddata->getNumScans()
      : (refType==RT_Analog || refType==RT_Train) ? adata->getNumScans()
      : refType==RT_Frequency ? 1000000000
      : 0;
    double fs_hz =
      refType==RT_Digital ? ddata->getSamplingFrequency()
      : (refType==RT_Analog || refType==RT_Train)
      ? adata->getSamplingFrequency()
      : refType==RT_Frequency ? 1e6
      : 0;
    
    if (refType==RT_Digital)
      ok = dsrc!=0;
    else if (refType==RT_Analog || refType==RT_Train)
      ok = asrc!=0;
    
    if (!ok) {
      for (int k=0; k<N; k++)
	ref[k]=0;
      data.fstar_hz[cp] = 1;
      continue;
    }

    for (int k=0; k<N; k++) {
      double tstart = (t.t0_us()/1e3)+(k+COH_STRIP_START)*(t.dt_us()/1e3);
      double tend = tstart+t.dt_us()/1e3*t.duty_percent()/100;
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
	case RT_Analog: case RT_Train:
	  for (int t=istart; t<iend; t++)
	    v += asrc[t*astep];
	  ref[k] = v/(iend-istart);
	  break;
	case RT_Frequency:
	  ref[k] = -1. + 2.*(fmod((tstart+tend)/2/1000 * ref_hz, 1) < 0.5);
	  break;
	}
      } else {
	ref[k] = 0;
      }
    }

    detrend(ref);
    if (refType==RT_Train) {
      filterForTrain(ref, 1e6/t.dt_us());
      QFile f("/tmp/train.txt");
      f.open(QFile::WriteOnly);
      QTextStream ts(&f);
      foreach (double v, ref)
	ts << v << "\n";
    }
    
    // run a psdest to find spectral peak
    double dt_s = t.dt_us()/1e6;
    double df_hz = 1./3;
    TaperID tid(N, dt_s, df_hz);
    data.taperIDs[cp] = tid;
    if (Taperbank::bank().canProvide(tid, true)) {
      Tapers const &tapers = Taperbank::bank().find(tid);
      psdest->compute(ref, dt_s, df_hz, tapers);
      if (refType!=RT_Frequency)
	crazyFilter(psdest->psd, psdest->freqbase[1]-psdest->freqbase[0]);
      double max=0;
      double df2_best = numbers.inf;
      int N = psdest->psd.size();
      QFile f("/tmp/psd.txt");
      f.open(QFile::WriteOnly);
      QTextStream ts(&f);
      int nbest = 0;
      for (int n=1; n<N; n++) { // skip DC
	bool best;
	if (refType==RT_Frequency) {
	  double f = psdest->freqbase[n];
	  double df = f-ref_hz;
	  double df2 = df*df;
	  best = df2<df2_best;
	  if (best) {
	    df2_best = df2;
	    nbest = n;
	  }
	} else {
	  double y = psdest->psd[n];
	  ts << n << " " << psdest->freqbase[n] << " " << y << "\n";
	  best = n>0 && y>max;
	  if (best) {
	    max = y;
	    nbest = n;
	  }
	}
      }
      data.fstar_hz[cp] = psdest->freqbase[nbest];
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

void CohData::setRefTrace(QString ach, bool train) {
  refType = train ? RT_Train : RT_Analog;
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

void CohData::detrend(rvec &ref) {
  int N = ref.size();
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
  double b = sumx/N;
  double a = sumtx / sumtt;
  for (int k=0; k<N; k++) {
    double t = k-t0;
    ref[k] -= a*t+b;
  }
}

void CohData::filterForTrain(rvec &ref, double fs_hz) {
  //== First, we construct a Butterworth low pass filter at 4 Hz.
  double f0_hz = 4;

  // This comes straight from my BUTTERLOW1 Octave code:
  double c = 1/tan((f0_hz/fs_hz)*M_PI);
  double n0 = 1;
  double n1 = 1;
  double d0 = c + 1;
  double d1 = -c + 1;

  // double a1 = 1;
  double a2 = d1/d0;
  double b1 = n0/d0;
  double b2 = n1/d0;
  
  //== Then we filter once in forward direction
  /* Using Octave's filter eqn:
       y(n) = -sum_(k=1..N) c(k+1) y(n-k) + sum_(k=0..N) d(k+1) x(n-k)
     where
       c_k := a_k / a_1   and   d_k = b_k / a_1.
     For us that means:
        y(n) = -a2 y(n-1) + b1 x(n) + b2 x(n-1).
  */
  int N = ref.size();
  if (N<2)
    return;
  
  double xn1 = ref[0];
  double yn1 = ref[0];
  for (int n=1; n<N; n++) {
    double xn = ref[n];
    double yn = -a2*yn1 + b1*xn + b2*xn1;
    ref[n] = yn;
    xn1 = xn;
    yn1 = yn;
  }

  //== And again, backwards
  xn1 = ref[N-1];
  yn1 = ref[N-1];
  for (int n=N-2; n>=0; n--) {
    double xn = ref[n];
    double yn = -a2*yn1 + b1*xn + b2*xn1;
    ref[n] = yn;
    xn1 = xn;
    yn1 = yn;
  }

  //== Now, we calculate the average, min, max
  /* This method is numerically not very stable, but we don't have
     that many data points, so we'll be OK.
  */
  double avg = 0;
  double minr = 1e99;
  double maxr = -1e99;
  for (int n=0; n<N; n++) {
    double y = ref[n];
    avg += y;
    if (y>maxr)
      maxr = y;
    if (y<minr)
      minr = y;
  }      
  avg /= N;

  //== Then, schmitt trigger at the average with a little margin
  bool up=false;
  double marg = (maxr-minr)/100;
  for (int n=0; n<N; n++) {
    if (up) {
      if (ref[n] < avg - marg)
	up = false;
    } else {
      if (ref[n] > avg + marg)
	up = true;
    }
    ref[n] = up ? 1 : -1;
  }

  //== Lastly, subtract average
  avg = 0;
  for (int n=0; n<N; n++) {
    double y = ref[n];
    avg += y;
  }
  avg /= N;
  for (int n=0; n<N; n++)
    ref[n] -= avg;  

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
