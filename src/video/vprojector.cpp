// video/vprojector.cpp - This file is part of VScope.
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
    if (proc->waitForFinished(100)) {
      // this is a problem
      Dbg() << "  instantly quit";
      return false;
    } else {
      Dbg() << "  activated";
      return true;
    }
  } else {
    Dbg() << "  failed to activate";
    return false;
  }
}

bool VProjector::isOk() const {
  return proc && proc->state()==QProcess::Running;
}

static double angleFromString(QString s) {
  if (s=="HT")
    return 0;
  else if (s=="TH")
    return 180;
  else if (s=="RL")
    return 90;
  else if (s=="LR")
    return 270;
  else
    return s.toDouble();
}

void VProjector::prepare(class ParamTree const *p) {
  if (!isOk())
    return;
  Dbg() << "vprojector::setup";
  if (!p->find("stimVideo/enable").toBool())
    return;

  proc->write(QString("set type %1\n")
              .arg(p->find("stimVideo/type").toString()).toUtf8());
  proc->write(QString("set target %1\n")
              .arg(p->find("stimVideo/target").toString()).toUtf8());
  proc->write(QString("set angledeg %1\n")
              .arg(angleFromString(p->find("stimVideo/angle").toString()))
	      .toUtf8());
  proc->write(QString("set wavelengthmm %1\n")
              .arg(p->find("stimVideo/wavelength").toDouble()).toUtf8());
  proc->write(QString("set frequencyhz %1\n")
              .arg(p->find("stimVideo/frequency").toDouble()).toUtf8());
  double del = p->find("stimVideo/delay").toDouble();
  proc->write(QString("set delayms %1\n")
              .arg(del).toUtf8());
  double dur = p->find("stimVideo/dur").toDouble();
  double acqt = p->find("acqEphys/acqTime").toDouble();
  if (del+dur>acqt)
    dur = acqt-del;
  if (dur<0)
    dur = 0;
  proc->write(QString("set durms %1\n")
              .arg(dur).toUtf8());
  proc->write(QString("set xscalemm %1\n")
              .arg(p->find("stimVideo/xscale").toDouble()).toUtf8());
  proc->write(QString("set yscalemm %1\n")
              .arg(p->find("stimVideo/yscale").toDouble()).toUtf8());
  proc->write(QString("set xhomemm %1\n")
              .arg(p->find("stimVideo/xhome").toDouble()).toUtf8());
  proc->write(QString("set yhomemm %1\n")
              .arg(p->find("stimVideo/yhome").toDouble()).toUtf8());
  proc->waitForBytesWritten(); // hmmm
  Dbg() << "  vprojector prepared";
}

void VProjector::start() {
  if (!isOk())
    return;
  Dbg() << "vprojector::start";
  proc->write("start\n");
  proc->waitForBytesWritten(); // hmmm
}

void VProjector::readyReadStderr() {
  Dbg() << "VProjector::stderr";
  Dbg() << "  " << QString(proc->readAllStandardError());
}

void VProjector::readyReadStdout() {
  Dbg() << "VProjector::stdout";
  Dbg() << "  " << QString(proc->readAllStandardOutput());
}
