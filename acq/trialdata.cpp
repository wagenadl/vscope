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

  renewCameras(Connections::allCams());
  placeCameras();

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

bool TrialData::renewCameras(QList<QString> newcams) {
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

  return newset!=oldset;
}

void TrialData::placeCameras(ParamTree const *ptree) {
  // place all cameras
  foreach (QString id, camids) {
    ccdplace[id] = camPlace(id, ptree);
    ccddata[id]->setDataToCanvas(ccdplace[id]);
  }
}

void TrialData::placeCameras(QDomElement ccd) {
  QSet<QString> inXML;
  for (QDomElement cam=ccd.firstChildElement("camera");
       !cam.isNull(); cam=cam.nextSiblingElement("camera")) {
    QString id = cam.attribute("name");
    if (camids.contains(id)) {
      inXML.insert(id);
      
      ccdplace[id] = ...;
      ccddata[id]->setDataToCanvas(ccdplace[id]);    
    }
  }
  foreach (QString id, camids)
    if (!inXML.contains(id)) 
      ccddata[id]->setDataToCanvas(ccdplace[id] = Transform());     
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

void TrialData::generalPrep(ParamTree const *ptree, bool newccds) {
  if (xml)
    delete xml;

  fpath = ptree->find("_filePath").toString();
  exptname = ptree->find("acquisition/_exptname").toString();
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
      .arg(ptree->find("acquisition/_contephys_trialno").toInt(),
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
    ccd.setAttribute("rate", ptree->find("acqCCD/rate").toString());
    ccd.setAttribute("delay", ptree->find("acqCCD/delay").toString());
    if (newccds)
      renewCameras(Connections::allCams(), ptree);
  }
  prep = true;
}

Transform TrialData::camPlace(QString camid, ParamTree const *ptree) {
  Connections::CamCon *cam = Connections::findpCam(camid);
  if (!cam) {
    Dbg() << "TrialData::CamPlace: no data for camera " << camid;
    return Transform();
  }
  Transform t0 = cam->placement;
  if (ptree) {
    QRect reg(ptree->find("acqCCD/region").toRect());
    QRect bin(ptree->find("acqCCD/binning").toRect());
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

void TrialData::prepare(ParamTree const *ptree, bool newccds) {
  snap=false;
  timing_.prepTrial(ptree);
  contEphys = ptree->find("acquisition/contEphys").toBool();
  /* contEphys tracks whether continuous e'phys acq is simultaneously
     happening (in "ContAcq"). We are *not* doing that ourselves here. */
  do_ccd = ptree->find("acqCCD/enable").toBool();
  generalPrep(ptree, newccds);
}

void TrialData::prepareSnapshot(ParamTree const *ptree) {
  prepareSnapshot(ptree, true);
}

 void TrialData::prepareSnapshot(ParamTree const *ptree, bool newccds) {
  snap=true;
  timing_.prepSnap(ptree);
  do_ccd = true;
  generalPrep(ptree, newccds);
}

QString TrialData::trialname(ParamTree const *ptree) {
  int t = ptree->find("acquisition/_trialno").toInt();
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
  ptree_dest->find("acquisition/_exptname").set(exptname);
  ptree_dest->find("acquisition/_trialno").set(trialid);
  
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
  // Soon I should make ccddata be able to read and write by itself
  QString ccdfn = base + "-ccd.dat";
  QFile ccdf(ccdfn);
  if (!ccdf.open(QIODevice::WriteOnly))
    throw Exception("Trial",
		    QString("Cannot open '%1' for writing").arg(ccdfn),
		    "write");
  
  int ndata = ccddata.size();
  for (int k=0; k<ndata; k++) {
    if (ccddata[k]) {
      int nfr = ccddata[k]->getNFrames();
      int frpix = ccddata[k]->getFramePix();
      for (int fr=0; fr<nfr; fr++) 
	if (ccdf.write((char const *)ccddata[k]->frameData(fr),2*frpix)
	    != 2*frpix)
	  throw Exception("Trial","Cannot write CCD data","write");
    }
  }
  ccdf.close();
  QDomElement ccd = xml->find("ccd");
  ccd.setAttribute("cameras",QString::number(ndata));
  for (int k=0; k<ndata; k++) {
    QDomElement cam = xml->append("camera",ccd);
    cam.setAttribute("idx",QString::number(k));
    cam.setAttribute("name",camids[k]);
    cam.setAttribute("type","uint16");
    cam.setAttribute("typebytes","2");
    cam.setAttribute("serpix",QString::number(ccddata[k]->getSerPix()));
    cam.setAttribute("parpix",QString::number(ccddata[k]->getParPix()));
    cam.setAttribute("frames",QString::number(ccddata[k]->getNFrames()));
    Connections::CamCon const &c = Connections::findCam(camids[k]);
    cam.setAttribute("flipx",c.flipx?"yes":"no");
    cam.setAttribute("flipy",c.flipy?"yes":"no");
    ccdplace[k].write(cam);
  }
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
  // read ccd
  bool ok;
  // read attributes from xml
  
  int ncam = ccd.attribute("cameras").toInt(&ok);
  if (!ok)
    throw Exception("Trial",
		    "Cannot read number of cameras from xml","read");
  Param delay("time"); delay.set(ccd.attribute("delay"));
  double delay_ms = delay.toDouble();
  Param rate("freq"); rate.set(ccd.attribute("rate"));
  double rate_hz = rate.toDouble();
  if (delay_ms>0 || rate_hz>0) {
    ; // actually, I should compare these against the ptree.
  }
  // I also should compare placement against the data already loaded from
  // the ptree in prepare()
  
  readCCDNewStyle(ccd);
  
  // read actual ccd data
  QString ccdfn = base + "-ccd.dat";
  QFile ccdf(ccdfn);
  if (!ccdf.open(QIODevice::ReadOnly))
    throw Exception("Trial",
		    QString("Cannot open '%1' for reading").arg(ccdfn),
		    "read");
  foreach (QString id, camids) {
    int nfrm = ccddata[id]->getNFrames();
    int nser = ccddata[id]->getSerPix();
    int npar = ccddata[id]->getParPix();
    int frpix = nser*npar;
    for (int fr=0; fr<nfrm; fr++) 
      if (ccdf.read((char *)ccddata[id]->frameData(fr),2*frpix)!=2*frpix)
	throw Exception("Trial","Cannot read CCD data","read");
  }
  ccdf.close();
}

void TrialData::readCCDNewStyle(QDomElement ccd) {
  double ccd_rate_Hz = UnitQty::str2num(ccd.attribute("rate"),"Hz");
  double ccd_delay_s = UnitQty::str2num(ccd.attribute("delay"),"s");

  QMap<int, QString> newcams;
  for (QDomElement elt=ccd.firstChildElement("camera");
       !elt.isNull(); elt=elt.nextSiblingElement("camera")) {
    bool ok;
    int idx = elt.attribute("idx").toInt(&ok);
    if (!ok)
      throw Exception("Trial","Cannot read camera index","read");
    QString name = elt.attribute("name");
    newcams[idx] = name;
  }

  if (renewCameras(newcams.values()))
    emit newCameras();
  
  for (QDomElement elt=ccd.firstChildElement("camera");
       !elt.isNull(); elt=elt.nextSiblingElement("camera")) {
    bool ok;
    int nfrm = elt.attribute("frames").toInt(&ok);
    if (!ok)
      throw Exception("Trial","Cannot read number of frames from xml","read");
    int nser = elt.attribute("serpix").toInt(&ok);
    if (!ok)
      throw Exception("Trial",
		      "Cannot read number of serial pixels from xml","read");
    int npar = elt.attribute("parpix").toInt(&ok);
    if (!ok)
      throw Exception("Trial",
		      "Cannot read number of parallel pixels from xml","read");
    if (elt.attribute("type")!="uint16")
      throw Exception("Trial",
		      "Only know how to read ccd data of type 'uint16', not '"
		      + ccd.attribute("type") + "'", "read");
    
    ccddata[id]->reshape(nser, npar, nfrm);
    ccddata[id]->setTimeBase(ccd_delay_s*1e3,1e3/ccd_rate_Hz);
    ccddata[id]->setDataToCanvas(ccdplace[id]);
    /* Placement should be read from xml. */
}
