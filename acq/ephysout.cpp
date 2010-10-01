// ephysout.cpp

#include "ephysout.h"

#include <xml/enumerator.h>
#include <xml/paramtree.h>
#include <xml/connections.h>
#include <base/dbg.h>
#include <base/minmax.h>
#include <base/exception.h>
#include <base/analogdata.h>
#include <daq/analogout.h>
#include <base/digitaldata.h>
#include <daq/digitalout.h>
#include <acq/ephysacq.h>
#include "dutycyclelimit.h"
#include <acq/trialsig.h>
#include <video/videoprog.h>
#include <QTimer>
#include <QSet>

#define PAR_OUTRATE "acqEphys/acqFreq"
// Alternative: "stimEphys/outrate"

EPhysOut::EPhysOut() {
  timer = 0;
  aout = 0;
  dout = 0;
  adata = 0;
  ddata = 0;
  active = false;
  prep = false;
  master = oldmaster = 0;
}

EPhysOut::~EPhysOut() {
  if (active) {
    fprintf(stderr,"EPhysOut destructed while active. Armageddon imminent.\n");
    try {
      abort();
    } catch(...) {
      fprintf(stderr,"Exception caught by EPhysOut destructor.\n");
    }
  }
  if (aout)
    delete aout;
  if (dout)
    delete dout;
}

void EPhysOut::setBuffer(AnalogData *ad, DigitalData *dd) {
  adata = ad;
  ddata = dd;
}

bool EPhysOut::prepare(ParamTree *ptree) {
  // This trial might involve ephys acquisition, ccd acquisition,
  // ephys stimulation, or all three.
  trialtime_ms = ptree->find("acqEphys/acqTime").toDouble();
  CCDAvoid ccdavoid = setupDData_CCD(ptree);

  bool enableStim = ptree->find("stimEphys/enable").toBool();
  if (enableStim) {
    setupAData_stim(ptree);
    setupDData_addStim(ptree);
  } else {
    setupAData_dummy();
  }

  if (ptree->find("stimVideo/enable").toBool())
    VideoProg::find().prepStim(ptree, ccdavoid, adata, ddata);
  
  prep = true;
  
  return createDAQ(ptree);
}

bool EPhysOut::prepareSnap(ParamTree *ptree) {
  // All we will do is trigger the camera and open the shutter for the
  // right amount of time. No other stimuli.
  trialtime_ms=0;
  setupDData_CCD(ptree);
  setupAData_dummy();
  prep=true;
  
  return createDAQ(ptree);
}

bool EPhysOut::createDAQ(ParamTree *ptree) {
  // perhaps we should use the ptree to figure out what device to use?
  QString devid = "";
  if (!DAQDevice::find(devid).ok())
    return false;
  
  if (!aout) {
    aout = new AnalogOut(0, devid);
    connect(aout,SIGNAL(productionEnded(AnalogOut *, bool)),
	    this,SLOT(aoutEnded()));
  }
  aout->setData(adata);
  aout->setFrequency(ptree->find(PAR_OUTRATE).toDouble());
  
  if (!dout)
    dout = new DigitalOut(aout, devid);
  dout->setData(ddata);
  dout->setChannelMask(ddata->getMask());

  return true;
}

CCDAvoid EPhysOut::setupDData_CCD(ParamTree *ptree) {
  /* This fills a DigitalData structure that triggers the cameras and
     opens the shutter if CCD acquisition is enabled in the ptree.
     If trialtime_ms=0, this is a "snapshot", and triggering and shuttering
     is enabled irrespective of the ptree.
  */
  /* NOTE dd 10/8/09: This, and other "setupDData" functions really
     ought to be farmed out to separate CCD, VStim, EPStim modules. */

  CCDAvoid ccdavoid;
  
  if (!ddata)
    throw Exception("EPhysOut","No digital buffer defined",
		    "setupDData_CCD");

  bool isSnap = trialtime_ms==0;

  double preshtr_ms = ptree->find("acqCCD/preIllum").toDouble();
  double postshtr_ms = ptree->find("acqCCD/postIllum").toDouble();

  if (isSnap) {
    postshtr_ms = 10; // HACK FOR POSTILLUMINATION TIME IN SNAPSHOTS
    dbg("Post-illum ignored for snapshot. Using 10 ms fixed value.");
  }
  
  double framerate_hz = ptree->find("acqCCD/rate").toDouble();
  double active_ms =  ptree->find("acqCCD/dur").toDouble();
  double delay_ms =  isSnap ? preshtr_ms 
    : ptree->find("acqCCD/delay").toDouble();
  if (delay_ms < preshtr_ms) {
    delay_ms = preshtr_ms;
    fprintf(stderr,"Warning: CCD delay increased to respect pre-illumination setting\n");
  }
  double dutyCycle_percent = ptree->find("acqCCD/dutyCycle").toDouble();
  bool trigEach = dutyCycle_TriggerEach(dutyCycle_percent);
  double frame_ms = 1000 / framerate_hz;
  bool enableCCD = isSnap ? true : ptree->find("acqCCD/enable").toBool();
  int nframes = isSnap ? 1 : int(active_ms/frame_ms+.49);
  //  double illumend_ms = delay_ms + nframes*frame_ms + postshtr_ms;
  double outrate_hz = ptree->find(PAR_OUTRATE).toDouble();
  int seqstartscan = int(delay_ms * outrate_hz/1000);
  int illumprescans = int(preshtr_ms*outrate_hz/1000);
  int illumpostscans = int(postshtr_ms*outrate_hz/1000);
  int nscans = int(trialtime_ms * outrate_hz/1000);
  int framescans = int(frame_ms * outrate_hz/1000);
  int actvscans = int(frame_ms*dutyCycle_percent/100 * outrate_hz/1000);
  if (nscans>0 && nscans<40)
    throw Exception("EPhysOut","Trial too short for CCD to acquire anything",
		    "setupDDataForCCD");

  if (nscans==0)
    nscans = seqstartscan + nframes*actvscans + illumpostscans + 40;

  if (seqstartscan > nscans - 40) {
    fprintf(stderr,
	    "Warning: Trimming CCD acq. start by %i to fit in trial dur.\n",
	    seqstartscan - (nscans-40));
    seqstartscan = nscans - 40; // ensure all our loops are clean
    // Probably, a warning should be issued.
  }
  if (trigEach && seqstartscan+nframes*framescans>nscans-40) {
    int nfr = (nscans-40-seqstartscan)/framescans;
    fprintf(stderr,
	    "Warning: Reducing trigger count by %i to fit in trial dur.\n",
	    nframes-nfr);
    // Probably, a warning should be issued.
  }

  ccdavoid.start_scans = seqstartscan;
  ccdavoid.ival_scans = framescans;
  ccdavoid.actv_scans = actvscans;
  ccdavoid.nframes = nframes;

  Enumerator *lines = Enumerator::find("DIGILINES");
  int shtrline = lines->lookup("ExcShtr");
  int trigccline = lines->lookup("TrigCc");
  int trigoxline = lines->lookup("TrigOx");
  ddata->addLine(shtrline);
  ddata->addLine(trigccline);
  ddata->addLine(trigoxline);
  uint32_t one = 1;
  uint32_t shtrval = one<<shtrline;
  uint32_t trigccval = one<<trigccline;
  uint32_t trigoxval = one<<trigoxline;
  
  /* Really, this ought to be done elsewhere, but we do need the correct
     value of nscans, which is set by CCD demands in case of snapshots: */
  ddata->reshape(nscans);
  uint32_t *data = ddata->allData();
  for (int i=0; i<nscans; i++)
    data[i]=0;
  /* End of unfortunately placed code. */

  if (enableCCD) {
    for (int n=0; n<nframes; n++) {
      int framestart = seqstartscan + n*framescans;
      int trigend = framestart + 10;
      int illumstart = framestart-illumprescans;
      int illumend = framestart+actvscans+illumpostscans;
      if (illumstart<0) {
	fprintf(stderr,
		"Warning: Illumination did not start as early as requested (by %i scans)\n",
		-illumstart);
	illumstart=0;
      }
      if (illumend>nscans-2) {
	fprintf(stderr,
		"Warning: Illumination did not end as late as requested (by %i scans)\n",
		illumend-(nscans-2));
	illumend=nscans-2;
      }
      if (trigend>nscans-2) {
	fprintf(stderr,
		"Warning: Trigger pulse trimmed to fit in trial (by %i scans)\n",
		trigend-(nscans-2));
	trigend=nscans-2;
      }
      for (int i=illumstart; i<illumend; i++)
	data[i] |= shtrval;
      if (trigEach || n==0)
	for (int i=framestart; i<trigend; i++)
	  data[i] |= trigccval|trigoxval;
    }
  }
  return ccdavoid;
}

void EPhysOut::setupDData_addStim(ParamTree *ptree) {
  // This must be called *after* setupDData_CCD.
  if (!ddata)
    throw Exception("EPhysOut","No digital buffer defined","setupDStim");

  Enumerator *lines = Enumerator::find("DIGILINES");
  int nmax0 = lines->getLargestValue();
  int nmax = -1;
  for (int n=0; n<=nmax0; n++) {
    if (lines->has(n)) {
      QString tag = lines->reverseLookup(n);
      if (tag.startsWith("DO")) {
	int k = tag.mid(2).toInt();
	if (k>nmax)
	  nmax=k;
      }
    }
  }
  QList<int> channels;
  Dbg() << "ephysout: nmax = " << nmax;
  for (int n=0; n<=nmax; n++) {
    Param &p = ptree->find(QString("stimEphys/channel:DO%1/enable").arg(n));
    uint32_t line = lines->lookup(QString("DO%1").arg(n));
    ddata->addLine(line); /* We're clamping all DOx lines, even if they
			     are not enabled. (We'll just write zeros.) */
    if (p.toBool())
      channels.push_back(n);
  }
  int nscans = ddata->getNumScans();
  double freqhz = ptree->find(PAR_OUTRATE).toDouble();
  uint32_t one = 1;
  uint32_t *dat = ddata->allData();
  for (QList<int>::iterator i=channels.begin(); i!=channels.end(); ++i) {
    int channel = *i;
    uint32_t line = lines->lookup(QString("DO%1").arg(channel));
    uint32_t cmask = one<<line;
    double delayms =
      ptree->find(QString("stimEphys/channel:DO%1/delay")
		  .arg(channel)).toDouble();
    int ntrains =
      ptree->find(QString("stimEphys/channel:DO%1/nTrains")
		  .arg(channel)).toInt();
    double trainperiodms =
      ptree->find(QString("stimEphys/channel:DO%1/trainPeriod")
		  .arg(channel)).toDouble();
    int npulses =
      ptree->find(QString("stimEphys/channel:DO%1/nPulses")
		  .arg(channel)).toInt();
    double pulseperiodms =
      ptree->find(QString("stimEphys/channel:DO%1/pulsePeriod")
		  .arg(channel)).toDouble();
    double pulsedurms =
      ptree->find(QString("stimEphys/channel:DO%1/pulseDur")
		  .arg(channel)).toDouble();

    int pulseperiodscans = roundi(pulseperiodms*freqhz/1000);
    int pulsedur1scans = roundi(pulsedurms*freqhz/1000);
    for (int itr=0; itr<ntrains; itr++) {
      int s00 = roundi((delayms+itr*trainperiodms)*freqhz/1000);
      for (int ipu=0; ipu<npulses; ipu++) {
	int s0 = s00 + ipu*pulseperiodscans;
	/* This way of rounding ensures that the pulseperiod is the same
	   for each pulse in the train, even if that makes the entire
	   train duration imperfectly rounded. */
	int s1=mini(s0+pulsedur1scans,nscans);
	for (int s=s0; s<s1; s++)
	  dat[s] |= cmask;
      }
    }
  }
  //  dbg("ephysout:setupdstim complete");
}


void EPhysOut::setupAData_stim(ParamTree *ptree) {
  // This must be called *after* setupDData_CCD.
  if (!adata)
    throw Exception("EPhysOut","No analog buffer defined","setupAData_stim");
  if (!ddata)
    throw Exception("EPhysOut","No digital buffer defined","setupAData_stim");
  
  QList<int> channels;
  QSet<int> chanIsStim;
  Enumerator *aochan = Enumerator::find("AOCHAN");
  int nmax = aochan->getLargestValue();
  int vchanx = aochan->has("VidX") ? aochan->lookup("VidX") : -1;
  int vchany = aochan->has("VidY") ? aochan->lookup("VidY") : -1;
  bool hasvid = ptree->find("stimVideo/enable").toBool();
  for (int n=0; n<=nmax; n++) {
    QString name = aochan->reverseLookup(n);
    bool isvid = n==vchanx || n==vchany;
    bool use;
    if (isvid) {
      use = hasvid;
    } else if (name=="SIGNATURE") {
      use = true;
    } else {
      Param *p = ptree->findp(QString("stimEphys/channel:%1/enable").arg(name));
      use = p && p->toBool();
      chanIsStim.insert(n);
    }
    if (use)
      channels.push_back(n);
  }
  if (channels.size()==0) {
    setupAData_dummy();
    return;
  }

  int nscans = ddata->getNumScans(); // we copy scan count from ddata
  int nchans = channels.size();

  adata->reshape(nscans,nchans);

  int idx=0;
  for (QList<int>::iterator i=channels.begin(); i!=channels.end(); ++i) {
    int channel = *i;
    QString name = aochan->reverseLookup(channel);
    dbg("ephysout: idx=%i channel=%i",idx,channel);
    adata->defineChannel(idx++,channel);

    if (chanIsStim.contains(channel))
      setupAData_mkStim(ptree, channel,
			QString("stimEphys/channel:%1/").arg(name));

   if (name=="SIGNATURE")
      TrialSig::sign(ptree->find("acquisition/_trialno").toInt(), adata);
  }
  dbg("ephysout:setupastim complete nch=%i nsc=%i",nchans,nscans);
}

void EPhysOut::setupAData_mkStim(ParamTree *ptree, int channel, QString path) {
  double *dat = adata->channelData(channel);
  double freqhz = ptree->find(PAR_OUTRATE).toDouble();
  int nscans = adata->getNumScans();
  int nchans = adata->getNumChannels();
  double delayms = ptree->find(path+"delay").toDouble();
  int ntrains = ptree->find(path+"nTrains").toInt();
  double trainperiodms = ptree->find(path+"trainPeriod").toDouble();
  int npulses = ptree->find(path+"nPulses").toInt();
  double pulseperiodms = ptree->find(path+"pulsePeriod").toDouble();
  double pulsedurms = ptree->find(path+"pulseDur").toDouble();
  double pulsedur2ms = ptree->find(path+"pulseDur2").toDouble();
  double pulseampmV = ptree->find(path+"pulseAmp").toDouble();
  double pulseamp2mV = ptree->find(path+"pulseAmp2").toDouble();
  QString pulseType = ptree->find(path+"pulseType").toString();
  
  for (int s=0; s<nscans; s++)
    dat[s*nchans] = 0;
  
  int pulseperiodscans = roundi(pulseperiodms*freqhz/1000);
  int pulsedur1scans = roundi(pulsedurms*freqhz/1000);
  int pulsedur2scans = roundi(pulsedur2ms*freqhz/1000);
  for (int itr=0; itr<ntrains; itr++) {
    int s00 = roundi((delayms+itr*trainperiodms)*freqhz/1000);
    for (int ipu=0; ipu<npulses; ipu++) {
      int s0 = s00 + ipu*pulseperiodscans;
      /* This way of rounding ensures that the pulseperiod is the same
	 for each pulse in the train, even if that makes the entire
	 train duration imperfectly rounded. */
      if (pulseType=="TTL") {
	pulseampmV=5000;
	pulseType="Square";
      }
      if (pulseType=="Square" || pulseType=="Biphasic") {
	int s1=mini(s0+pulsedur1scans,nscans);
	for (int s=s0; s<s1; s++)
	  dat[s*nchans] = pulseampmV/1000;
	if (pulseType=="Biphasic") {
	  int s2=mini(s1+pulsedur2scans,nscans);
	  for (int s=s1; s<s2; s++)
	    dat[s*nchans] = pulseamp2mV/1000;
	}
      } else if (pulseType=="Ramp") {	  
	int s1=mini(s0+pulsedur1scans,nscans);
	int ds = s1-s0-1;
	if (ds<1)
	  ds=1;
	double dV = (pulseamp2mV-pulseampmV)/1000/ds;
	for (int s=s0; s<s1; s++) 
	  dat[s*nchans] = pulseampmV/1000 + dV*(s-s0);
      } else
	throw Exception("EPhysOut","Unknown pulse type: '"+pulseType+"'",
			"setupAData_mkStim");
    }
  }
}

void EPhysOut::setupAData_dummy() {
  /* This requires that ddata has already been set up */
  /* Since USB6229 does not have a clock source for digital output, we
     must generate some analog data as well. We'll write some zeros to
     ao0. That should be harmless. */
  if (!adata)
    throw Exception("EPhysOut","No analog buffer defined","setupAData_dummy");
  if (!ddata)
    throw Exception("EPhysOut","No digital buffer defined","setupAData_dummy");
  
  adata->reshape(ddata->getNumScans(),1);
  adata->defineChannel(0,0);
  double *ad = adata->allData();
  int nscans = ddata->getNumScans();
  for (int i=0; i<nscans; i++)
    ad[i]=0;
}  

void EPhysOut::commit() {
  if (!prep)
    throw Exception("EPhysOut","Cannot commit: not prepared");
  if (active)
    throw Exception("EPhysOut","Cannot commit: already active");
  if (aout) {
    dbg("ephysout:commit");
    if (master!=oldmaster)
      aout->setMaster(master ? master->daq() : 0);
    oldmaster=master;
    //if (master) {
    //  dbg("ephysout: aout not committed, we'll let master do it");
    //} else {
    if (master)
      dbg("ephysout: not sure we should be committing aout, since we have a master");
    aout->commit();
    dbg("ephysout: aout committed");
    //}
  }
}

void EPhysOut::setMaster(class EPhysAcq *mstr) {
  master = mstr;
}

void EPhysOut::start() {
  dbg("ephysout::start");
  commit();
  active=true;
  if (aout) {
    if (master) {
      dbg("ephysout: aout not started, we'll let master do it");
    } else {
      aout->start();
      dbg("ephysout: aout started");
    }
  } else {
    dbg("ephysout:fakestart");
    if (!timer) {
      timer = new QTimer(this);
      connect(timer,SIGNAL(timeout()), this,SLOT(aoutEnded()));
    }
    timer->setSingleShot(true);
    timer->setInterval(int(trialtime_ms));
    try {
      QMap<int,double> sclmap = adata->writeInt16("/tmp/fakeout-analog.dat");
      ddata->writeUInt32("/tmp/fakeout-digital.dat");
      FILE *fakeout = fopen("/tmp/fakeout-info.txt","w");
      if (!fakeout)
	throw Exception("EPhysOut","Cannot write fakeout-info.txt");
         for (int i=0; i<adata->getNumChannels(); i++) {
	int c = adata->getChannelAtIndex(i);
	fprintf(fakeout,"idx %i: chn AO%i step=%g mV\n",
		i,c,
		sclmap[c]*1e3);
      }
      fclose(fakeout);
    } catch (Exception) {
      //dbg("failure to write fakeout data");
    }
    timer->start();
    //    dbg("ephysout:fakestarted ival=%g ms",trialtime_ms);
  }
  dbg("ephysout:started");
}

void EPhysOut::abort() {
  if (aout)
    aout->abort();
  active=false;
}

void EPhysOut::aoutEnded() {
  active=false;
  dbg("EPhysOut::aoutEnded");
  if (master) {
    dbg("ephysout::aoutended: not stopping aout; master will do it");
  } else {
    if (aout) {
      aout->stop(); // is this needed?
      dbg("ephysout::aoutended: aout stopped");
      aout->uncommit();
    } else {
      dbg("ephysout::aoutended: no aout to stop !!??");
    }
  }
  emit ended();
}
