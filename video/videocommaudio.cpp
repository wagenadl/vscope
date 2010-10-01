// videocommaudio.cpp

#include "videocommaudio.h"

#include <QApplication>
#include <QTime>
#include <xml/enumerator.h>
#include <video/videocmd.h>
#include <video/videoresp.h>
#include <base/exception.h>
#include <base/dbg.h>

#include <base/analogdata.h>
#include <base/digitaldata.h>
#include <xml/paramtree.h>
#include <xml/enumerator.h>
#include <base/ccdavoid.h>

/* Keep these in sync with vsdvideo/main/WaiterCli.H */
#define cmdSetProgram  1  // arg is a single byte prog no
#define cmdSetParX     4  // arg1 is a single byte par no, arg2 is a float

#define cmdGetProgName 2  // arg is a single byte prog no
#define cmdGetAllProgNames 3  // no args
#define cmdGetParamNames 5  // arg is a single byte prog no
#define cmdGetParamUnits 6  // arg is a single byte prog no
#define cmdGetParamValues 7  // arg1 is a single byte prog no, arg2 is a single byte par no



#define VIDEO_STARTBLIPDUR_MS 1.0
#define VIDEO_ENDBLIPDUR_MS 0.2
#define VIDEO_ENDBLIPCOUNT 10
#define VIDEO_ENDBLIPPERIOD_MS 0.6

#ifdef vsdLINUX
#define VC_DUMMY
#include <base/base26.h>
#endif

VideoCommAudio::VideoCommAudio() {
  vidcmd = 0;
  vidresp = 0;
  vidcmd = new VideoCmd(Enumerator::find("DIGILINES")->lookup("VideoCmd"));
  vidresp = new VideoResp(Enumerator::find("DIGILINES")->lookup("VideoResp"));
}

VideoCommAudio::~VideoCommAudio() {
  if (vidcmd)
    delete vidcmd;
  if (vidresp)
    delete vidresp;
}

QByteArray VideoCommAudio::waitForMessage(int max_ms) {
  unsigned char resp[VideoResp::MAXMSGLEN];
  int n=0;
  QTime t0 = QTime::currentTime();
  while ((n=vidresp->retrieveMessage(resp))==0) {
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 50);
    if (t0.msecsTo(QTime::currentTime()) > max_ms)
      break;
  }
  if (n<=0) {
    vidresp->abort();
    if (n==0)
      throw Exception("VideoCommAudio",
		      "No message received: Time out");
    else
      throw Exception("VideoCommAudio",
		      "No message received: Communication error");
  }
  return QByteArray((char *)resp,n);
}

#ifdef VC_DUMMY
void VideoCommAudio::setProgram(int prog) {
  dbg("VideoCommAudio: DUMMY setProgram: %i",prog);
  if (prog<=0 || prog>99)
    throw Exception("VideoCommAudio","Unreasonable program number","setProgram");
}

void VideoCommAudio::setPar(int id, double val) {
  dbg("VideoCommAudio: DUMMY setPar %c = %g",'A' + id - 1, val);
  if (id<=0 || id>99)
    throw Exception("VideoCommAudio","Unreasonable parameter number","setPar");
}

QString VideoCommAudio::getProgName(int prog) {
  QString name = "Prog-";
  name += num2az(prog);
  dbg("VideoCommAudio: DUMMY getProgName %i -> %s",
      'A' + prog - 1, qPrintable(name));
  return name;
}

QMap<int,QString> VideoCommAudio::getAllProgNames() {
  dbg("VideoCommAudio: DUMMY getAllProgNames");
  QMap<int,QString> names;
  for (int i=0; i<5; i++) {
    for (int j=1; j<=5; j++) {
      int prog = i*10 + j;
      QString name = "Prog-";
      name += num2az(prog);
      names[prog] = name;
    }
  }
  return names;
}

QStringList VideoCommAudio::getParNames(int prog) {
  dbg("VideoCommAudio: DUMMY getParNames %i",prog);
  return QStringList() << QString("Freq%1").arg(prog) << "Period" << "Angle";
}

QStringList VideoCommAudio::getParUnits(int prog) {
  dbg("VideoCommAudio: DUMMY getParUnits %i",prog);
  return QStringList() << "Hz" << "s" << QString("rad%1").arg(prog);
}

QStringList VideoCommAudio::getParValues(int prog, int par) {
  dbg("VideoCommAudio: DUMMY getParValues %i %i",prog, par);
  return QStringList() << "0.01" << "0.02" << "0.03" << QString("%1.%2").arg(prog).arg(par);
}


#else
void VideoCommAudio::setProgram(int prog) {
  dbg("setProgram %i",prog);
  if (prog<=0 || prog>99)
    throw Exception("VideoCommAudio","Unreasonable program number","setProgram");
  vidcmd->createMessage(2);
  vidcmd->addByte(cmdSetProgram);
  vidcmd->addByte(prog);
  dbg("  setProgram: sending");
  vidcmd->checkAndSend();
  dbg("  setProgram done");
}

void VideoCommAudio::setPar(int id, double val) {
  dbg("setPar %i %g",id,val);
  if (id<=0 || id>99)
    throw Exception("VideoCommAudio","Unreasonable parameter number","setPar");
  float v = val;
  vidcmd->createMessage(2+sizeof(v));
  vidcmd->addByte(cmdSetParX);
  vidcmd->addByte(id);
  vidcmd->addBytes((unsigned char*)&v,sizeof(v));
  dbg("  setPar: sending");
  vidcmd->checkAndSend();
  dbg("  setPar: done");
}
			
QString VideoCommAudio::getProgName(int prog) {
  dbg("getProgName %i",prog);
  vidresp->prepare();
  vidcmd->createMessage(2);
  vidcmd->addByte(cmdGetProgName);
  vidcmd->addByte(prog);
  dbg("  getProgName: sending");
  vidcmd->checkAndSend();
  dbg("  getProgName: receiving");
  QString s = waitForMessage();
  dbg("  getProgName: received");
  dbg("  getProgName: progname=%s",qPrintable(s));
  return s;
}

QMap<int,QString> VideoCommAudio::getAllProgNames() {
  dbg("getAllProgNames");
  vidresp->prepare();
  vidcmd->createMessage(1);
  vidcmd->addByte(cmdGetAllProgNames);
  dbg("  getAllProgNames: sending");
  vidcmd->checkAndSend();
  dbg("  getAllProgNames: receiving");
  QByteArray progs(waitForMessage());
  dbg("  getAllProgNames: received");
  QMap<int,QString> prognames;
  QString accum="";
  for (int k=0; k<progs.size(); k++) {
    unsigned char x = progs[k];
    if (x>=128) {
      prognames[x-128] = accum;
      dbg("  getAllProgNames: progname[%i]=%s",x-128,qPrintable(accum));
      accum="";
    } else {
      accum += x;
    }
  }
  dbg("  getAllProgNames: done");
  return prognames;
}

QStringList VideoCommAudio::getParNames(int prog) {
  dbg("getParNames %i",prog);
  vidresp->prepare();
  vidcmd->createMessage(2);
  vidcmd->addByte(cmdGetParamNames);
  vidcmd->addByte(prog);
  dbg("  getParNames: sending");
  vidcmd->checkAndSend();
  dbg("  getParNames: receiving");
  QString names(waitForMessage());
  dbg("  getParNames: received");
  dbg("  getParNames -> %s",qPrintable(names));
  return names.split(":");
}

QStringList VideoCommAudio::getParUnits(int prog) {
  dbg("getParUnits %i",prog);
  vidresp->prepare();
  vidcmd->createMessage(2);
  vidcmd->addByte(cmdGetParamUnits);
  vidcmd->addByte(prog);
  dbg("  getParUnits: sending");
  vidcmd->checkAndSend();
  dbg("  getParUnits: receiving");
  QString units(waitForMessage());
  dbg("  getParUnits: received");
  dbg("  getParUnits -> %s",qPrintable(units));
  return units.split(":");
}

QStringList VideoCommAudio::getParValues(int prog, int par) {
  dbg("getParValues %i %i",prog,par);
  vidresp->prepare();
  vidcmd->createMessage(3);
  vidcmd->addByte(cmdGetParamValues);
  vidcmd->addByte(prog);
  vidcmd->addByte(par);
  dbg("  getParValues: sending");
  vidcmd->checkAndSend();
  dbg("  getParValues: receiving");
  QString values(waitForMessage());
  dbg("  getParValues: received");
  dbg("  getParValues -> %s",qPrintable(values));
  return values.split(" ");
}
#endif

void VideoCommAudio::prepStim(ParamTree /*const*/ *ptree,
			      CCDAvoid const &ccdavoid,
			      AnalogData *,
			      DigitalData *ddata) const {
  if (!ddata)
    throw Exception("VideoCommAudio","No digital buffer defined",
		    "prepStim");

  Enumerator *lines = Enumerator::find("DIGILINES");
  int trigline = lines->lookup("VideoTrig");
  int lightline = lines->lookup("VideoLight");
  ddata->addLine(trigline);
  ddata->addLine(lightline);
  uint32_t one = 1;
  uint32_t trigval = one<<trigline;
  uint32_t lightval = one<<lightline;
  uint32_t *data = ddata->allData();
  int nscans = ddata->getNumScans(); // we copy scan count from ddata

  double marginpre_ms = ptree->find("stimVideo/marginPreCCD").toDouble();
  double marginpost_ms = ptree->find("stimVideo/marginPostCCD").toDouble();
  
  double start_ms = ptree->find("stimVideo/delay").toDouble(); // start of vstim
  double outrate_hz = ptree->find("stimEphys/outrate").toDouble();
  int start_scans = int(start_ms * outrate_hz/1000);
  if (start_scans>nscans)
    start_scans = nscans;
  int blipstart_scans = start_scans - int(VIDEO_STARTBLIPDUR_MS*outrate_hz/1000);
  if (blipstart_scans<0)
    blipstart_scans = 0;
  for (int n=blipstart_scans; n<start_scans; n++)
    data[n] |= trigval;

  double end_ms = start_ms + ptree->find("stimVideo/dur").toDouble(); // end of vstim
  int end_scans = int(end_ms * outrate_hz/1000);
  if (end_scans>nscans)
    end_scans = nscans;
  blipstart_scans = end_scans - int(VIDEO_ENDBLIPDUR_MS*outrate_hz/1000);
  if (blipstart_scans<0)
    blipstart_scans = 0;
  for (int k=0; k<VIDEO_ENDBLIPCOUNT; k++) {
    int n0 = blipstart_scans+int(k*VIDEO_ENDBLIPPERIOD_MS/1000);
    int n1 = n0 + int(VIDEO_ENDBLIPDUR_MS*outrate_hz/1000);
    if (n1>nscans)
      break;
    for (int n=n0; n<n1; n++)
      data[n] |= trigval;
  }

  for (int n=start_scans; n<end_scans; n++)
    data[n] |= lightval;

  if (ptree->find("stimVideo/avoidCCD").toBool()) {
    dbg("avoidccd on");
    int s = ccdavoid.start_scans - int(marginpre_ms*outrate_hz/1000);
    int ds = ccdavoid.actv_scans + int(marginpre_ms*outrate_hz/1000) 
      + int(marginpost_ms*outrate_hz/1000);
    while (s>start_scans)
      s-=ccdavoid.ival_scans;
    while (s<0)
      s+=ccdavoid.ival_scans;
    while (s<end_scans) {
      int s1 = s+ds;
      if (s1>end_scans)
	s1=end_scans;
      int s2 = s+ccdavoid.ival_scans;
      while (s<s1)
	data[s++] &= ~lightval;
      s=s2;
    }    
  } else {
    dbg("avoidccd off");
  }
}

  
