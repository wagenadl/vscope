// taperbank.cpp

#include "taperbank.h"
#include <QFile>
#include <QIODevice>
#include <QProcess>
#include <QSignalMapper>
#include <base/exception.h>
#include <base/dbg.h>
#include <QDir>

Taperbank::Taperbank(QString dir): dir(dir) {
  Dbg() << "Taperbank: path = " << dir;
  mapper = new QSignalMapper(this);
  connect(mapper, SIGNAL(mapped(QString)), SLOT(processDone(QString)));
}

Taperbank::~Taperbank() {
}

QString Taperbank::buildfn(TaperID const &id) const {
  return QString("%1/%2.taper").arg(dir).arg(id.name());
}

bool Taperbank::contains(TaperID const &id) {
  if (data.contains(id)) 
    return true;
  QString fn = buildfn(id);
  Dbg() << "Taperbank looking for " << fn;
  return QFile::exists(fn);
}

Tapers const &Taperbank::find(TaperID const &id) {
  if (data.contains(id))
    return data[id];

  if (id.K<=0)
    throw Exception("Taperbank", "Cannot build taper " + id.name() + ": K=0");

  QString fn = buildfn(id);
  if (!QFile::exists(fn))
    instaPrep(id);
  if (!QFile::exists(fn))
    throw Exception("Taperbank",
		    QString("The tapers %1 do not exist and could not be created.\nPlease run\n\n  preptaper('%2',%3,%4);\n\nin Octave or Matlab.")
		    .arg(id.name())
		    .arg(id.N)
		    .arg(id.nw,0, 'f', 6)
		    .arg(dir));

  // let's load it
  QFile fd(fn);
  if (!fd.open(QFile::ReadOnly))
    throw Exception("Taperbank", "Cannot open tapers: " + fn);

  Tapers &tap = data[id];
  tap.rebuild(id.N,id.K);
  for (int k=0; k<id.K; k++) {
    double *vec = tap.tapers[k].data();
    int nbytes = sizeof(double)*id.N;
    if (fd.read((char*)vec, nbytes) != nbytes)
      throw Exception("Taperbank","Cannot read tapers");
  }
  return tap;
}

void Taperbank::forget(TaperID const &id) {
  if (data.contains(id)) {
    data.remove(id);
  }
}

void Taperbank::destroy(TaperID const &id) {
  forget(id);
  QString fn = buildfn(id);
  if (QFile::exists(fn))
    QFile::remove(fn);
}

void Taperbank::instaPrep(TaperID const &id) {
  QString fn = buildfn(id);
  QDir d(dir);
  if (!d.mkpath(d.absolutePath()))
    throw Exception("Taperbank", "Cannot make taperbank dir");
  Dbg() << "Taperbank::instaPrep: " << id.name() << ": " << fn;
  QProcess mkdpss(this);
  QStringList args;
  args.append(fn);
  args.append(QString::number(id.N));
  args.append(QString::number(id.nw));
  args.append(QString::number(id.K));
  mkdpss.start("mkdpss", args);
  if (!mkdpss.waitForStarted() || !mkdpss.waitForFinished())
    throw Exception("Taperbank", "Cannot run mkdpss");
  Dbg() << "Taperbank::instaPrep done";
}

bool Taperbank::couldExist(TaperID const &id) {
  return id.K>0;
}

bool Taperbank::canProvide(TaperID const &id, bool guarantee) {
  Dbg() << "Taperbank::canProvide: " << id.name() << " K="<<id.K;
  if (contains(id))
    return true;

  if (id.K<=0)
    return false;
  
  if (guarantee) {
    try {
      find(id);
    } catch (Exception const &e) {
      return false;
    }
    return true;
  } else {
    QProcess mkdpss(this);
    QStringList args;
    args.append("--help");
    mkdpss.start("mkdpss", args);
    bool ok = mkdpss.waitForStarted() && mkdpss.waitForFinished();
    return ok;
  }
}

void Taperbank::prepare(TaperID const &id) {
  if (preps.contains(id))
    return; // already running

  QProcess *p = new QProcess(this);
  preps[id] = p;
  prepids[id.name()] = id;
  connect(p, SIGNAL(finished(int)), mapper, SLOT(map()));
  mapper->setMapping(p, id.name());
  
  QStringList args;
  args.append(QString::number(id.N));
  args.append(QString::number(id.nw));
  args.append(QString::number(id.K));
  args.append(buildfn(id));
  p->start("mkdpss", args);
  if (!p->waitForStarted()) {
    delete p;
    preps.remove(id);
    prepids.remove(id.name());
    throw Exception("Taperbank", "Cannot run mkdpss");
  }
}

void Taperbank::processDone(QString name) {
  if (!prepids.contains(name)) {
    Dbg() << "Taperbank: unknown process completed. Ignored.";
    return;
  }
  
  TaperID id = prepids[name];
  QProcess *p = preps[id];
  int rc = p->exitCode();
  delete p;
  preps.remove(id);
  prepids.remove(name);
  Dbg() << "Taperbank: emitting prepared " << name
	<< " (" << rc << ") " << (rc==0);
  emit prepared(id, rc==0);
}
  
//////////////////////////////////////////////////////////////////////
Taperbank *Taperbank::deflt=0;

void Taperbank::initialize(QString dir) {
  if (deflt)
    delete deflt;
  deflt=0;
  deflt = new Taperbank(dir);
}

Taperbank &Taperbank::bank() {
  if (deflt)
    return *deflt;
  throw Exception("Taperbank", "Not initialized");
}

