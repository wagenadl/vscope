// contacq.cpp

#include "contacq.h"
#include <base/memalloc.h>
#include <base/analogdata.h>
#include <base/digitaldata.h>
#include <acq/ephysacq.h>

#include <xml/enumerator.h>
#include <xml/connections.h>
#include <xml/paramtree.h>
#include <base/xml.h>
#include <base/base26.h>

#include <QDateTime>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QThread>

#include <base/dbg.h>
#include <math.h>

void ContAcq::dataAvFcn(int analogscans, int digitalscans, void *aux) {
  ContAcq *me = (ContAcq*)aux;
  me->dataAvailable(analogscans, digitalscans);
}

ContAcq::ContAcq() {
  ephysacq = 0;
  xml = 0;
  adataIn = new AnalogData(1024, 1, 1e4);
  ddataIn = new DigitalData(1024, 1e4);

  ephysacq = new EPhysAcq();
  ephysacq->setBuffer(adataIn,ddataIn);

  analogFile = 0;
  digitalFile = 0;
  analogStream = 0;
  digitalStream = 0;

  analogBinary = 0;
  
  prep=false;
  active = false;
}

ContAcq::~ContAcq() {
  if (active) {
    fprintf(stderr,"ContAcq destructed while active. Armageddon imminent.\n");
    try {
      stop();
    } catch(...) {
      fprintf(stderr,"ContAcq destructor caught exception. Armageddon imminent.\n");
    }
  }
  if (ephysacq)
    delete ephysacq;
  delete ddataIn;
  delete adataIn;

  if (analogStream)
    delete analogStream;
  if (digitalStream)
    delete digitalStream;
  if (analogFile)
    delete analogFile;
  if (digitalFile)
    delete digitalFile;
  if (xml)
    xml->write(xmlfn);
  delete xml;
}

QString ContAcq::prepare(class ParamTree *ptree) {
  dbg("ContAcq::prepare");
  if (active)
    throw Exception("ContAcq","Cannot prepare while active");
  ephysacq->prepare(ptree);

  dummy = ptree->find("acquisition/dummy").toBool();
  QString dir = ptree->find("filePath").toString();
  exptname = ptree->find("acquisition/exptname").toString();
  int trialno = ptree->find("acquisition/trialno").toInt();
  trialid = QString("%1").arg(trialno,int(3),int(10),QChar('0'));
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

  if (xml)
    delete xml;
  
  xml = new XML(0,"vsdscopeTrial");
  xmlfn = QString("%1/%2/%3.xml").arg(dir).arg(exptname).arg(trialname);

  if (analogStream)
    delete analogStream;
  if (analogFile)
    delete analogFile;
  analogFileOffset = 0;
  analogMax.clear();
  if (!dummy) {
    analogFile = new QFile(QString("%1/%2/%3-analog.dat").
			   arg(dir).arg(exptname).arg(trialname));
    if (!analogFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
      throw Exception("ContAcq","Cannot write -analog.dat","prepare");
    analogStream = new QDataStream(analogFile);
  }
  
  if (digitalStream)
    delete digitalStream;
  if (digitalFile)
    delete digitalFile;
  digitalFileOffset = 0;
  if (!dummy) {
    digitalFile = new QFile(QString("%1/%2/%3-digital.dat").
			    arg(dir).arg(exptname).arg(trialname));
    if (!digitalFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
      throw Exception("ContAcq","Cannot write -digital.dat","prepare");
    digitalStream = new QDataStream(digitalFile);
  }

  ptree->find("acquisition/contephys_trialno").setInt(trialno);
  QDomElement settings = xml->append("settings");
  ptree->write(settings);

  info_elt = xml->append("info");
  info_elt.setAttribute("expt",exptname);
  info_elt.setAttribute("trial",trialid);
  info_elt.setAttribute("type","contephys");
  info_elt.setAttribute("stim","0");
  info_elt.setAttribute("duration","0 s"); // change later
  
  ana_elt = xml->append("analog");
  digi_elt = xml->append("digital");

  QString acqfreq = ptree->find("acqEphys/acqFreq").toString();
  analogRate_Hz = ptree->find("acqEphys/acqFreq").toDouble();
  ana_elt.setAttribute("rate",acqfreq);
  digi_elt.setAttribute("rate",acqfreq);
  ana_elt.setAttribute("channels",QString::number(adataIn->getNumChannels()));
  ana_elt.setAttribute("type","int16");
  ana_elt.setAttribute("typebytes","2");
  ana_elt.setAttribute("scans","0"); // change later
  int nchans = adataIn->getNumChannels();
  for (int idx=0; idx<nchans; idx++) {
    QDomElement channel = xml->append("channel",ana_elt);
    channel.setAttribute("idx",QString("%1").arg(idx));
    QString chname = adataIn->getChannelAtIndex(idx);
    channel.setAttribute("id",chname);
    QString unit = "mV";
    Connections::AIChannel const *chinfo = Connections::findpAI(chname);
    if (chinfo)
      unit = chinfo->unit;
    channel.setAttribute("scale","0 " + unit);
  }

  digi_elt.setAttribute("type","uint32");
  digi_elt.setAttribute("typebytes","4");
  digi_elt.setAttribute("scans","0"); // change later
  Enumerator *dlines = Enumerator::find("DIGILINES");
  QStringList dltags = dlines->getAllTags();
  for (QStringList::iterator i=dltags.begin(); i!=dltags.end(); ++i) {
    QString dlid = *i;
    int dlno = dlines->lookup(dlid);
    QDomElement line = xml->append("line",digi_elt);
    line.setAttribute("id",dlid);
    line.setAttribute("idx",QString::number(dlno));
  }

  if (!dummy) 
    xml->write(xmlfn);
  
  prep=true;
  return trialname;
}

void ContAcq::start() {
  if (!prep)
    throw Exception("ContAcq","Cannot start: not prepared");
  if (active)
    throw Exception("ContAcq","Cannot start: already running");

  ephysacq->setDataAvFcn(&dataAvFcn,(void*)this);
    
  QDateTime now(QDateTime::currentDateTime());
  info_elt.setAttribute("date",now.toString("yyMMdd"));
  info_elt.setAttribute("time",now.toString("hhmmss"));

  ephysacq->start();
  active = true;
}

void ContAcq::stop() {
  if (!active) {
    Dbg() << "ContAcq: Cannot stop: not active";
    return;
  }

  if (ephysacq) {
    ephysacq->setDataAvFcn(0,0);
    if (ephysacq->isActive())
      ephysacq->abort();
  }

  if (analogStream) {
    delete analogStream;
    analogStream = 0;
  }
  if (analogFile) {
    analogFile->close();
    delete analogFile;
    analogFile = 0;
  }

  if (digitalStream) {
    delete digitalStream;
    digitalStream = 0;
  }
  if (digitalFile) {
    digitalFile->close();
    delete digitalFile;
    digitalFile = 0;
  }

  if (xml) {
    info_elt.setAttribute("duration",QString("%1 s")
			  .arg(analogFileOffset/analogRate_Hz));
    ana_elt.setAttribute("scans",QString::number(analogFileOffset));
    digi_elt.setAttribute("scans",QString::number(digitalFileOffset));
    if (!dummy) 
      xml->write(xmlfn);
    delete xml;
    xml = 0;
  }

  active = false;
}

void ContAcq::dataAvailable(int analogscans, int digitalscans) {
  if (analogscans) {
    int nchans = adataIn->getNumChannels();
    if (analogBinary && analogBinaryScans<analogscans)
      delete [] analogBinary;
    if (!analogBinary || analogBinaryScans<analogscans) {
      analogBinaryScans = analogscans;
      analogBinary = 0;
      analogBinary = memalloc<int16_t>(analogBinaryScans * nchans,
				       "ContAcq::dataAvailable");
    }
    bool anymax=false;
    if (analogMax.isEmpty()) 
      for (int c=0; c<nchans; c++)
	analogMax.push_back(0);
    for (int c=0; c<nchans; c++) {
      double mx = 0;
      double const *dat = adataIn->channelData(adataIn->getChannelAtIndex(c));
      for (int s=0; s<analogscans; s++) {
	double x = fabs(*dat);
	dat += nchans;
	if (x>mx)
	  mx=x;
      }
      if (mx>analogMax[c]) {
	anymax=true;
	analogMax[c] = 2*mx; // let's have some headroom so we don't do this too often.
      }
    }
    if (anymax) {
      // must write new scale information to xml
      QDomElement elt = xml->append("scale",ana_elt);
      elt.setAttribute("startscan",QString::number(analogFileOffset));
      for (int idx=0; idx<nchans; idx++) {
	QDomElement channel = xml->append("channel",elt);
	channel.setAttribute("idx",QString("%1").arg(idx));
	double scl = analogMax[idx]/32767;
	QString chname = adataIn->getChannelAtIndex(idx);
	Connections::AIChannel const *chinfo = Connections::findpAI(chname);
	QString unit = "V";
	if (chinfo) {
	  scl = scl * chinfo->scale;
	  unit = chinfo->unit;
	}
	channel.setAttribute("scale",QString("%1 %2")
			     .arg(scl,0,'f',9).arg(unit));
      }
      if (!dummy)
	xml->write(xmlfn);
    }
    
    double const *src = adataIn->allData();
    int16_t *dst = analogBinary;
    for (int s=0; s<analogscans; s++) {
      for (int c=0; c<nchans; c++) {
	double x = *src++;
	int16_t y = int16_t(x*32767/analogMax[c]);
	*dst++ = y;
      }
    }
    if (!dummy)
      analogStream->writeRawData((char*)analogBinary,2*analogscans*nchans);
    analogFileOffset += analogscans;
  }

  if (digitalscans) {
    if (!dummy)
      digitalStream->writeRawData((char*)ddataIn->allData(),4*digitalscans);
    digitalFileOffset += digitalscans;
  }

  emit dataSaved(analogscans, digitalscans);
}

void ContAcq::markTrial(QString trialno) {
  if (active) {
    QDomElement e = xml->append("trial",info_elt);
    e.setAttribute("id",trialno);
    e.setAttribute("ascan",QString::number(analogFileOffset));
    e.setAttribute("dscan",QString::number(digitalFileOffset));
    if (!dummy)
      xml->write(xmlfn);
    if (analogFile)
      analogFile->flush();
    if (digitalFile)
      digitalFile->flush();
  }
}
