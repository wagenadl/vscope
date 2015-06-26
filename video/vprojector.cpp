// vprojector.cpp

#include "vprojector.h"
#include <xml/paramtree.h>
#include <base/dbg.h>
#include <QProcess>

VProjector::VProjector(QObject *parent): QObject(parent) {
  Dbg() << "vprojector";
  proc = 0;
}

VProjector::~VProjector() {
  Dbg() << "~vprojector";
  if (proc) {
    proc->terminate();
    proc->waitForFinished(1000);
  }
}

bool VProjector::activate() {
  Dbg() << "vprojector::activate";
  if (isOk())
    return true;
  proc = new QProcess(this);
  connect(proc, SIGNAL(readyReadStandardOutput()), SLOT(readyReadStdout()));
  connect(proc, SIGNAL(readyReadStandardError()), SLOT(readyReadStderr()));
  QStringList args; args << "leechprojector" << "-server";
  proc->start("bash", args);
  if (proc->waitForStarted()) {
    Dbg() << "  activated";
    return true;
  } else {
    Dbg() << "  failed to activate";
    return false;
  }
}

bool VProjector::isOk() const {
  return proc && proc->state()==QProcess::Running;
}

void VProjector::prepare(class ParamTree const *p) {
  if (!isOk())
    return;
  Dbg() << "vprojector::setup";
  if (!p->find("stimVideo/enable").toBool())
    return;

  proc->write(QString("set type %1\n")
              .arg(p->find("stimVideo/type").toString()).toAscii());
  proc->write(QString("set angledeg %1\n")
              .arg(p->find("stimVideo/angle").toDouble()).toAscii());
  proc->write(QString("set wavelengthmm %1\n")
              .arg(p->find("stimVideo/wavelength").toDouble()).toAscii());
  proc->write(QString("set frequencyhz %1\n")
              .arg(p->find("stimVideo/frequency").toDouble()).toAscii());
  double del = p->find("stimVideo/delay").toDouble();
  proc->write(QString("set delayms %1\n")
              .arg(del).toAscii());
  double dur = p->find("stimVideo/dur").toDouble();
  double acqt = p->find("acqEphys/acqTime").toDouble();
  if (del+dur>acqt)
    dur = acqt-del;
  if (dur<0)
    dur = 0;
  proc->write(QString("set durms %1\n")
              .arg(dur).toAscii());
  proc->write(QString("set xscalemm %1\n")
              .arg(p->find("stimVideo/xscale").toDouble()).toAscii());
  proc->write(QString("set yscalemm %1\n")
              .arg(p->find("stimVideo/yscale").toDouble()).toAscii());
  proc->write(QString("set xhomemm %1\n")
              .arg(p->find("stimVideo/xhome").toDouble()).toAscii());
  proc->write(QString("set yhomemm %1\n")
              .arg(p->find("stimVideo/yhome").toDouble()).toAscii());
  proc->waitForBytesWritten(); // hmmm
  Dbg() << "  vprojector prepared";
}

void VProjector::start() {
  if (!isOk())
    return;
  Dbg() << "vprojector::start";
  proc->write("start\n");
}

void VProjector::readyReadStderr() {
  Dbg() << "VProjector::stderr";
  Dbg() << "  " << QString(proc->readAllStandardError());
}

void VProjector::readyReadStdout() {
  Dbg() << "VProjector::stdout";
  Dbg() << "  " << QString(proc->readAllStandardOutput());
}
