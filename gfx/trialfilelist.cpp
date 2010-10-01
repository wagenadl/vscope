// trialfilelist.cpp

#include "trialfilelist.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <base/dbg.h>

TrialFileList::TrialFileList(QWidget *parent): FileList(parent) {
  dbg("trialfilelist");
}

bool TrialFileList::includeFileHook(QDir const &, QString fn) {
  dbg("trialfilelist:include(%s)",qPrintable(fn));
  return !fn.contains("-rois");
}

QString TrialFileList::infoHook(class QDir const &dir, QString fn) {
  dbg("trialfilelist:info(%s)",qPrintable(fn));
  QFile logf(dir.absoluteFilePath("log.txt"));
  if (logf.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream txt(&logf);
    QString ourtrial = "Trial " + fn.replace(QString(".xml"),QString("")) + ":";
    while (!txt.atEnd()) {
      QString line = txt.readLine();
      if (line.contains(ourtrial)) {
	if (line.contains("Snapshot"))
	  return "snapshot";
	QString typ="";
	if (line.contains("E'phys"))
	  typ="eph";
	if (line.contains("vsd")) {
	  if (typ!="")
	    typ+="+";
	  typ+="vsd";
	}
	if (line.contains("stim")) {
	  typ+="+stim";
	}
	return typ;
      }
    }
  }
  return "";
}
