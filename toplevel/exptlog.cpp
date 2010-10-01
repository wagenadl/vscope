// exptlog.cpp

#include "exptlog.h"
#include <gfx/lineedit.h>
#include <QFile>
#include <QTextStream>
#include <toplevel/globals.h>
#include <xml/paramtree.h>
#include <gfx/textentry.h>
#include <QDateTime>
#include <QDir>
#include <base/exception.h>
#include <acq/datatrove.h>

ExptLog::ExptLog(QObject *parent): QObject(parent) {
  noteEditor = 0;
  outputFile = 0;
  outputStream = 0;
  suppressFurtherROI = false;
  suppressFurtherSetting = false;
  addNote("VSDScope starting");
  addNote("New experiment: "
	  + Globals::ptree->find("acquisition/_exptname").toString());
}

ExptLog::~ExptLog() {
  addNote("VSDScope closing");
}

void ExptLog::prepareUserNote() {
  if (!noteEditor) {
    noteEditor = new TextEntry(Globals::mainwindow);
    noteEditor->setMessage("Note for experiment log");
    connect(noteEditor,SIGNAL(accept(QString)),
	    this,SLOT(acceptUserNote(QString)));
    connect(noteEditor,SIGNAL(cancel()),
	    this,SLOT(cancelUserNote()));
  }
  noteEditor->setText("");
  noteEditor->show();
}

void ExptLog::acceptUserNote(QString s) {
  activate();
  addNote("Note: " + s);
  noteEditor->setText("");
}

void ExptLog::cancelUserNote() {
  noteEditor->hide();
}

void ExptLog::activate() {
  if (outputStream)
    return;
  QString exptname = Globals::ptree->find("acquisition/_exptname").toString();
  QString basedir = Globals::ptree->find("_filePath").toString();
  QDir d; d.mkpath(basedir + "/" + exptname);
  outputFile = new QFile(basedir + "/" + exptname + "/log.txt");
  bool preexist = outputFile->exists();
  if (!outputFile->open(QFile::WriteOnly | QFile::Append))
    throw Exception("ExptLog","Cannot write log file: "
		    + outputFile->fileName());
  outputStream = new QTextStream(outputFile);
  if (preexist)
    (*outputStream) << endl;
  for (QStringList::iterator i=backLog.begin(); i!=backLog.end(); ++i)
    (*outputStream) << (*i) << endl;
  backLog.clear();
}  
    

void ExptLog::addNote(QString s, bool nodate) {
  s.replace("\n"," ");
  QDateTime now = QDateTime::currentDateTime();
  QString nowdate = now.toString("MM/dd/yy");
  QString nowhr = now.toString(" hh");
  QString nowmin = now.toString(":mm");
  QString nowsec = now.toString(":ss");
  QString nowrep = nowdate==olddate ? "        " : nowdate;
  if (nowhr!=oldhr || nowmin!=oldmin) {
    nowrep += nowhr+nowmin+nowsec;
  } else {
    nowrep += "   ";
    if (nowmin!=oldmin) {
      nowrep += nowmin+nowsec;
    } else {
      nowrep += "   ";
      if (nowsec!=oldsec) {
	nowrep += nowsec;
      } else {
	nowrep += "   ";
      }
    }
  }
  olddate = nowdate;
  oldhr = nowhr;
  oldmin = nowmin;
  oldsec = nowsec;
  if (nodate)
    nowrep = "                 ";
  QString note = nowrep + " " + s;
  if (outputStream) {
    (*outputStream) << note << endl;
    outputStream->flush();
  } else {
    backLog.push_back(note);
  }
}

void ExptLog::newExptName() {
  if (outputStream) {
    addNote("Experiment ended");
    delete outputStream;
    outputStream = 0;
    delete outputFile;
    outputFile = 0;
  }
  addNote("New experiment: "
	  + Globals::ptree->find("acquisition/_exptname").toString());
}

void ExptLog::markTrial(bool snap) {
  activate();
  int trialno = Globals::ptree->find("acquisition/_trialno").toInt();
  QString typ;
  if (snap) {
    typ = "Snapshot";
  } else {
    bool hasvsd = Globals::ptree->find("acqCCD/enable").toBool();
    bool hasstim = Globals::ptree->find("stimEphys/enable").toBool();
    if (hasvsd)
      typ = "E'phys. + vsd";
    else
      typ = "E'phys.";
    if (hasstim)
      typ += " with e'phys. stimuli";
  }
  addNote(QString("Trial %1: %2").arg(trialno,3,10,QChar('0')).arg(typ));
  suppressFurtherROI = false;
  suppressFurtherSetting = false;
}

void ExptLog::markContEphys(bool start_not_stop) {
  if (start_not_stop) {
    int trialno = Globals::ptree->find("acquisition/_trialno").toInt();
    addNote(QString("Start of continuous e'phys., recording as Trial %1")
	    .arg(trialno,3,10,QChar('0')));
  } else {
    addNote("End of continuous e'phys.");
  }
}
  
void ExptLog::changeSetting(QString label) {
  if (!suppressFurtherSetting)
    addNote("Parameter change");
  if (!label.isEmpty())
    addNote("  " + label,true);
  suppressFurtherSetting = true;
}

void ExptLog::changeROI() {
  if (suppressFurtherROI)
    return;
  addNote("ROIs changed");
  suppressFurtherROI = true;
}

void ExptLog::markLoadSettings(QString s) {
  addNote("Settings loaded: " + s);
  suppressFurtherSetting = false;
}
