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
#include <base/dbg.h>
#include "vscopegui.h"
#include <xml/enumerator.h>

ExptLog::ExptLog(QObject *parent): QObject(parent) {
  noteEditor = 0;
  outputFile = 0;
  outputStream = 0;
  suppressFurtherROI = false;
  //  addNote("VScope starting");
  addNote("New experiment: "
	  + Globals::ptree->find("acquisition/_exptname").toString());
  makeConnections();
}

ExptLog::~ExptLog() {
  writeSettingsBacklog();
  addNote("VScope closing");
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
  if (!noteEditor->ctrlHeld())
    noteEditor->hide();
}

void ExptLog::cancelUserNote() {
  noteEditor->hide();
}

void ExptLog::openFile() {
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
  if (preexist) {
    (*outputStream) << endl;
    //    addNote("Appending to existing log");
  }
}

void ExptLog::writeBacklog() {
  if (backlog.isEmpty() || !outputStream)
    return;

  for (QStringList::iterator i=backlog.begin(); i!=backlog.end(); ++i)
    (*outputStream) << (*i) << endl;
  backlog.clear();
}

void ExptLog::writeSettingsBacklog() {
  if (settingsBacklog.isEmpty() | !outputStream)
    return;

  if (settingsBacklog.contains("!")) {
    addNote("Settings loaded: " + settingsBacklog["!"]);
    settingsBacklog.remove("!");
    if (settingsBacklog.isEmpty())
      return;
  }

  addNote("Parameter change");
  for (QMap<QString,QString>::const_iterator i=settingsBacklog.begin();
       i!=settingsBacklog.end(); ++i)
    addNote("  " + i.key() + ": " + i.value(),true);
  settingsBacklog.clear();
}

void ExptLog::activate() {
  openFile();
  writeBacklog();
  writeSettingsBacklog();
}

void ExptLog::addNote(QString s, bool nodate) {
  s.replace("\n"," ");
  QDateTime now = QDateTime::currentDateTime();
  QString nowdate = now.toString("MM/dd/yy");
  QString nowtime = now.toString("hh:mm:ss");
  bool suppressDate = nodate || (nowdate==olddate);
  bool suppressTime = nodate || (nowdate==olddate && nowtime==oldtime
				&& !s.startsWith("Trial"));
  QString nowrep = suppressDate ? "        " : nowdate;
  nowrep += " ";
  nowrep += suppressTime ? "        " : nowtime;
  olddate = nowdate;
  oldtime = nowtime;
  QString note = nowrep + " " + s;
  if (outputStream) {
    (*outputStream) << note << endl;
    outputStream->flush();
  } else {
    backlog.push_back(note);
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
  backlog.clear();
  settingsBacklog.clear();
  olddate="";
  oldtime="";
  addNote("New experiment: "
	  + Globals::ptree->find("acquisition/_exptname").toString());
}

void ExptLog::markTrial(bool snap) {
  Dbg() <<"markTrial";
  activate();
  int trialno = Globals::ptree->find("acquisition/_trialno").toInt();
  QString typ;
  if (snap) {
    typ = "Snapshot";
  } else {
    bool hasvsd =  Globals::ptree->find("acqCCD/enable").toBool();
    bool hasstim = Globals::ptree->find("stimEphys/enable").toBool();
    if (hasstim) {
      hasstim = false;
      Enumerator const *e = Enumerator::find("STIMCHS");
      QStringList chs = e->getAllTags();
      foreach (QString s, chs) {
	if (Globals::ptree->find(QString("stimEphys/channel:%1/enable").arg(s))
	    .toBool()) {
	  hasstim = true;
	  break;
	}
      }
    }
    bool hasvid =  Globals::ptree->find("stimVideo/enable").toBool();
    if (hasvsd)
      typ = "E'phys. + vsd";
    else
      typ = "E'phys.";
    if (hasstim) {
      typ += " with e'phys.";
      if (hasvid)
	typ += " and video";
      typ += " stimuli";
    } else if (hasvid) {
      typ += " with video stimuli";
    }
  }
  addNote(QString("Trial %1: %2").arg(trialno,3,10,QChar('0')).arg(typ));
  suppressFurtherROI = false;
}

void ExptLog::markContEphys(bool start_not_stop) {
  activate();
  if (start_not_stop) {
    int trialno = Globals::ptree->find("acquisition/_trialno").toInt();
    addNote(QString("Start of continuous e'phys., recording as Trial %1")
	    .arg(trialno,3,10,QChar('0')));
  } else {
    addNote("End of continuous e'phys.");
  }
}
  
void ExptLog::changeSetting(QString label, QString value) {
  settingsBacklog[label]=value;
}

void ExptLog::changeROI() {
  if (suppressFurtherROI)
    return;
  addNote("ROIs changed");
  suppressFurtherROI = true;
}

void ExptLog::markLoadSettings(QString s) {
  settingsBacklog.clear();
  settingsBacklog["!"] = s;
}

void ExptLog::acceptDoubleClick(QString, QString txt) {
  txt.replace("\n"," ");
  addNote("Info: " + txt);
}

void ExptLog::makeConnections() {
  QStringList buts;
  buts.append("acquisition/_ccvals");
  buts.append("acquisition/_oxvals");
  foreach (QString s, buts) {
    connect(Globals::gui->findpButton(s),
	    SIGNAL(doubleClicked(QString,QString)),
	    this,
	    SLOT(acceptDoubleClick(QString,QString)));
  }
}
