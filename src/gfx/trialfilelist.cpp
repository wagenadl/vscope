// gfx/trialfilelist.cpp - This file is part of VScope.
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

// trialfilelist.cpp

#include "trialfilelist.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <base/dbg.h>

TrialFileList::TrialFileList(QWidget *parent): FileList(parent) {
}

bool TrialFileList::includeFileHook(QDir const &, QString fn) {
  return !fn.contains("-rois");
}

QString TrialFileList::infoHook(class QDir const &dir, QString fn) {
  static QMap<QString, QStringList> logfiles;
  static QMap<QString, QDateTime> lastread;
  QString logfn = dir.absoluteFilePath("log.txt");
  QFile logf(logfn);
  bool have = logfiles.contains(logfn);
  if (have && lastread[logfn].secsTo(QDateTime::currentDateTime()) > 10) {
    QDateTime mtime = QFileInfo(logf).lastModified();
    if (mtime > lastread[logfn])
      have = false;
  }
  if (!have) {
    logfiles[logfn] = QStringList();
    if (logf.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream txt(&logf);
      while (!txt.atEnd())
        logfiles[logfn] << txt.readLine();
      lastread[logfn] = QFileInfo(logf).lastModified();
      have = true;
    }
  }

  QString typ = "";
  if (have) {
    QString ourtrial = "Trial " + fn.replace(QString(".xml"),QString(""));
    foreach (QString line, logfiles[logfn]) {
      if (line.contains(ourtrial)) {
	if (line.contains("Snapshot"))
	  typ = "snapshot";
        else if (line.contains("continuous"))
          typ = "cont";
        else if (line.contains("E'phys"))
	  typ = "eph";
	else
	  typ = "";
	if (line.contains("vsd") || line.contains("VSD")) {
	  if (typ!="")
	    typ+="+";
	  typ+="vsd";
	}
	if (line.contains("stim")) {
	  typ+="+stim";
	}
      }
    }
  }
  return typ;
}
