// toplevel/savedsettings.cpp - This file is part of VScope.
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

// savedsettings.cpp

#include "savedsettings.h"
#include <base/dbg.h>
#include <toplevel/globals.h>
#include <base/exception.h>
#include <toplevel/vscopegui.h>
#include <gui/guipage.h>
#include <xml/paramtree.h>
#include <base/xml.h>
#include <xml/connections.h>
#include <gfx/filedlgkey.h>
#include <toplevel/mgauto.h>
#include <toplevel/exptlog.h>
#include <QDateTime>
#include <acq/datatrove.h>
#include <toplevel/gt_slots.h>
#include <gui/guiexc.h>
#include <xml/settingsfile.h>
#include <toplevel/mainwindow.h>
#include <acq/trialdata.h>

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
  FileChooser::showEvent(e);

  QWidget *p1 = Globals::gui->findpButton("savedSettings/_a");
  QWidget *p2 = Globals::gui->findpButton("savedSettings/_b");
  if (!p1 || !p2) 
    throw Exception("SavedSettings","Cannot find savedSettings _a and _b buttons",
		    "showEvent");
  setParent(p1->parentWidget());
  QRect r(p1->geometry());
  r |= p2->geometry();
  setGeometry(r);
  populateFiles(QDir(Globals::filePath()+"/_settings"),"xml");

  p1->hide();
  p2->hide();
  show();
}

void SavedSettings::loadSettings(QString fn) {
  QString bfn = fn;
  if (!fn.startsWith("/"))
    fn = Globals::filePath() + "/_settings/" + fn;
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
    visualselect(bfn);
  } catch (Exception const &e) {
    GUIExc::report(e,"SavedSettings::loadSettings");
  }
}

void SavedSettings::anythingChanged() {
  //  Globals::ptree->find("savedSettings/name").set("Modified");
  visualdeselect();
}

void SavedSettings::visualselect(QString fn) {
  QStringList bits = fn.split("/");
  QString bfn = bits.takeLast();
  if (bfn.endsWith(".xml"))
    bfn=bfn.left(bfn.length()-4);
  visualdeselect();
  Button *b = buttonp(bfn);
  if (b) 
    b->setSelected(true);
  lastsel = bfn;
}

void SavedSettings::visualdeselect() {
  if (lastsel=="")
    return;
  
  Button *b = buttonp(lastsel);
  if (b) 
    b->setSelected(false);
  lastsel = "";
}

void SavedSettings::deleteCurrent() {
  if (lastsel=="")
    return;
  
  QString fn = lastsel;
  if (!fn.startsWith("/"))
    fn = Globals::filePath() + "/_settings/" + fn;
  if (!fn.endsWith(".xml"))
    fn += ".xml";

  QFile f(fn);
  f.remove();
  
  visualdeselect();
  hide();
  show();
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
  
  savedlg->goDir(Globals::filePath() + "/_settings");
  savedlg->show();
  saveframe->show();
}

void SavedSettings::saveSettings(QString fn) {
  if (!fn.endsWith(".xml"))
    fn+=".xml";

  saveframe->hide();
  try {
    ParamTree *ptree = Globals::trove->trial().myParamTree();
    if (!ptree)
      ptree = Globals::ptree;
    SettingsFile::save(fn, ptree);
  } catch (Exception const &) {
    dbg("SavedSettings::saveSettings: Caught exception");
  }
  if (isVisible()) {
    populateFiles(QDir(Globals::filePath() + "/_settings"),"xml");
    visualselect(fn);
  }
}

