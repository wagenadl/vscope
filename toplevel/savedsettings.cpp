// savedsettings.cpp

#include "savedsettings.h"
#include <base/dbg.h>
#include <toplevel/globals.h>
#include <base/exception.h>
#include <toplevel/vscopegui.h>
#include <gui/xmlpage.h>
#include <xml/paramtree.h>
#include <base/xml.h>
#include <xml/connections.h>
#include <gfx/filedlgkey.h>
#include <toplevel/mgauto.h>
#include <toplevel/exptlog.h>
#include <QDateTime>
#include <acq/datatrove.h>
#include <gui/videogui.h>
#include <toplevel/gt_slots.h>
#include <gui/guiexc.h>
#include <xml/settingsfile.h>

SavedSettings::SavedSettings(QWidget *parent): FileChooser(parent) {
  savedlg = 0;
  saveframe = 0;
  setBackground("#ffccff");
  hide();
  connect(this,SIGNAL(clickedItem(QString)),
	  this,SLOT(loadSettings(QString)));
}

SavedSettings::~SavedSettings() {
}

void SavedSettings::showEvent(class QShowEvent *e) {
  dbg("SavedSettings::showEvent");
  FileChooser::showEvent(e);

  QWidget *p = Globals::gui->findpPage("savedSettings/_name");
  if (!p)
    throw Exception("SavedSettings","Cannot find savedSettings/_name page",
		    "showEvent");
  setParent(p->parentWidget());
  QRect r(p->geometry());
  r.adjust(2,2,-2,-2);
  setGeometry(r);
  p->hide();
  QPoint tl = r.topLeft();
  QPoint br = r.bottomRight();
  // QRect r1(p->mapTo(parentWidget(),tl), p->mapTo(parentWidget(),br));
  dbg("  r.tl=(%i,%i) .br=(%i,%i)", // r1.tl=(%i,%i) .br=(%i,%i)",
      tl.x(),tl.y(),br.x(),br.y());
  //     r1.left(),r1.top(), r1.right(),r1.bottom());
  // setGeometry(r1);
  populateFiles(QDir(Globals::ptree->find("_filePath").toString()
		     +"/_settings"),"xml");
  show();
}

void SavedSettings::loadSettings(QString fn) {
  dbg("loadSettings %s",qPrintable(fn));
  if (!fn.startsWith("/"))
    fn = Globals::ptree->find("_filePath").toString()
      +"/_settings/" + fn;
  if (!fn.endsWith(".xml"))
    fn += ".xml";
  try {
    SettingsFile::load(fn, Globals::ptree);
    Globals::gtslots->everythingChanged();
    QString setname = fn;
    int lastslash = setname.lastIndexOf("/");
    if (lastslash>=0)
      setname = setname.mid(lastslash+1);
    setname = setname.left(setname.length()-4); // drop the ".xml";
    Globals::exptlog->markLoadSettings(setname);
  } catch (Exception const &e) {
    GUIExc::report(e,"SavedSettings::loadSettings");
  }
}

void SavedSettings::prepareSave() {
  if (!savedlg) {
    saveframe = new QFrame(Globals::mainwindow);
    saveframe->setGeometry(50,30,924,700);
    saveframe->setFrameStyle(QFrame::Raised|QFrame::Panel);
    saveframe->setLineWidth(2);
    saveframe->setAutoFillBackground(true);
    QPalette p = saveframe->palette();
    p.setColor(QPalette::Window,"#eedddd");
    saveframe->setPalette(p);
    
    savedlg = new FileDlgKey(saveframe);
    QRect r(saveframe->contentsRect());
    r.adjust(2,2,-2,-2);
    savedlg->setGeometry(r);
    savedlg->setLabel("Save settings...");
    savedlg->setExtn("xml");
    connect(savedlg,SIGNAL(chosenFile(QString)),
	    this,SLOT(saveSettings(QString)));
    connect(savedlg,SIGNAL(canceled()),
	    saveframe,SLOT(hide()));
  }
  
  savedlg->goDir(Globals::ptree->find("_filePath").toString()
		 +"/_settings");
  savedlg->show();
  saveframe->show();
}

void SavedSettings::saveSettings(QString fn) {
  if (!fn.endsWith(".xml"))
    fn+=".xml";

  saveframe->hide();
  try {
    SettingsFile::save(fn, Globals::ptree);
  } catch (Exception const &e) {
    dbg("SavedSettings::saveSettings: Caught exception");
  }
  if (isVisible())
    populateFiles(QDir(Globals::ptree->find("_filePath").toString()
		       +"/_settings"),"xml");
}

