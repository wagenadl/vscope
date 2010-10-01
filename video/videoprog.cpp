// videoprog.cpp

#include "videoprog.h"
#include <xml/paramtree.h>
#include <base/dbg.h>
#include <base/exception.h>
#include <video/videocomm.h>

QStringList emptylist;

VideoProg::VideoProg() {
  hasprognames = false;
  isuptodate = false;
  try {
    vidcom = new VideoComm();
  } catch (Exception &e) {
    Dbg() << "Exception caught by VideoProg. VideoProg disabled.";
    e.report();
    vidcom = 0;
  }
}

VideoProg::~VideoProg() {
  if (vidcom)
    delete vidcom;
}

bool VideoProg::enabled(ParamTree *ptree) const {
  if (!vidcom)
    return false;
  return ptree->find("stimVideo/enable").toBool();
}

void VideoProg::reset(ParamTree *ptree) {
  if (enabled(ptree)) {
    isuptodate = false;
    ensureSettings(ptree);
  }
}

void VideoProg::setProgram(ParamTree *ptree, bool forreal) {
  QString prog = ptree->find("stimVideo/prog").toString();
  Dbg() <<"VideoProg::setProgram: " << prog;
  int progno = progname2num(prog);
  if (progno<=0)
    throw Exception("VideoProg","Unknown program name");
  if (forreal) 
    vidcom->setProgram(progno);
  else
    isuptodate = false;
  dbg("  VideoProg::setProgram done");
}

void VideoProg::setParam(ParamTree *ptree,
			 int parno, bool forreal) {
  dbg("VideoProg::setParam");
  double val = ptree->find(QString("stimVideo/par%1")
			   .arg(QString('A'+parno-1))).toDouble();
  if (forreal)
    vidcom->setPar(parno, val);
  else
    isuptodate = false;
  dbg("  VideoProg::setParam done");
}

void VideoProg::ensureProgNames() {
  if (hasprognames)
    return;

  prognames = vidcom->getAllProgNames();
  prognos.clear();
  for (QMap<int, QString>::iterator i=prognames.begin();
       i!=prognames.end(); ++i) {
    int progno = i.key();
    QString progname = i.value();
    prognos[progname] = progno;
    dbg("Note: I named program #%i '%s'.",
	progno,qPrintable(progname));
  }
  hasprognames = true;
}

void VideoProg::ensureParNames(int prog) {
  if (!parnames4prog.contains(prog)) 
    parnames4prog[prog] = vidcom->getParNames(prog);
  if (!parunits4prog.contains(prog)) 
    parunits4prog[prog] = vidcom->getParUnits(prog);
}


void VideoProg::ensureParValues(int prog, int par) {
  if (!parvalues4prog[prog].contains(par)) 
    parvalues4prog[prog][par] = vidcom->getParValues(prog,par);
}

void VideoProg::ensureSettings(ParamTree *ptree) {
  if (isuptodate)
    return;

  dbg("VideoProg::ensureSettings");
  ensureProgNames();
  setProgram(ptree,true);
  for (int k=1; k<=MAXPAR; k++)
    setParam(ptree,k,true);
  dbg("  VideoProg::ensureSettings done");

  isuptodate = true;
}

int VideoProg::progname2num(QString name) {
  ensureProgNames();
  if (prognos.contains(name))
    return prognos.value(name);
  else
    return -1;
}

QString VideoProg::prognum2name(int num) {
  ensureProgNames();
  return prognames.value(num);
}


void VideoProg::prepStim(class ParamTree /*const*/ *ptree,
			 class CCDAvoid const &ccdavoid,
			 class AnalogData *adata,
			 class DigitalData *ddata) const {
  vidcom->prepStim(ptree, ccdavoid, adata, ddata);
}

QMap<int,QString> const &VideoProg::progNames() {
  ensureProgNames();
  return prognames;
}

QStringList const &VideoProg::parNames(int prog) {
  ensureParNames(prog);
  QMap<int,QStringList>::const_iterator i = parnames4prog.find(prog);
  if (i!=parnames4prog.end())
    return i.value();
  return emptylist;
}

QStringList const &VideoProg::parUnits(int prog) {
  ensureParNames(prog);
  QMap<int,QStringList>::const_iterator i = parunits4prog.find(prog);
  if (i!=parunits4prog.end())
    return i.value();
  return emptylist;
}

QStringList const &VideoProg::parValues(int prog, int par) {
  ensureParValues(prog, par);
  QMap<int, QMap<int,QStringList> >::const_iterator i
    = parvalues4prog.find(prog);
  if (i!=parvalues4prog.end()) {
    QMap<int,QStringList>::const_iterator j = i.value().find(par);
    if (j!=i.value().end())
      return j.value();
  }
  return emptylist;
}

VideoProg *VideoProg::deflt = 0;

VideoProg &VideoProg::find() {
  if (!deflt)
    deflt = new VideoProg;
  return *deflt;
}
