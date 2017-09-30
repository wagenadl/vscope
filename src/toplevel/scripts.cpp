// toplevel/scripts.cpp - This file is part of VScope.
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

// scripts.cpp

#include "scripts.h"
#include <gui/guiexc.h>
#include "globals.h"
#include <base/dbg.h>
#include <gfx/textedit.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QFile>
#include <QTimerEvent>

#include <xml/paramtree.h>
#include <toplevel/vscopegui.h>
#include <gui/guipage.h>
#include <gfx/filedlgkey.h>
#include <acq/trial.h>
#include <toplevel/acquire.h>
#include <toplevel/savedsettings.h>
#include <toplevel/gt_slots.h>
#include <toplevel/mainwindow.h>

Scripts::Scripts(QWidget *parent): QWidget(parent) {
  QSize s = parent ? parent->size() : QSize(500,400);
  move(0,0);
  resize(s.width()*4/5-4,s.height());
  QVBoxLayout *lay = new QVBoxLayout(this);
  QHBoxLayout *hlay = new QHBoxLayout;
  editor = new TextEdit(this);
  lay->addWidget(editor);
  statusLabel = new QLabel(this);
  hlay->addWidget(statusLabel);
  nameLabel = new QLabel(this);
  nameLabel->setAlignment(Qt::AlignRight);
  hlay->addWidget(nameLabel);
  statusLabel->setText("No script");
  lay->addLayout(hlay);

  loaddlg = 0;
  loadframe = 0;
  savedlg = 0;
  saveframe = 0;

  active = false;
  ival_ms = 1000;
  timer.setSingleShot(true);
  connect(&timer,SIGNAL(timeout()), this, SLOT(timeout()));
  connect(editor,SIGNAL(textChanged()), this, SLOT(changed()));
}

Scripts::~Scripts() {
}

void Scripts::prepareLoad() {
  Dbg() << "Scripts::prepareLoad";
  if (!loaddlg) {
    dbg("Scripts::prepareload: creating dialog");
    loadframe = new QFrame(Globals::mainwindow);
    loadframe->setGeometry(50,30,924,700);
    loadframe->setFrameStyle(QFrame::Raised|QFrame::Panel);
    loadframe->setLineWidth(2);
    loadframe->setAutoFillBackground(true);
    QPalette p = loadframe->palette();
    p.setColor(QPalette::Window,"#ddeeee");
    loadframe->setPalette(p);
    
    loaddlg = new FileDlgKey(loadframe, true);
    QRect r(loadframe->contentsRect());
    r.adjust(2,2,-2,-2);
    loaddlg->setGeometry(r);
    loaddlg->setLabel("Load script...");
    loaddlg->setExtn("txt");
    connect(loaddlg,SIGNAL(chosenFile(QString)),
	    this,SLOT(doLoad(QString)));
    connect(loaddlg,SIGNAL(changedDir(QString)),
	    this,SLOT(setDir(QString)));
    connect(loaddlg,SIGNAL(canceled()),
	    loadframe,SLOT(hide()));
  }
  if (true || lastdir.isEmpty()) {
    lastdir = Globals::filePath() + "/_scripts";
  }
  loaddlg->goDir(lastdir);
  loaddlg->show();
  loadframe->show();  
}

void Scripts::prepareSave() {
  Dbg() << "Scripts::prepareSave";
  if (!savedlg) {
    saveframe = new QFrame(Globals::mainwindow);
    saveframe->setGeometry(100,80,824,600);
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
    savedlg->setLabel("Save script...");
    savedlg->setExtn("txt");
    connect(savedlg,SIGNAL(chosenFile(QString)),
	    this,SLOT(doSave(QString)));
    connect(savedlg,SIGNAL(changedDir(QString)),
	    this,SLOT(setDir(QString)));
    connect(savedlg,SIGNAL(canceled()),
	    saveframe,SLOT(hide()));
  }

  if (true || lastdir.isEmpty()) {
    lastdir = Globals::filePath() + "/_scripts";
    QDir root(Globals::filePath());
    root.mkpath("_scripts");
    Dbg() << "mkpath _scripts";
  }
  savedlg->goDir(lastdir);
  savedlg->show();
  saveframe->show();
}

void Scripts::resave() {
  Dbg() << "Scripts::resave";
  if (name.isEmpty()) {
    Warning() << "Cannot save: no name set";
    return;
  }
  doSave(name);
}

void Scripts::check() {
  if (status()) {
  } else {
    // something
  }
}

void Scripts::setName(QString fn) {
  name = fn;
  QString leaf = name.split("/").last();
  QStringList bits = leaf.split("."); bits.takeLast();
  nameLabel->setText(bits.join("."));
}

void Scripts::doLoad(QString fn) {
  Dbg() << "Scripts::doLoad: " << fn;
  if (loadframe)
    loadframe->hide();
  setName(fn);
  QFile f(fn);
  if (!f.open(QIODevice::ReadOnly)) {
    Warning() << "Cannot read script '" + fn + "'";
    return;
  }
  QTextStream ts(&f);
  editor->setPlainText(ts.readAll());
  check();
}

void Scripts::doSave(QString fn) {
  Dbg() << "Scripts::doSave: " << fn;
  if (!fn.endsWith(".txt"))
    fn+=".txt";

  if (saveframe)
    saveframe->hide();
  setName(fn);
  QFile f(fn);
  if (!f.open(QIODevice::WriteOnly)) {
    Warning() << "Cannot save script '" + fn + "'";
    return;
  }
  QTextStream ts(&f);
  ts << editor->document()->toPlainText();
  check();
}

void Scripts::setDir(QString dir) {
  lastdir = dir;
}

void Scripts::setRunning(bool r) {
  Dbg() << "Scripts::setRunning " << r;
  if (r)
    run();
  else
    stop();
}

bool Scripts::status() {
  Dbg() << "Scripts::status";
  Script s;
  if (!s.setText(editor->document()->toPlainText())) {
    statusLabel->setText(s.firstError());
    return false;
  }
  if (s.contents().isEmpty()) {
    statusLabel->setText("No script");
    return true;
  }
  if (!s.ok(*Globals::ptree)) {
    statusLabel->setText(s.firstError());
    return false;
  }
  statusLabel->setText("Script OK");
  return true;
}

void Scripts::timeout() {
  if (Globals::trial->isActive()) 
    timer.start(1000); // wait one second to make sure previous trial ended
  else
    runSome();
}

void Scripts::run() {
  if (!status()) {
    Warning() << "Cannot run script with errors";
    Globals::ptree->find("scripts/_run").set("off");
    Globals::gui->findPage("scripts").open();
    return;
  }
  Dbg() << "Scripts::run";
  statusLabel->setText("Starting");
  active = true;
  script.setText(editor->document()->toPlainText());
  it = script.contents().begin();
  lineno = 0;
  loopno = 1;
  runSome();
}

void Scripts::runSome() {
  Dbg() << "Scripts::runSome active="<<active;
  if (!active) {
    stop();
    return;
  }

  while (it!=script.contents().end()) {
    Script::Command const &c(*it);
    ++it; ++lineno;
    statusLabel->setText(QString("Running; at line %1 (iteration %2)")
			 .arg(lineno)
			 .arg(loopno));
    switch (c.kwd) {
    case Script::KW_SET:
      Globals::ptree->find(c.arg1).set(c.arg2);
      Globals::gtslots->paramchanged(c.arg1,c.arg2);
      break;
    case Script::KW_TRIAL:
      Globals::acquire->acqTrial();
      if (it==script.contents().end())
	break; // end script and do not wait for trial to finish
      timer.start(ival_ms);
      return;
    case Script::KW_SNAP:
      Globals::acquire->acqFrame();
      if (it==script.contents().end())
	break; // end script and do not wait for trial to finish
      timer.start(ival_ms);
      return;
    case Script::KW_IVAL:
      ival_ms = c.argv;
      break;
    case Script::KW_LOADSETTINGS:
      Globals::savedSettings->loadSettings(c.arg1);
      break;
    case Script::KW_LOOP:
      // start from start, or N lines from start
      it=script.contents().begin();
      loopno++;
      lineno=0;
      for (int k=0; k<c.argv-1; k++) {
	++it;
	++lineno;
      }
      break;
    }
  }
  if (it==script.contents().end())
    stop();
}
  

void Scripts::stop() {
  Dbg() << "Scripts::stop";
  timer.stop();
  statusLabel->setText(QString("Stopped (at line %1; iteration %2)")
		       .arg(lineno).arg(loopno));
  active = false;
  Globals::ptree->find("scripts/_run").set("off");
  Globals::gui->findPage("scripts").open();
}


void Scripts::changed() {
  statusLabel->setText("Script edited");
}
