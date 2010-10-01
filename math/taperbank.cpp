// taperbank.cpp

#include "taperbank.h"
#include <QFile>
#include <QIODevice>
#include <base/exception.h>
#include <base/dbg.h>

Taperbank::Taperbank(QString dir): dir(dir) {
  Dbg() << "Taperbank: path = " << dir;
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

  QString fn = buildfn(id);
  if (!QFile::exists(fn)) 
    throw Exception("Taperbank",
		    QString("The tapers %1 do not exist. Please run\n\n  preptaper('%2',%3,%4);\n\nin matlab.")
		    .arg(id.name())
		    .arg(id.N)
		    .arg(id.nw,0,'f',2)
		    .arg(dir));

  // let's load it
  FILE *fd = fopen(qPrintable(fn),"rb");
  if (!fd)
    throw SysExc("Taperbank","Cannot open tapers");

  Tapers &tap = data[id];
  tap.rebuild(id.N,id.K);
  for (int k=0; k<id.K; k++) {
    double *vec = tap.tapers[k].data();
    if (fread(vec,sizeof(double),id.N,fd)!=size_t(id.N))
      throw SysExc("Taperbank","Cannot read tapers");
  }
  fclose(fd);
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

Taperbank *Taperbank::deflt=0;

void Taperbank::initialize(QString dir) {
  if (deflt)
    delete deflt;
  deflt=0;
  deflt = new Taperbank(dir);
}

bool Taperbank::hasTaper(TaperID const &id) {
  if (deflt)
    return deflt->contains(id);
  else
    return false;
}

Tapers const &Taperbank::tapers(TaperID const &id) {
  if (deflt)
    return deflt->find(id);
  else
    throw Exception("Taperbank","Not initialized");
}
