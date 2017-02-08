// guiexc.cpp

#include "guiexc.h"
#include <base/dbg.h>
#include <QMessageBox>
#include <QApplication>
#include <QPushButton>
#include <stdlib.h>
#ifdef vsdLINUX
#include <unistd.h>
#endif
#include <xml/settingsfile.h>

QString GUIExc::settingsdir;
int GUIExc::argc=0;
char **GUIExc::argv=0;
ParamTree const *GUIExc::ptree=0;

void GUIExc::setSettingsDir(QString path) {
  settingsdir = path;
}

void GUIExc::setArgs(int argc_, char **argv_) {
  argc = argc_;
  argv = argv_;
}

void GUIExc::setParamTree(ParamTree const *p) {
  ptree = p;
}

void GUIExc::saveSettings() {
  if (ptree)
    SettingsFile::save(settingsdir + "/crash.xml", ptree);
}
			 

void GUIExc::report(class Exception const &e, QString reporter) throw() {
  try {
    if (reporter=="")
      Dbg() << "Exception caught.";
    else
      Dbg() << "Exception caught by " << reporter;
  
    saveSettings();

    QString txt = "Exception";
    if (!reporter.isEmpty())
      txt += "(reported by " + reporter +")";
    txt += ": " + QString(e) + ". ";
    txt += "Your settings will be saved as 'crash'. ";
    if (reporter.isEmpty())
      txt += "VScope must exit.";
    else
      txt += "It may be best to restart VScope.";
      
    QMessageBox msg(QMessageBox::Critical, "VScope Exception", txt);
    QAbstractButton *b_quit =
      msg.addButton("Quit",QMessageBox::DestructiveRole);
#ifdef vsdLINUX
    QAbstractButton *b_restart = argv 
      ? msg.addButton("Restart",QMessageBox::DestructiveRole)
      : 0;
#endif
    QAbstractButton *b_ignore = reporter.isEmpty()
      ? 0
      : msg.addButton("Ignore",QMessageBox::RejectRole);
    msg.exec();
    saveSettings();
    QAbstractButton *clicked = msg.clickedButton();
    if (clicked == b_quit) {
      Dbg() << "exiting";
      ::exit(0);
      Dbg() << "exited";
#ifdef vsdLINUX
    } else if (clicked == b_restart) {
      ::execvp(argv[0],argv);
      ::exit(1); // this should never execute
#endif
    } else if (clicked == b_ignore) {
      Dbg() << "ignored";
    } else {
      Dbg() << "GUIExc got unknown button.";
    }
  } catch (...) {
    fprintf(stderr,"Exception in GUIExc. Instantly aborting.\n");
    fflush(stderr);
    ::exit(1);
  }      
}

