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

TrialData::TrialData() {
  xml = 0;

  adataIn = new AnalogData(1024,1);
  adataOut = new AnalogData(1024,1);
  ddataIn = new DigitalData(1024);
  ddataOut = new DigitalData(1024);

  Enumerator *camenum = Enumerator::find("CAMERAS");
  for (int k=camenum->getSmallestValue();
       k<=camenum->getLargestValue(); k++) {
    QString camid = camenum->reverseLookup(k);
    camidx[camid] = ccddata.size();
    camids.append(camid);
    CCDData *d = new CCDData();
    ccddata.append(d);
  }

  prep=false;
  snap=false;
}

TrialData::~TrialData() {

  for (QVector<CCDData *>::iterator i=ccddata.begin();
       i!=ccddata.end(); ++i) 
    delete *i;

  delete ddataOut;
  delete adataOut;
  delete ddataIn;
  delete adataIn;
}

CCDData const *TrialData::ccdData(QString camid) const {
  if (camidx.contains(camid))
    return ccddata[camidx[camid]];
  else
    return 0;
}

CCDData *TrialData::ccdData(QString camid) {
  if (camidx.contains(camid))
    return ccddata[camidx[camid]];
  else
    return 0;
}

void TrialData::generalPrep(ParamTree const *ptree) {
  if (xml)
    delete xml;

  exptname = ptree->find("acquisition/_exptname").toString();
  int trialno = ptree->find("acquisition/_trialno").toInt();
  trialid = QString("%1").arg(trialno,int(3),int(10),QChar('0'));

  xml = new XML(0,"vsdscopeTrial");

  QDomElement settings = xml->append("settings");
  ptree->write(settings);
  //QDomElement connections = xml->append("connections");
  //Connections::writeXML(connections);

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
  }
}

void TrialData::prepare(ParamTree const *ptree) {
  snap=false;
  contEphys = ptree->find("acquisition/contEphys").toBool();
  /* contEphys tracks whether continuous e'phys acq is simultaneously
     happening (in "ContAcq"). We are *not* doing that ourselves here. */
  do_ccd = ptree->find("acqCCD/enable").toBool();
  generalPrep(ptree);
  prep = true;
}

void TrialData::prepareSnapshot(ParamTree const *ptree) {
  snap=true;
  do_ccd = true;
  generalPrep(ptree);
  prep = true;
}

QString TrialData::write(QString dir) const {
  if (!xml)
    throw Exception("Trial","Cannot write - not prepared");

  QDir d; d.mkpath(QString("%1/%2").arg(dir).arg(exptname));

  int append=0;
  QString trialname = trialid;
  while (true) {
    QString testname = QString("%1/%2/%3.xml").
      arg(dir).arg(exptname).arg(trialname);
    if (QFile(testname).exists())
      trialname = trialid+num2az(++append);
    else
      break;
  }

  QString base = QString("%1/%2/%3").arg(dir).arg(exptname).arg(trialname);
  if (!snap && !contEphys) {
    writeAnalog(base);
    writeDigital(base);
  }
  
  if (do_ccd) {
    writeCCD(base);
  }

  // xml
  xml->write(base + ".xml");
  return trialname;
}

static double getScale(QString str) {
  /* Given either a voltage, e.g., "3.4 uV", or a current, e.g., "1.2 pA",
     converts to canonical units, i.e., millivolts or nanoamperes.
  */
  QString qty;
  if (str.endsWith("A"))
    qty = "current";
  else
    qty = "voltage";
  Param scale(qty);
  scale.set(str);
  double scl = scale.toDouble();
  return scl;
}

void TrialData::read(QString dir, QString exptname0, QString trialid0,
		     ParamTree *ptree_dest) {
  prep=false;

  exptname = exptname0;
  trialid = trialid0;
  QString base = QString("%1/%2/%3").arg(dir).arg(exptname0).arg(trialid0);
  XML myxml(base + ".xml");

  QDomElement info = myxml.find("info");
  QDomElement settings = myxml.find("settings");

  bool own_ptree_dest = ptree_dest==0;
  Dbg() << "trialdata::read " << exptname << "/" << trialid << "own_ptree_dest=" <<own_ptree_dest;
  
  if (own_ptree_dest)
    ptree_dest = new ParamTree(settings);
  else
    ptree_dest->read(settings);

  dbg("trialdata: read ptree");
  ptree_dest->find("acquisition/_exptname").set(exptname);
  ptree_dest->find("acquisition/_trialno").set(trialid);
  
  contEphys = info.attribute("contephys")=="1";
  
  // the following ensures that we have stimulus data prepared
  // and that the xdataIn have the right sizes
  VideoProg::find().reset(ptree_dest);
  if (info.attribute("type")=="snapshot")
    prepareSnapshot(ptree_dest);
  else
    prepare(ptree_dest);
  if (own_ptree_dest)
    delete ptree_dest;
  if (xml)
    delete xml;
  xml = new XML(myxml);
  
  if (!snap && !contEphys) {
    readAnalog(myxml, base);
    readDigital(myxml, base);
  }

  if (do_ccd) {
    readCCD(myxml, base);
  }
}

void TrialData::writeAnalog(QString base) const {
  QMap<int,double> scale = adataIn->writeInt16(base+"-analog.dat");
  QDomElement analog = xml->find("analog");
  analog.setAttribute("channels",QString::number(adataIn->getNumChannels()));
  analog.setAttribute("type","int16");
  analog.setAttribute("typebytes","2");
  analog.setAttribute("scans",QString::number(adataIn->getNumScans()));
  Enumerator *aichan = Enumerator::find("AICHAN");
  for (int idx=0; idx<adataIn->getNumChannels(); idx++) {
    int chn = adataIn->getChannelAtIndex(idx);
    double scl = scale[chn];
    QString chname = aichan->reverseLookup(chn);
    Connections::AIChannel const *chinfo = Connections::findpAI(chname);
    QString unit = "V";
    if (chinfo) {
      scl = scl * chinfo->scale;
      unit = chinfo->unit;
    }
    QDomElement channel = xml->append("channel",analog);
    channel.setAttribute("idx",QString("%1").arg(idx));
    channel.setAttribute("chn",QString("%1").arg(chn));
    channel.setAttribute("id",chname);
    channel.setAttribute("scale",QString("%1 %2").arg(scl,0,'f',9).arg(unit));
  }
}

void TrialData::writeDigital(QString base) const {
  ddataIn->writeUInt32(base + "-digital.dat");
  QDomElement digital = xml->find("digital");
  digital.setAttribute("type","uint32");
  digital.setAttribute("typebytes","4");
  digital.setAttribute("scans",QString::number(ddataIn->getNumScans()));
  Enumerator *dlines = Enumerator::find("DIGILINES");
  QStringList dltags = dlines->getAllTags();
  for (QStringList::iterator i=dltags.begin(); i!=dltags.end(); ++i) {
    QString dlid = *i;
    int dlno = dlines->lookup(dlid);
    QDomElement line = xml->append("line",digital);
    line.setAttribute("id",dlid);
    line.setAttribute("idx",QString::number(dlno));
  }
}

void TrialData::writeCCD(QString base) const {
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
  }
}

void TrialData::readAnalog(XML &myxml, QString base) {
  QDomElement analog = myxml.find("analog");
  bool ok;
  
  // read analog
  int nscans = analog.attribute("scans").toInt(&ok);
  if (!ok)
    throw Exception("Trial","Cannot read number of scans from xml","read");
  int nchans = analog.attribute("channels").toInt(&ok);
  if (!ok)
    throw Exception("Trial","Cannot read number of channels from xml",
		    "read");
  QString dtyp = analog.attribute("type");
  if (dtyp!="int16")
    throw Exception("Trial",
		    "Only know how read analog data of type 'int16', not '"
		    + dtyp + "'", "read");

  adataIn->reshape(nscans, nchans);
  
  Enumerator *aichan = Enumerator::find("AICHAN");
  QMap<int,double> scalemap;
  for (QDomElement elt=analog.firstChildElement("channel");
       !elt.isNull(); elt=elt.nextSiblingElement("channel")) {
    int idx = elt.attribute("idx").toInt(&ok);
    if (!ok)
      throw Exception("Trial","Cannot read channel index from xml","read");
    int chn = elt.attribute("chn").toInt(&ok);
    if (!ok)
      throw Exception("Trial","Cannot read channel number from xml","read");
    adataIn->defineChannel(idx,chn);
    double scl = getScale(elt.attribute("scale"));
    try {
      QString chname = aichan->reverseLookup(chn);
      Connections::AIChannel const *chinfo = Connections::findpAI(chname);
      if (chinfo) 
	scl /= getScale(QString("%1 %2").arg(chinfo->scale).arg(chinfo->unit));
      //scl = scl / chinfo->scale;
    } catch(...) {
      Dbg() << "TrialData:: unknown channel #" << chn;
    }
    scalemap[chn] = scl;
  }
  adataIn->readInt16(base + "-analog.dat", scalemap);
}

void TrialData::readDigital(XML &myxml, QString base) {
  QDomElement digital = myxml.find("digital");
  bool ok;
  int nscans = digital.attribute("scans").toInt(&ok);
  if (!ok)
    throw Exception("Trial","Cannot read number of scans from xml","read");

  ddataIn->reshape(nscans);

  ddataIn->readUInt32(base + "-digital.dat");
}

void TrialData::readCCD(XML &myxml, QString base) {
  QDomElement ccd = myxml.find("ccd");
  // read ccd
  bool ok;
  // read attributes from xml
  
  bool oldstyle = ccd.hasAttribute("type");
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
  
  QVector<QString> camsstored;
  camsstored.fill("",ncam);
  
  if (oldstyle) 
    readCCDOldStyle(camsstored, ccd);
  else
    readCCDNewStyle(camsstored, ccd);
  
  // read actual ccd data
  QString ccdfn = base + "-ccd.dat";
  QFile ccdf(ccdfn);
  if (!ccdf.open(QIODevice::ReadOnly))
    throw Exception("Trial",
		    QString("Cannot open '%1' for reading").arg(ccdfn),
		    "read");
  
  if (oldstyle) {
    int nfrm = ccd.attribute("frames").toInt();
    int nser = ccd.attribute("serpix").toInt();
    int npar = ccd.attribute("parpix").toInt();
    int frpix = nser*npar;
    for (int fr=0; fr<nfrm; fr++) {
      if (ccdf.read((char *)ccddata[0]->frameData(fr),2*frpix)!=2*frpix)
	throw Exception("Trial","Cannot read CCD data","read");
      if (ccdf.read((char *)ccddata[1]->frameData(fr),2*frpix)!=2*frpix)
	throw Exception("Trial","Cannot read CCD data","read");
      }
  } else {
    for (int idx=0; idx<ncam; idx++) {
      int k = camidx[camsstored[idx]];
      int nfrm = ccddata[k]->getNFrames();
      int nser = ccddata[k]->getSerPix();
      int npar = ccddata[k]->getParPix();
      int frpix = nser*npar;
      for (int fr=0; fr<nfrm; fr++) 
	if (ccdf.read((char *)ccddata[k]->frameData(fr),2*frpix)!=2*frpix)
	  throw Exception("Trial","Cannot read CCD data","read");
    }
  }
  ccdf.close();
}

void TrialData::readCCDOldStyle(QVector<QString> &camsstored,
				QDomElement ccd) {
  // This is an old-style file with interleaved identical frames
  int ndata = ccddata.size();
  int ncam = camsstored.size();
  bool ok;  
  int nfrm = ccd.attribute("frames").toInt(&ok);
  if (!ok)
    throw Exception("Trial","Cannot read number of frames from xml","readOld");
  int nser = ccd.attribute("serpix").toInt(&ok);
  if (!ok)
    throw Exception("Trial",
		    "Cannot read number of serial pixels from xml","readOld");
  int npar = ccd.attribute("parpix").toInt(&ok);
  if (!ok)
    throw Exception("Trial",
		    "Cannot read number of parallel pixels from xml","readOld");
  
  //for (int k=0; k<ndata; k++) 
  //  ccddata[k]->setTimeBase(delay_ms,1e3/rate_hz);
  
  // check that dimensions make sense
  if (ccd.attribute("type")!="uint16")
    throw Exception("Trial",
		    "Only know how to read ccd data of type 'uint16', not '"
		    + ccd.attribute("type") + "'", "readOld");
  for (int k=0; k<ndata; k++)
    if (nfrm!=ccddata[k]->getNFrames())
      throw Exception("Trial",
		      QString("Frame count mismatch (%1; expected %2)")
		      .arg(nfrm)
		      .arg(ccddata[k]->getNFrames()),"readOld");
  for (int k=0; k<ndata; k++)
    if (nser!=ccddata[k]->getSerPix())
      throw Exception("Trial",
		      QString("Ser. pix count mismatch (%1; expected %2)")
		      .arg(nser)
		      .arg(ccddata[k]->getSerPix()),"readOld");
  for (int k=0; k<ndata; k++)
    if (npar!=ccddata[k]->getParPix())
      throw Exception("Trial",
		      QString("Par. pix count mismatch (%1; expected %2)")
		      .arg(npar)
		      .arg(ccddata[k]->getParPix()),"readOld");
  if (ncam!=2)
    throw Exception("Trial",
		    QString("Camera count mismatch (%1; expected 2)")
		    .arg(ncam),"readOld");
  
  // check that camera identities match expectations
  int okmask=0;
  for (QDomElement elt=ccd.firstChildElement("camera");
       !elt.isNull(); elt=elt.nextSiblingElement("camera")) {
    bool ok;
    int idx = elt.attribute("idx").toInt(&ok);
    QString name = elt.attribute("name");
    if (idx==0) {
      if (name.toLower()==camids[0].toLower())
	okmask|=1;
      else
	throw Exception("Trial","Expected camera #0 to be cc","readOld");
    } else if (idx==1) {
      if (name.toLower()==camids[1].toLower())
	okmask|=2;
      else
	throw Exception("Trial","Expected camera #1 to be ox","readOld");
    } else {
      throw Exception("Trial",
		      "Expected cameras to be numbered 0 and 1","readOld");
    }
  }
  if (okmask!=3)
    throw Exception("Trial","Insufficient camera definitions found in xml",
		    "readOld");
  camsstored[0] = camids[0];
  camsstored[1] = camids[1];
}

void TrialData::readCCDNewStyle(QVector<QString> &camsstored,
				QDomElement ccd) {
  // int ndata = ccddata.size();
  int ncam = camsstored.size();
  double ccd_rate_Hz = UnitQty::str2num(ccd.attribute("rate"),"Hz");
  double ccd_delay_s = UnitQty::str2num(ccd.attribute("delay"),"s");
  Dbg() << "Hz=" << ccd_rate_Hz << " delay=" << ccd_delay_s;
  QSet<int> havecam;
  for (QDomElement elt=ccd.firstChildElement("camera");
       !elt.isNull(); elt=elt.nextSiblingElement("camera")) {
    bool ok;
    int idx = elt.attribute("idx").toInt(&ok);
    if (!ok)
      throw Exception("Trial","Cannot read camera index","read");
    havecam.insert(idx);
    QString name = elt.attribute("name");
    if (!camidx.contains(name))
      throw Exception("Trial",
		      "File contains data from a camera I don't know about",
		      "read");
    int k = camidx[name];
    
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
    
    ccddata[k]->reshape(nser, npar, nfrm);
    ccddata[k]->setTimeBase(ccd_delay_s*1e3,1e3/ccd_rate_Hz);
    
    //if (nfrm!=ccddata[k]->getNFrames())
    //  throw Exception("Trial",
    //		      QString("Frame count mismatch (%1; expected %2)")
    //		      .arg(nfrm)
    //		      .arg(ccddata[k]->getNFrames()),"read");
    //for (int k=0; k<ndata; k++)
    //  if (nser!=ccddata[k]->getSerPix())
    //	throw Exception("Trial",
    //			QString("Ser. pix count mismatch (%1; expected %2)")
    //			.arg(nser)
    //			.arg(ccddata[k]->getSerPix()),"read");
    //for (int k=0; k<ndata; k++)
    //  if (npar!=ccddata[k]->getParPix())
    //	throw Exception("Trial",
    //			QString("Par. pix count mismatch (%1; expected %2)")
    //			.arg(npar)
    //			.arg(ccddata[k]->getParPix()),"read");
    camsstored[idx] = name;
  }
  if (havecam.size()!=ncam)
    throw Exception("Trial",
		    QString("Camera count mismatch (%1; expected %2)")
		    .arg(havecam.size()).arg(ncam));
}
