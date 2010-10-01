// exptdirlist.cpp

#include "exptdirlist.h"
#include <QDir>
#include <QTextStream>
#include <QRegExp>
#include <base/dbg.h>

ExptDirList::ExptDirList(QWidget *parent): FileList(parent) {
  dbg("exptdirlist");
}

QString ExptDirList::infoHook(QDir const &dir, QString fn) {
  dbg("exptdirlist:info for %s",qPrintable(fn));
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
	return QString("%1 (+%2 snap.)")
	  .arg(ntrials_nonsnap)
	  .arg(ntrials_snap);
    }
  }
  return QString();
}
