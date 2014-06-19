// ephysout.cpp

#include "ephysout.h"

#include <xml/enumerator.h>
#include <base/enums.h>
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
#include <video/videoprog.h>
#include <QTimer>
#include <QSet>
#include <acq/epho_ccd.h>
#include <xml/outrate.h>

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

bool EPhysOut::prepare(ParamTree const *ptree,
		       AllCCDTimingDetail const &timing) {
  // This trial might involve ephys acquisition, ccd acquisition,
  // ephys stimulation, or all three.
  if (!ddata)
    throw Exception("EPhysOut","No digital buffer defined",
		    "prepare");
  trialtime_ms = ptree->find("acqEphys/acqTime").toDouble();
  EPhO_CCD epho_ccd(timing);
  ddata->reshape(timing.neededScans());
  ddata->zero();
  epho_ccd.prepare(ddata);
  bool enableStim = ptree->find("stimEphys/enable").toBool();
  if (enableStim) {
    setupAData_stim(ptree);
    setupDData_addStim(ptree);
  } else {
    setupAData_dummy();
  }
  adata->setSamplingFrequency(ptree->find(PAR_OUTRATE).toDouble());
  ddata->setSamplingFrequency(ptree->find(PAR_OUTRATE).toDouble());
  
  if (ptree->find("stimVideo/enable").toBool())
    VideoProg::find().prepStim(ptree, timing.first(), adata, ddata);
  
  prep = true;
  
  return createDAQ(ptree);
}

bool EPhysOut::prepareSnap(ParamTree const *ptree,
			   AllCCDTimingDetail const &timing) {
  // All we will do is trigger the camera and open the shutter for the
  // right amount of time. No other stimuli.
  if (!ddata)
    throw Exception("EPhysOut","No digital buffer defined",
		    "prepare");
  trialtime_ms=0;
  EPhO_CCD epho_ccd(timing);
  ddata->reshape(timing.neededScans());
  ddata->zero();
  epho_ccd.prepare(ddata);
  setupAData_dummy();
  adata->setSamplingFrequency(ptree->find(PAR_OUTRATE).toDouble());
  prep=true;
  
  return createDAQ(ptree);
}

bool EPhysOut::createDAQ(ParamTree const *ptree) {
  QString devid = "";
  if (!DAQDevice::find(devid).ok())
    return false;
  
  if (!aout) {
    aout = new AnalogOut(0, devid);
    connect(aout, SIGNAL(productionEnded(AnalogOut *, bool)),
	    this, SLOT(aoutEnded()));
  }
  QList<AnalogOut::Channel> chlist;
  foreach (QString id, channelList) 
    chlist.append(AnalogOut::Channel(Connections::findAO(id).line));
  aout->setData(adata, chlist);
  aout->setFrequency(ptree->find(PAR_OUTRATE).toDouble());
  
  if (!dout)
    dout = new DigitalOut(aout, devid);
  dout->setData(ddata);
  dout->setChannelMask(ddata->getMask());

  return true;
}

void EPhysOut::setupDData_addStim(ParamTree const *ptree) {
  if (!ddata)
    throw Exception("EPhysOut","No digital buffer defined","setupDStim");

  KeyGuard guard(*ddata);

  QStringList allStimLines = Connections::digiStimLines();

  QStringList useStimLines;
  foreach (QString id, allStimLines) {
    Param const &p =
      ptree->find(QString("stimEphys/channel:%1/enable").arg(id));
    ddata->addLine(Connections::findDig(id).line);
    /* We're clamping all DOx lines, even if they
       are not enabled. (We'll just write zeros.) */
    if (p.toBool())
      useStimLines.append(id);
  }
  int nscans = ddata->getNumScans();
  double freqhz = ptree->find(PAR_OUTRATE).toDouble();
  uint32_t one = 1;
  uint32_t *dat = ddata->allData(guard.key());

  foreach (QString id, useStimLines) {
    uint32_t line = Connections::findDig(id).line;
    uint32_t cmask = one<<line;
    double delayms =
      ptree->find(QString("stimEphys/channel:%1/delay")
		  .arg(id)).toDouble();
    int ntrains =
      ptree->find(QString("stimEphys/channel:%1/nTrains")
		  .arg(id)).toInt();
    double trainperiodms =
      ptree->find(QString("stimEphys/channel:%1/trainPeriod")
		  .arg(id)).toDouble();
    int npulses =
      ptree->find(QString("stimEphys/channel:%1/nPulses")
		  .arg(id)).toInt();
    double pulseperiodms =
      ptree->find(QString("stimEphys/channel:%1/pulsePeriod")
		  .arg(id)).toDouble();
    double pulsedurms =
      ptree->find(QString("stimEphys/channel:%1/pulseDur")
		  .arg(id)).toDouble();

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
}


void EPhysOut::setupAData_stim(ParamTree const *ptree) {
  if (!adata)
    throw Exception("EPhysOut","No analog buffer defined","setupAData_stim");
  if (!ddata)
    throw Exception("EPhysOut","No digital buffer defined","setupAData_stim");

  QStringList allStimChs = Connections::analogStims();
  QMap<int, QString> useChannel;
  QSet<QString> stimChannels;
  
  foreach (QString id, allStimChs) {
    Param const *p =
      ptree->findp(QString("stimEphys/channel:%1/enable").arg(id));
    if (p && p->toBool()) {
      useChannel[Connections::findAO(id).line] = id;
      stimChannels.insert(id);
    }
  }

  if (ptree->find("stimVideo/enable").toBool()) {
    QStringList vidChs = QString("VidX VidY").split(" ");
    foreach (QString id, vidChs) {
      Connections::AOChannel const *aoc = Connections::findpAO(id);
      if (aoc)
	useChannel[aoc->line] = id;
      else
	Dbg() << "ephysout surprise: stimvideo enabled but did not find line "
	      << id;
    }
  }

  channelList = useChannel.values();
  
  if (channelList.isEmpty()) {
    setupAData_dummy();
    return;
  }

  int nscans = ddata->getNumScans(); // we copy scan count from ddata
  int nchans = channelList.size();

  adata->reshape(nscans, nchans);
  int idx=0;
  foreach (QString name, channelList) {
    Connections::AOChannel const &aoc = Connections::findAO(name);
    adata->defineChannel(idx++, name, aoc.scale, aoc.unit);

    if (stimChannels.contains(name))
      setupAData_mkStim(ptree, name,
			QString("stimEphys/channel:%1/").arg(name));
  }
}

void EPhysOut::setupAData_mkStim(ParamTree const *ptree,
				 QString channel, QString path) {
  KeyGuard guard(*adata);
  double *dat = adata->channelData(guard.key(), channel);
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
  PULSETYPE pulseType = (PULSETYPE)ptree->find(path+"pulseType").toInt();
  
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
      if (pulseType==PT_TTL) {
	pulseampmV=5000;
	pulseType=PT_Square;
      }
      if (pulseType==PT_Square || pulseType==PT_Biphasic) {
	int s1=mini(s0+pulsedur1scans,nscans);
	for (int s=s0; s<s1; s++)
	  dat[s*nchans] = pulseampmV/1000;
	if (pulseType==PT_Biphasic) {
	  int s2=mini(s1+pulsedur2scans,nscans);
	  for (int s=s1; s<s2; s++)
	    dat[s*nchans] = pulseamp2mV/1000;
	}
      } else if (pulseType==PT_Ramp) {	  
	int s1=mini(s0+pulsedur1scans,nscans);
	int ds = s1-s0-1;
	if (ds<1)
	  ds=1;
	double dV = (pulseamp2mV-pulseampmV)/1000/ds;
	for (int s=s0; s<s1; s++) 
	  dat[s*nchans] = pulseampmV/1000 + dV*(s-s0);
      } else
	throw Exception("EPhysOut","Unknown pulse type: '"
			+ ptree->find(path+"pulseType").toString() + "'",
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

  KeyGuard guard(*adata);
  QString cid = Connections::analogOutputs().first();
  channelList.clear();
  channelList.append(cid);
  adata->reshape(ddata->getNumScans(), 1);
  adata->defineChannel(0, cid);
  double *ad = adata->allData(guard.key());
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
    if (master!=oldmaster)
      aout->setMaster(master ? master->daq() : 0);
    oldmaster=master;
    aout->commit();
  }
}

void EPhysOut::setMaster(class EPhysAcq *mstr) {
  master = mstr;
}

void EPhysOut::start() {
  commit();
  active=true;
  if (aout) {
    if (master) {
    } else {
      aout->start();
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
      AnalogData::ScaleMap sclmap = adata->writeInt16("/tmp/fakeout-analog.dat");
      ddata->writeUInt32("/tmp/fakeout-digital.dat");
      FILE *fakeout = fopen("/tmp/fakeout-info.txt","w");
      if (!fakeout)
	throw Exception("EPhysOut","Cannot write fakeout-info.txt");
      for (int i=0; i<adata->getNumChannels(); i++) {
	QString ch = adata->getChannelAtIndex(i);
	fprintf(fakeout,"idx %i: chn %s step=%g mV\n",
		i, qPrintable(ch),
		sclmap[ch]*1e3);
      }
      fclose(fakeout);
    } catch (Exception) {
    }
    timer->start();
  }
}

void EPhysOut::abort() {
  if (aout)
    aout->abort();
  active=false;
}

void EPhysOut::aoutEnded() {
  active=false;
  if (master) {
  } else {
    if (aout) {
      aout->stop(); // is this needed?
      aout->uncommit();
    } else {
      dbg("ephysout::aoutended: no aout to stop !!??");
    }
  }
  emit ended();
}
