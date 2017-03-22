// gfx/exptdirlist.cpp - This file is part of VScope.
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

// exptdirlist.cpp

#include "exptdirlist.h"
#include <QDir>
#include <QTextStream>
#include <QRegExp>
#include <base/dbg.h>

ExptDirList::ExptDirList(QWidget *parent): FileList(parent) {
}

QString ExptDirList::infoHook(QDir const &dir, QString fn) {
  QDir sub(dir.absoluteFilePath(fn));
  if (sub.exists()) {
    // This means "fn" actually is a directory
    QFile logf(sub.absoluteFilePath("log.txt"));
    if (logf.open(QIODevice::ReadOnly | QIODevice::Text)) {
      // This means the "log.txt" file is readable
      int ntrials_tot     = 0;
      int ntrials_snap    = 0;
      int ntrials_nonsnap = 0;
      int ntrials_vsd     = 0;
      int ntrials_ephys   = 0;
      int ntrials_stim    = 0;
      QTextStream txt(&logf);
      QRegExp regexp(" Trial (\\d+): (.*)");
      while (!txt.atEnd()) {
	QString line = txt.readLine();
	if (regexp.indexIn(line)>=0) {
	  // got a real trial
	  ntrials_tot++;
	  QString typ = regexp.cap(2);
	  if (typ.contains("Snapshot")) {
	    ntrials_snap++;
	  } else {
	    ntrials_nonsnap++;
	    if (typ.contains("E'phys"))
	      ntrials_ephys++;
	    if (typ.contains("vsd"))
	      ntrials_vsd++;
	    if (typ.contains("e'phys. stimuli"))
	      ntrials_stim++;
	  }
	}
      }
      if (ntrials_tot>0) 
	return QString("%1 (+%2 snap)")
	  .arg(ntrials_nonsnap)
	  .arg(ntrials_snap);
    }
  }
  return QString();
}
