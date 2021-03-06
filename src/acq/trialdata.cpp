// acq/trialdata.cpp - This file is part of VScope.
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

// trialdata.cpp


#include <base/analogdata.h>
#include <base/digitaldata.h>
#include <base/ccddata.h>
#include <base/unitqty.h>

#include <xml/enumerator.h>
#include <xml/connections.h>
#include <xml/paramtree.h>
#include <base/xml.h>
#include <base/base26.h>
#include <acq/ccdmaster.h>

#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QThread>
#include <QSet>
#include <xml/enumerator.h>
#include <base/dbg.h>
#include <base/progressdialog.h>
#include "trialdata.h"
#include <base/keyagg.h>

TrialData::TrialData(): KeyAgg(0) {
  xml = 0;

  adataIn = new AnalogData(1024, 1, 1e4);
  adataOut = new AnalogData(1024, 1, 1e4);
  ddataIn = new DigitalData(1024, 1e4);
  ddataOut = new DigitalData(1024, 1e4);
  partree = 0;
  mypartree = 0;

  add(adataIn);
  add(ddataIn);
  add(adataOut);
  add(ddataOut);

  useConnectedCameras();

  prep=false;
  snap=false;
}

TrialData::~TrialData() {
  foreach (CCDData *d, ccddata)
    delete d;
  delete ddataOut;
  delete adataOut;
  delete ddataIn;
  delete adataIn;

  if (mypartree)
    delete mypartree;
}

QStringList const &TrialData::cameras() const {
  return camids;
}

void TrialData::useConnectedCameras() {
  QStringList newcams = Connections::allCams();
  
  QSet<QString> newset;
  foreach (QString id, newcams)
    newset.insert(id);

  QSet<QString> oldset;
  foreach (QString id, camids)
    oldset.insert(id);
  
  // remove old cameras
  foreach (QString id, camids)
    if (!newset.contains(id)) {
      remove(ccddata[id]);
      delete ccddata[id]; // automatically removes from signal list
      ccddata.remove(id);
      ccdplace.remove(id);
      campairs.remove(id);
    }

  // add new cameras
  foreach (QString id, newcams) 
    if (!oldset.contains(id))
      add(ccddata[id] = new CCDData());

  camids = newcams;

  // place all cameras
  foreach (QString id, camids) {
    ccdplace[id] = camPlace(id);
    ccddata[id]->setDataToCanvas(ccdplace[id]);
  }

  // create pairs
  foreach (QString id, camids) 
    campairs[id] = Connections::camPair(id);
  
  if (newset!=oldset)
    emit newCameras();
}

CCDTimingDetail const &TrialData::timing(QString camid) const {
  return timing_[camid];
}

bool TrialData::haveCCDData(QString camid) const {
  return ccddata.contains(camid);
}

CCDData const *TrialData::ccdData(QString camid) const {
  if (haveCCDData(camid))
    return ccddata[camid];
  else
    throw Exception("TrialData", "No CCD Data for camera " + camid);
}

CCDData *TrialData::ccdData(QString camid) {
  if (haveCCDData(camid))
    return ccddata[camid];
  else
    throw Exception("TrialData", "No CCD Data for camera " + camid);
}

Transform const &TrialData::ccdPlacement(QString camid) const {
  if (haveCCDData(camid))
    return ccdplace.find(camid).value();
  else
    throw Exception("TrialData", "No CCD Data for camera " + camid);
}

CamPair const &TrialData::camPair(QString camid) const {
  if (haveCCDData(camid))
    return campairs.find(camid).value();
  else
    throw Exception("TrialData", "No CCD Data for camera " + camid);
}

void TrialData::generalPrep(bool concams) {
  clearCCD();
  clearAnalog();
  clearDigital();

  if (xml)
    delete xml;

  fpath = partree->find("filePath").toString();
  exptname = partree->find("acquisition/exptname").toString();
  trialid = trialname();

  xml = new XML(0, "vsdscopeTrial");
  QDomElement settings = xml->append("settings");
  partree->write(settings);

  QDomElement info = xml->append("info");
  info.setAttribute("expt",exptname);
  info.setAttribute("trial",trialid);
  if (snap)
    info.setAttribute("type","snapshot");
  else if (partree->find("acqCCD/enable").toBool())
    info.setAttribute("type","ephys+vsd");
  else
    info.setAttribute("type","ephys");
  if (snap) {
    info.setAttribute("stim","0");
    info.setAttribute("duration","0 s");
  } else {
    info.setAttribute("stim",partree->find("stimEphys/enable").toString());
    info.setAttribute("duration",partree->find("acqEphys/acqTime").toString());
  }
  if (contEphys) {
    QString conttri = QString("%1")
      .arg(partree->find("acquisition/contephys_trialno").toInt(),
	   int(3),int(10),QChar('0'));
    info.setAttribute("contephys", conttri);
  } else {
    info.setAttribute("contephys", "0");
    if (!snap) {
      QString acqfreq = partree->find("acqEphys/acqFreq").toString();
      QDomElement analog = xml->append("analog");
      analog.setAttribute("rate",acqfreq);
      QDomElement digital = xml->append("digital");
      digital.setAttribute("rate",acqfreq);
    }
  }
  if (do_ccd) {
    QDomElement ccd = xml->append("ccd");
    if (concams)
      useConnectedCameras();
  }
  prep = true;
}

Transform TrialData::camPlace(QString camid) const {
  Connections::CamCon const *cam = Connections::findpCam(camid);
  if (!cam) {
    Dbg() << "TrialData::CamPlace: no data for camera " << camid;
    return Transform();
  }
  Transform t0 = cam->placement;
  if (partree) {
    ParamTree const &ct = ::camTree(partree, camid);
    QRect reg(ct.find("region").toRect());
    QRect bin(ct.find("binning").toRect());
    /* This is actually hard to figure out for flipped cameras.
       Let's try some examples.
       A camera that y-flips, so that t0=1x-1+0+512.
       Assume that I am binning 1x2. I want to return t=1x-2+0+512.
       So, if I set t1=1x2+0+0, t0(t1) = 1x-2+0+512; all good.
       Second example. Same camera.
       Assume that the region is 256x256+256+256. That's in global coords,
       for now. I want to return t=1x-1+256+512.
       Now, if I set t1=1x1+256+256, I get t0(t1) = 1x-1+256+256. That's not
       what I want. I would get the right thing from t1=1x1+256+0, but how
       does that make sense? Answer: by reverse applying t0 to the region!
       Just like in ccdacq!
     */
    reg = t0.inverse()(reg);
    Transform t1;
    t1.scale(bin.width(),bin.height());
    t1.translate(reg.left(),reg.top());
    return t0(t1);
  } else {
    return t0;
  }
}

void TrialData::useThisPTree(ParamTree const *ptree) {
  Dbg() << "useThisPTree" << ptree;
  if (mypartree)
    delete mypartree;
  partree = mypartree = 0;
  partree = ptree;
  updateEnums();
  prepare();
}

void TrialData::cloneThisPTree(ParamTree const *ptree) {
  Dbg() << "cloneThisPTree" << ptree;
  if (mypartree)
    delete mypartree;
  mypartree = 0;
  mypartree = new ParamTree(*ptree);
  partree = mypartree;
  updateEnums();
  prepare();
}

void TrialData::prepare() {
  KeyGuard guard(*this);
  prepare(true);
}

void TrialData::prepare(bool concams) {
  snap=false;
  timing_.prepTrial(partree);
  contEphys = partree->find("acquisition/contEphys").toBool();
  /* contEphys tracks whether continuous e'phys acq is simultaneously
     happening (in "ContAcq"). We are *not* doing that ourselves here. */
  do_ccd = partree->find("acqCCD/enable").toBool();
  generalPrep(concams);
}

void TrialData::prepareSnapshot() {
  KeyGuard guard(*this);
  prepareSnapshot(true);
}

void TrialData::prepareSnapshot(bool concams) {
  snap=true;
  timing_.prepSnap(partree);
  do_ccd = true;
  generalPrep(concams);
}

QString TrialData::trialname() const {
  int t = partree->find("acquisition/trialno").toInt();
  return QString("%1").arg(t,int(3),int(10),QChar('0'));
}

void TrialData::write() const {
  if (!xml)
    throw Exception("Trial","Cannot write - not prepared");

  QDir d; d.mkpath(QString("%1/%2").arg(fpath).arg(exptname));
  QString base = QString("%1/%2/%3").arg(fpath).arg(exptname).arg(trialid);
  if (!snap && !contEphys) {
    writeAnalog(base);
    writeDigital(base);
  }
  
  if (do_ccd) 
    writeCCD(base);

  // xml
  xml->write(base + ".xml");
  Dbg() << "TrialData::write" << d.absolutePath() << d.exists() << snap << contEphys << do_ccd << base;
}

void TrialData::read(QString dir, QString exptname0, QString trialid0,
                     ProgressDialog *pd) {
  if (pd)
    pd->push(5, "Loading trial metadata");
  KeyGuard guard(*this);
  prep=false;

  fpath = dir;
  exptname = exptname0;
  trialid = trialid0;
  QString base = QString("%1/%2/%3").arg(dir).arg(exptname0).arg(trialid0);
  XML myxml(base + ".xml");

  QDomElement info = myxml.find("info");
  QDomElement settings = myxml.find("settings");

  Dbg() << "TrialData::read" << mypartree;
  if (!mypartree)
    mypartree = new ParamTree(*partree);
  
  mypartree->read(settings);

  mypartree->find("filePath").set(fpath);
  mypartree->find("acquisition/exptname").set(exptname);
  mypartree->find("acquisition/trialno").set(trialid);

  contEphys = info.attribute("contephys").toInt() > 0;
  
  // the following ensures that we have stimulus data prepared
  // and that the xdataIn have the right sizes
  if (info.attribute("type")=="snapshot")
    prepareSnapshot(false);
  else
    prepare(false);
  if (pd)
    pd->pop();

  if (xml)
    delete xml;
  xml = new XML(myxml);

  if (pd)
    pd->push(95, "Loading trial data");
  if (snap) {
    clearAnalog();
    clearDigital();
  } else if (contEphys) {
    if (pd)
      pd->push(do_ccd ? 25 : 100);
    readContEphys(myxml, base, pd);
    pd->pop();
  } else {
    if (pd) {
      pd->push(do_ccd ? 25 : 100);
      pd->push(75, "Loading analog data");
    }
    readAnalog(myxml, base, pd);
    if (pd) {
      pd->pop();
      pd->push(25, "Loading digital data");
    }
    readDigital(myxml, base, pd);
    if (pd) {
      pd->pop();
      pd->pop();
    }
  }

  if (do_ccd) {
    if (pd)
      pd->push((snap||contEphys) ? 100 : 75, "Loading CCD data");
    readCCD(myxml, base, pd);
    if (pd)
      pd->pop();
  } else {
    clearCCD();
  }
}

void TrialData::writeAnalog(QString base) const {
  QDomElement analog = xml->find("analog");
  adataIn->write(base+"-analog.dat", analog);
}

void TrialData::writeDigital(QString base) const {
  QDomElement digital = xml->find("digital");
  ddataIn->write(base + "-digital.dat", digital);
}

void TrialData::writeCCD(QString base) const {
  QDomElement ccd = xml->find("ccd");
  ccd.setAttribute("cameras", QString::number(camids.size()));
  QString ccdfn = base + "-ccd.dat";
  QFile ccdf(ccdfn);
  if (!ccdf.open(QIODevice::WriteOnly))
    throw Exception("Trial",
		    QString("Cannot open '%1' for writing").arg(ccdfn),
		    "write");

  foreach (QString id, camids) {
    if (ccddata[id]->getNFrames()>0) {
      QDomElement cam = ccddata[id]->write(ccdf, ccd);
      cam.setAttribute("name", id);
      try {
	CamPair pair(Connections::camPair(id));
	if (pair.acceptor==id)
	  cam.setAttribute("donor", pair.donor);
      } catch (...) {
	// We _try_ to add donor information, but not aggressively
	// Eventually, we should do that in a better way, but for now this
	// is enough to future-proof our files. This information is needed
	// by camimages and cohimages. (Which are way too similar; there
	// should be some subclassing there.)
      }
    }
  }
  ccdf.close();
}

void TrialData::clearAnalog() {
  adataIn->zero();
}

void TrialData::clearDigital() {
  ddataIn->zero();
}

void TrialData::readContEphys(XML &vsdxml, QString vsdbase, ProgressDialog *pd) {
  // vsdxml and vsdbase refer to the vsd trial, not the cont trial
  clearAnalog();
  clearDigital();
  QDir exptdir(QFileInfo(vsdbase).dir());
  int vsdtrialno = mypartree->find("acquisition/trialno").toInt();
  QDomElement vsdinfo = vsdxml.find("info");
  QString conttrialid = vsdinfo.attribute("contephys");

  QString contbase = QString("%1/%3")
    .arg(exptdir.absolutePath())
    .arg(conttrialid);
  // Get info from cont trial xml
  XML contxml(contbase + ".xml");
  QDomElement continfo = contxml.find("info");
  // Where should we load from?
  quint64 ascan = 0;
  quint64 dscan = 0;
  for (QDomElement trialxml = continfo.firstChildElement("trial");
       !trialxml.isNull(); trialxml = trialxml.nextSiblingElement("trial")) {
    if (trialxml.attribute("id").toInt()==vsdtrialno) {
      ascan = trialxml.attribute("ascan").toULongLong();
      dscan = trialxml.attribute("dscan").toULongLong();
      break;
    }
  }
  if (ascan<=0 || dscan!=ascan) {
    Dbg() << "Failed to get meaningful ascan or dscan - cannot load contephys";
    return; // should show message box!
  }
  Dbg() << "Got ascan" << ascan << "dscan" << dscan;
  // how much should we read?
  UnitQty dur(vsdinfo.attribute("duration"));
  double dur_ms = dur.toDouble("ms");
  dur_ms += 2000; // add some margin for trial start delays

  // load partial data
  if (pd)
    pd->push(75, "Loading analog cont. data");
  ddataIn->readPartial(contbase + "-digital.dat", contxml.find("digital"),
                       dscan, dur_ms, pd);
  if (pd)
    pd->pop();
  if (pd)
    pd->push(25, "Loading digital cont. data");
  adataIn->readPartial(contbase + "-analog.dat", contxml.find("analog"),
                       ascan, dur_ms, pd);
  if (pd)
    pd->pop();
}  

void TrialData::readAnalog(XML &myxml, QString base, ProgressDialog *pd) {
  QDomElement analog = myxml.find("analog");
  adataIn->read(base+"-analog.dat", analog, pd);
}

void TrialData::readDigital(XML &myxml, QString base, ProgressDialog *pd) {
  QDomElement digital = myxml.find("digital");
  ddataIn->read(base+"-digital.dat", digital, pd);
}

void TrialData::readCCD(XML &myxml, QString base, ProgressDialog *pd) {
  QDomElement ccd = myxml.find("ccd");
  /* We now have three versions of ccd data storage
     (1) The very oldest, with interleaved frames for precisely two cameras.
         This we recognize by the existence of a "type" attribute at <ccd>
	 level.
     (2) The one used until 3/15/2011, with any number of cameras, but
         timing shared between cameras. This we recognize by the existence
	 of a "rate" attribute at <ccd> level.
     (3) The new one of 3/15/2011. Each <camera> now has independent timing.
  */

  if (ccd.hasAttribute("type"))
    throw Exception("TrialData",
		    "Loading of ancient CCD file format not yet implemented");
  // So now we know we are modern or at least not ancient.

  QStringList newcams;
  QSet<QString> oldset;
  QSet<QString> newset;
  foreach (QString id, camids)
    oldset.insert(id);

  QString ccdfn = base + "-ccd.dat";
  QFile ccdf(ccdfn);
  if (!ccdf.open(QIODevice::ReadOnly))
    throw Exception("Trial",
		    QString("Cannot open '%1' for reading").arg(ccdfn),
		    "read");
  int ncams = 0;
  for (QDomElement cam = ccd.firstChildElement("camera");
       !cam.isNull(); cam = cam.nextSiblingElement("camera"))
    ncams++;

  for (QDomElement cam = ccd.firstChildElement("camera");
       !cam.isNull(); cam = cam.nextSiblingElement("camera")) {
    QString id = cam.attribute("name");
    /* This is to make format (2) look like format (3). */
    if (!cam.hasAttribute("rate"))
      cam.setAttribute("rate", ccd.attribute("rate"));
    if (!cam.hasAttribute("delay"))
      cam.setAttribute("delay", ccd.attribute("delay"));
    /* End of format conversion. */
    newset.insert(id);
    newcams.append(id);
    if (!oldset.contains(id)) {
      ccddata[id] = new CCDData;
      add(ccddata[id]); // to the keyagg
    }
    if (pd)
      pd->push(90.0/ncams, QString("Loading CCD data: %1").arg(id));
    ccddata[id]->read(ccdf, cam, pd);
    if (pd)
      pd->pop();
  }
  ccdf.close();

  foreach (QString id, camids) {
    if (!newset.contains(id)) {
      remove(ccddata[id]); // from the keyagg
      delete ccddata[id];
      ccddata.remove(id);
      ccdplace.remove(id);
      campairs.remove(id);
    }
  }

  camids = newcams;
  Dbg() << "TrialData " << (void*)this << " 450: camids " << camids.join(" ");

  foreach (QString id, camids)
    ccdplace[id] = ccddata[id]->dataToCanvas();

  foreach (QString id, camids)
    campairs[id] = CamPair(id);
  
  for (QDomElement cam = ccd.firstChildElement("camera");
       !cam.isNull(); cam = cam.nextSiblingElement("camera")) {
    QString id = cam.attribute("name");
    if (cam.hasAttribute("donor")) {
      QString did = cam.attribute("donor");
      campairs[id] = campairs[did] = CamPair(did, id);
    }
  }
  if (pd)
    pd->progress(100);

  if (newset!=oldset)
    emit newCameras();
}

void TrialData::clearCCD() {
  foreach (CCDData *d, ccddata)
    d->zero();
}

void TrialData::refineCCDTiming() {
  if (!ddataIn)
    return;

  QSet<QString> gotrefined;
  foreach (QString camid, ccddata.keys()) {
    QString lineid = "Frame:" + camid;
    if (ddataIn->hasLine(lineid)) {
      DigitalData::DataType msk = 1;
      msk <<= ddataIn->findLine(lineid);
      int i0 = -1; // will be index of first pulse start
      int n = 0;
      int i1 = 0; // will be index of last pulse start
      int ie = -1; // will be index of first pulse end
      DigitalData::DataType const *ddat = ddataIn->allData();
      int N = ddataIn->getNumScans();
      bool old = false;
      for (int k=0; k<N; k++) {
	bool now = (ddat[k] & msk) != 0;
	if (now!=old) {
	  if (now) { // start of pulse
	    if (i0<0)
	      i0 = k;
	    i1 = k;
	    n++;
	  } else { // end of pulse
	    if (ie<0)
	      ie = k;
	  }
	  old = now;
	}
      }
      double f_hz = ddataIn->getSamplingFrequency();
      if (n>=2) {
	ccddata[camid]->setTimeBase(1e3 * i0 / f_hz,
				    1e3 * (i1-i0) / (n-1) / f_hz,
				    1e3 * (ie-i0) / f_hz);
	gotrefined.insert(camid);
      } else {
	Dbg() << "Couldn't set timebase for " << camid
	      << ": not enough frames:" << n;
      }
    } else {
      Dbg() << "trialdata: could not refine timing for camera " << camid;
    }
  }
  if (gotrefined.size()>0 && gotrefined.size()<ccddata.keys().size()) {
    // refine other cameras by guessing
    QString refid = *gotrefined.begin();
    double t0 = ccddata[refid]->getT0ms();
    double dt = ccddata[refid]->getDTms();
    double dur = ccddata[refid]->getDurms();
    for (QString camid: ccddata.keys()) { 
      if (!gotrefined.contains(camid)) {
	ccddata[camid]->setTimeBase(t0, dt, dur);
	Dbg() << "trialdata: estimated timebase for " << camid
	      << " from " << refid;
      }
    }
  }
}

void TrialData::updateEnums() {
  Enumerator::find("CAMERAS")->replace(QSet<QString>::fromList(cameras()));
  // It would be nice if we could do same with STIMCHS and AICHANNELS, perhaps
}
