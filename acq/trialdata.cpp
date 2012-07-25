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

#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QThread>
#include <QSet>
#include <xml/enumerator.h>
#include <base/dbg.h>
#include <video/videoprog.h>

#include "trialdata.h"
#include <base/keyagg.h>

TrialData::TrialData(): KeyAgg(0) {
  xml = 0;

  adataIn = new AnalogData(1024, 1, 1e4);
  adataOut = new AnalogData(1024, 1, 1e4);
  ddataIn = new DigitalData(1024, 1e4);
  ddataOut = new DigitalData(1024, 1e4);

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
}

void TrialData::useConnectedCameras(ParamTree const *ptree) {
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
      delete ccddata[id]; // automatically removes from signal list
      ccddata.remove(id);
      ccdplace.remove(id);
    }

  // add new cameras
  foreach (QString id, newcams) 
    if (!oldset.contains(id))
      add(ccddata[id] = new CCDData());

  camids = newcams;

  // place all cameras
  foreach (QString id, camids) {
    ccdplace[id] = camPlace(id, ptree);
    ccddata[id]->setDataToCanvas(ccdplace[id]);
  }
  
  if (newset!=oldset)
    emit newCameras();
}

CCDTimingDetail const &TrialData::timing(QString camid) const {
  CCDTimingDetail const *p = timing_[camid];
  if (p)
    return *p;
  else
    throw Exception("TrialData",
		    "Timing requested for unknown camera " + camid);
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

void TrialData::generalPrep(ParamTree const *ptree, bool concams) {
  if (xml)
    delete xml;

  fpath = ptree->find("filePath").toString();
  exptname = ptree->find("acquisition/exptname").toString();
  trialid = trialname(ptree);

  xml = new XML(0,"vsdscopeTrial");

  QDomElement settings = xml->append("settings");
  ptree->write(settings);

  QDomElement info = xml->append("info");
  info.setAttribute("expt",exptname);
  info.setAttribute("trial",trialid);
  if (snap)
    info.setAttribute("type","snapshot");
  else if (ptree->find("acqCCD/enable").toBool())
    info.setAttribute("type","ephys+vsd");
  else
    info.setAttribute("type","ephys");
  if (snap) {
    info.setAttribute("stim","0");
    info.setAttribute("duration","0 s");
  } else {
    info.setAttribute("stim",ptree->find("stimEphys/enable").toString());
    info.setAttribute("duration",ptree->find("acqEphys/acqTime").toString());
  }
  if (contEphys) {
    QString conttri = QString("%1")
      .arg(ptree->find("acquisition/contephys_trialno").toInt(),
	   int(3),int(10),QChar('0'));
    info.setAttribute("contephys", conttri);
  } else {
    info.setAttribute("contephys", "0");
    if (!snap) {
      QString acqfreq = ptree->find("acqEphys/acqFreq").toString();
      QDomElement analog = xml->append("analog");
      analog.setAttribute("rate",acqfreq);
      QDomElement digital = xml->append("digital");
      digital.setAttribute("rate",acqfreq);
    }
  }
  if (do_ccd) {
    QDomElement ccd = xml->append("ccd");
    if (concams)
      useConnectedCameras(ptree);
  }
  prep = true;
}

Transform TrialData::camPlace(QString camid, ParamTree const *ptree) {
  Connections::CamCon const *cam = Connections::findpCam(camid);
  if (!cam) {
    Dbg() << "TrialData::CamPlace: no data for camera " << camid;
    return Transform();
  }
  Transform t0 = cam->placement;
  if (ptree) {
    QRect reg(ptree->find("acqCCD/region").toRect());
    QRect bin(ptree->find("acqCCD/binning").toRect());
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

void TrialData::prepare(ParamTree const *ptree) {
  prepare(ptree, true);
}

void TrialData::prepare(ParamTree const *ptree, bool concams) {
  snap=false;
  timing_.prepTrial(ptree);
  contEphys = ptree->find("acquisition/contEphys").toBool();
  /* contEphys tracks whether continuous e'phys acq is simultaneously
     happening (in "ContAcq"). We are *not* doing that ourselves here. */
  do_ccd = ptree->find("acqCCD/enable").toBool();
  generalPrep(ptree, concams);
}

void TrialData::prepareSnapshot(ParamTree const *ptree) {
  prepareSnapshot(ptree, true);
}

 void TrialData::prepareSnapshot(ParamTree const *ptree, bool concams) {
  snap=true;
  timing_.prepSnap(ptree);
  do_ccd = true;
  generalPrep(ptree, concams);
}

QString TrialData::trialname(ParamTree const *ptree) {
  int t = ptree->find("acquisition/trialno").toInt();
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
}

void TrialData::read(QString dir, QString exptname0, QString trialid0,
		     ParamTree *ptree_dest) {
  KeyGuard guard(*this);
  prep=false;

  fpath = dir;
  exptname = exptname0;
  trialid = trialid0;
  QString base = QString("%1/%2/%3").arg(dir).arg(exptname0).arg(trialid0);
  XML myxml(base + ".xml");

  QDomElement info = myxml.find("info");
  QDomElement settings = myxml.find("settings");

  bool own_ptree_dest = ptree_dest==0;
  //Dbg() << "trialdata::read " << exptname << "/" << trialid << "own_ptree_dest=" <<own_ptree_dest;
  
  if (own_ptree_dest)
    ptree_dest = new ParamTree(settings);
  else
    ptree_dest->read(settings);

  //dbg("trialdata: read ptree");
  ptree_dest->find("acquisition/exptname").set(exptname);
  ptree_dest->find("acquisition/trialno").set(trialid);
  
  contEphys = info.attribute("contephys")=="1";
  
  // the following ensures that we have stimulus data prepared
  // and that the xdataIn have the right sizes
  VideoProg::find().reset(ptree_dest);
  if (info.attribute("type")=="snapshot")
    prepareSnapshot(ptree_dest, false);
  else
    prepare(ptree_dest, false);

  if (own_ptree_dest)
    delete ptree_dest;
  if (xml)
    delete xml;
  xml = new XML(myxml);
  
  if (!snap && !contEphys) {
    readAnalog(myxml, base);
    readDigital(myxml, base);
  }

  if (do_ccd) 
    readCCD(myxml, base);
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
    QDomElement cam = ccddata[id]->write(ccdf, ccd);
    cam.setAttribute("name", id);
  }
  ccdf.close();
}

void TrialData::readAnalog(XML &myxml, QString base) {
  QDomElement analog = myxml.find("analog");
  adataIn->read(base+"-analog.dat", analog);
}

void TrialData::readDigital(XML &myxml, QString base) {
  QDomElement digital = myxml.find("digital");
  ddataIn->read(base+"-digital.dat", digital);
}

void TrialData::readCCD(XML &myxml, QString base) {
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
  QString ccdfn = base + "-ccd.dat";
  QFile ccdf(ccdfn);
  if (!ccdf.open(QIODevice::ReadOnly))
    throw Exception("Trial",
		    QString("Cannot open '%1' for reading").arg(ccdfn),
		    "read");
  QStringList newcams;
  QSet<QString> oldset;
  QSet<QString> newset;
  foreach (QString id, camids)
    oldset.insert(id);
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
    if (!oldset.contains(id))
      ccddata[id] = new CCDData;
    ccddata[id]->read(ccdf, cam);
  }
  ccdf.close();
  foreach (QString id, camids) {
    if (!newset.contains(id)) {
      delete ccddata[id];
      ccddata.remove(id);
      ccdplace.remove(id);
    }
  }
  camids = newcams;
  foreach (QString id, newcams)
    ccdplace[id] = ccddata[id]->dataToCanvas();

  if (newset!=oldset)
    emit newCameras();
}
