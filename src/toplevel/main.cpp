// toplevel/main.cpp - This file is part of VScope.
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

// main.cpp

#include "vscopegui.h"
#include "globals.h"
#include "gt_slots.h"
#include <base/progressdialog.h>
#include <QWidget>
#include <QGuiApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QDateTime>
#include <QProcessEnvironment>

#include <base/dbg.h>
#include <base/base26.h>
#include <base/xml.h>
#include <xml/aliases.h>
#include <xml/param.h>
#include <xml/paramtree.h>
#include <xml/connections.h>
#include <xml/enumerator.h>
#include <base/exception.h>
#include <base/types.h>
#include <base/roidata3set.h>
#include <toplevel/panelhistory.h>
#include <toplevel/mainwindow.h>
#include <toplevel/scripts.h>
#include <toplevel/camimages.h>
#include <toplevel/cohimages.h>
#include <daq/daqbase.h>
#include <pvp/campool.h>
#include <base/roi3data.h>
#include <QDomDocument>
#include <QDomElement>
#include <QTextStream>
#include <QFile>
#include <stdio.h>
#include <stdlib.h>
#include <pvp/campool.h>
#include <gui/guipage.h>
#include <gui/timebutton.h>
#include <gfx/roiimage.h>
#include <gfx/roiimages.h>
#include <gfx/multigraph.h>
#include <gfx/colors.h>
#include <toplevel/ccdscroll.h>

#include <acq/liveephys.h>
#include <acq/focus.h>
#include <acq/trial.h>
#include <acq/datatrove.h>
#include <acq/contacq.h>

#include <toplevel/acquire.h>
#include <toplevel/mgauto.h>
#include <toplevel/vsdtraces.h>
#include <gfx/coherence.h>
#include <gfx/cohgraph.h>
#include <toplevel/savedsettings.h>
#include <toplevel/exptlog.h>
#include <gfx/blackout.h>
#include <gui/guiexc.h>
#include <math/taperbank.h>
#include <math/cohdata.h>
#include <video/vprojector.h>

#include <acq/ccdacq.h>

#include <QTextEdit>

extern QString checkdaq();
extern QStringList checkcam();

#include "toplevel/version.h"

QWidget *makeBanner1(QWidget *parent) {
  QTextEdit *w = new QTextEdit(parent);
  w->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  w->setReadOnly(true);

  QString txt = "<h2>VScope ";
  
  txt += versionBanner("</h2>");

  txt+= "<p>VScope working dir: " + QDir::currentPath();
  txt += "<p>System path: "
    + QProcessEnvironment::systemEnvironment().value("PATH");
  
  txt += "<h2>DAQ status</h2>";
  QString daqst = checkdaq();
  daqst.replace("\n","<br>");
  txt += daqst;

  txt += "</body></html>";
  w->setHtml(txt);
  return w;
}

QWidget *makeBanner2(QWidget *parent) {
  QTextEdit *w = new QTextEdit(parent);
  w->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  w->setReadOnly(true);
  QString txt = "<html><body>";
  
  txt += "<h2>Camera status</h2>";
  QStringList camst = checkcam();
  QString c0 = camst.takeFirst();
  c0.replace("\n","<br>");
  txt += c0; txt += "<br>";
  while (!camst.isEmpty()) {
    QString c = camst.takeFirst();
    QStringList lines = c.split("\n");
    txt += "<h3>" + lines.takeFirst() + "</h3>";
    c = lines.join("<br>");
    txt += c;
  }
  
  txt += "</body></html>";
  w->setHtml(txt);
  return w;
}

void setupAppStyle(QApplication &app) {
  QFont f = app.font();
  f.setFamily(BUTTON_FontFamily);
  f.setPixelSize(BUTTON_FontSize);
  app.setFont(f);
  app.setWindowIcon(QIcon(":/vscope.png"));
}  

void setupAIChannels() {
  QStringList aich = Connections::analogInputs();
  while (aich.size()>2)
    aich.takeLast();
  QSet<QString> ai = QSet<QString>::fromList(aich);
  Globals::ptree->find("maintenance/liveEphys/aiChannels").setStrings(ai);
  Globals::ptree->find("acqEphys/aiChannels").setStrings(ai);
}

void setupParsAndConns(char const *pathoverride) {
  XML enumsDoc(":/enums.xml");
  XML paramsDoc(":/parameters.xml");
  QDomElement enums = enumsDoc.root();
  QDomElement pars = paramsDoc.root();
  
  char const *envpath = getenv("VSCOPEPATH");
  QString fpath = "/";
  if (pathoverride) {
    QDir dir(pathoverride);
    fpath = dir.absolutePath();
  } else if (envpath) {
    fpath = envpath;
    Dbg() << "Got path from VSCOPEPATH: " << fpath;
  } else {
#ifdef vsdWIN32
      fpath = QString(getenv("HOMEDRIVE")) + QString(getenv("HOMEPATH"))
                + "\\Documents\\vsddata";
#else
    fpath = QString(getenv("HOME")) + "/vsddata";
#endif
    Warning(0) << "VSCOPEPATH not set. Defaulting to" << fpath;
    fflush(stderr);
  }

  QString spath = fpath + "/_settings";
  QString connfn = spath + "/_connections.xml";
  if (!QFile::exists(connfn)) {
    QDir x(QDir::root());
    if (!x.exists(spath))
      x.mkpath(spath);
    QFile f(":/connections.xml");
    if (!f.copy(connfn))
      throw Exception("main", "Cannot create connections file: " + connfn);
  }
  Dbg() << "Reading connections from: " << connfn;
  XML connDoc(connfn);
  QDomElement cons = connDoc.root();

  Enumerator::readAll(enums);
  
  Connections::readXML(cons);
  
  Globals::ptree = new ParamTree(pars);
  Globals::ptree->find("filePath").set(fpath);

  setupAIChannels();
}

void setupDefaultSettings() {
  QString settingsfn = Globals::filePath()
    + "/_settings/Default.xml";
  if (QFile::exists(settingsfn)) {
    Dbg() << "Reading settings from: " << settingsfn;
    XML valuesDoc(settingsfn);
    Globals::ptree->read(valuesDoc.root());
  }
}

void setupExptName() {
  QString fpath = Globals::filePath();
  QString exptname = QDateTime::currentDateTime().toString("yyMMdd");
  QString addn = "";
  int adno=0;
  QDir d;
  while (d.exists(fpath + "/" + exptname + addn)) 
    addn = num2az(++adno);
  Globals::ptree->find("acquisition/exptname").set(exptname + addn);
}

void setupDbgFile() {
  QString fpath = Globals::filePath();
  QString exptname = Globals::ptree->find("acquisition/exptname").toString();
  if (!dbgfile)
    dbgfile = new DbgFile();
  dbgfile->setDir(fpath + "/" + exptname);
}

void setupDAQ() {
  QString id = DAQDevice::search(0, 0);
  /* There used to be a funky DAQDEV enum that defined TYPE and SERNO
     tags. But it was an odd use of enums. Better to define a new <daq>
     tag in connections. But for now: just assume one device, hence (0,0).
  */
  DAQDevice::addAlias(id, "");
}  

QDomElement setupGUI() {
  QString fpath = Globals::filePath();
  QString guifn = ":/guiconfig.xml";
  XML guiConfigDoc(guifn);
  QDomElement guiConf = guiConfigDoc.root();
  Aliases::add(guiConf);
  Colors::add(guiConf);
  return guiConf;
}

void setupAliases() {
  QString spath = Globals::filePath() + "/_settings";
  QString alifn = spath + "/_aliases.xml";
  if (!QFile::exists(alifn)) {
    QDir x(QDir::root());
    if (!x.exists(spath))
      x.mkpath(spath);
    QFile f(":/aliases.xml");
    if (!f.copy(alifn))
      throw Exception("main", "Cannot create aliases file");
  }
  XML aliDoc(alifn);
  QDomElement ali = aliDoc.root();
  Aliases::add(ali);
  Colors::add(ali);
}
  
void setupCams() {
  Dbg() << "setupcams";
  CamPool::initialize();
  QStringList sz = Connections::allCams();
  foreach (QString id, sz) {
    QString serno = Connections::findCam(id).serno;
    bool xist = CamPool::findp(serno);
    Dbg() << "setupcams" << id << " "  << xist << " " << serno;
    Connections::markCameraExists(id, xist);
    if (xist) {
      CamPool::rename(serno, id);
      CamPool::find(id).fullReport();
    }
  }
}

void setupMainWindow(QApplication &app) {
  Globals::mainwindow = new MainWindow(app);
  Globals::leftplace = makeBanner1(Globals::mainwindow);
  Globals::leftplace->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::rightplace = makeBanner2(Globals::mainwindow);
  Globals::rightplace->setGeometry(512,0,512,Globals::mainwindow->basey());
}

void setupCCDScroll() {
  QWidget &acqPage = Globals::gui->findPage("acquisition");
  CCDScroll *bar = new CCDScroll(&acqPage);
  bar->setGeometry(acqPage.width()*3/5+12, acqPage.height()*4/5+8,
		   acqPage.width()*2/5-17,acqPage.height()/5-15);
  QObject::connect(&Globals::trove->trial(),
		   SIGNAL(newData()),
		   bar, SLOT(newData()));
}

void setupVSDTraces() {
  Globals::vsdtraces = new VSDTraces(Globals::rightplace);
  Globals::vsdtraces->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::vsdtraces->
    setRefTrace(Globals::ptree->find("analysis/refTrace").toString());
  Globals::vsdtraces->hide();

  QObject::connect(Globals::ccdw, SIGNAL(newSelection(int)),
		   Globals::vsdtraces, SLOT(updateSelection(int)));

  Globals::trove->roidata().
    setDebleach((DEBLEACH)Globals::ptree->find("analysis/debleach").toInt());
}

void setupMGAuto(QDomElement &) {
  Globals::mgintra = new MGAuto(Globals::leftplace, "intra");
  Globals::mgintra->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::mgintra->hide();
  
  Globals::mgextra = new MGAuto(Globals::rightplace, "extra");
  Globals::mgextra->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::mgextra->hide();
  
  Globals::mgstim = new MGAuto(Globals::leftplace, "stim");
  Globals::mgstim->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::mgstim->hide();
}

void setupFocus() {
  try {
    Globals::focus = new Focus(Globals::mainwindow);
    Globals::focus->setGeometry(0,0,1024,Globals::mainwindow->basey());
    Globals::focus->hide();
  } catch (...) {
    Dbg() << "Focus not available without cameras";
    Globals::focus=0;
  }
}

CohImages *setupCoherence() {
  CohImages *coi = new CohImages(&Globals::trove->cohdata());
  Globals::cohmaps = coi;
  
  Globals::cohgraph = new CohGraph(&Globals::trove->cohdata(),
				   Globals::rightplace);
  Globals::cohgraph->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::cohgraph->
    setRefTrace(Globals::ptree->find("analysis/refTrace").toString());
  Globals::cohgraph->hide();

  QObject::connect(Globals::ccdw, SIGNAL(newZoom(QRect)),
		   Globals::cohmaps, SLOT(updateZoom(QRect)));
  QObject::connect(Globals::cohmaps, SIGNAL(newZoom(QRect)),
		   Globals::ccdw, SLOT(updateZoom(QRect)));

  QObject::connect(Globals::ccdw, SIGNAL(newSelection(int)),
		   Globals::cohmaps, SLOT(updateSelection(int)));
  QObject::connect(Globals::ccdw, SIGNAL(newSelection(int)),
		   Globals::cohgraph, SLOT(updateSelection(int)));
  // ccdw and vsdtraces are connected in setupVSDTraces

  QObject::connect(Globals::cohmaps, SIGNAL(newSelection(int)),
		   Globals::ccdw, SLOT(updateSelection(int)));
  QObject::connect(Globals::cohmaps, SIGNAL(newSelection(int)),
		   Globals::cohgraph, SLOT(updateSelection(int)));
  QObject::connect(Globals::cohmaps, SIGNAL(newSelection(int)),
		   Globals::vsdtraces, SLOT(updateSelection(int)));

  QObject::connect(Globals::cohgraph, SIGNAL(newSelection(int)),
		   Globals::ccdw, SLOT(updateSelection(int)));
  QObject::connect(Globals::cohgraph, SIGNAL(newSelection(int)),
		   Globals::cohmaps, SLOT(updateSelection(int)));
  QObject::connect(Globals::cohgraph, SIGNAL(newSelection(int)),
		   Globals::vsdtraces, SLOT(updateSelection(int)));

  return coi;
}
 
void setupAcquisition(QDomElement &guiConf) {
  Globals::acquire = new Acquire();
  Globals::contacq = new ContAcq();
 
  Globals::liveephys = new LiveEPhys(Globals::mainwindow,guiConf);
  Globals::liveephys->setGeometry(0,0,1024,Globals::mainwindow->basey());
  Globals::liveephys->hide();
}

void setupTimeButtons() {
  Globals::walltime =
    new TimeButton(0,
		   &Globals::gui->findButton("acquisition/walltime"));
  Globals::exptelapsed =
    new TimeButton(0,
		   &Globals::gui->findButton("acquisition/exptelapsed"));
  Globals::trialelapsed =
    new TimeButton(0,
		   &Globals::gui->findButton("acquisition/trialelapsed"));
  Globals::walltime->showWallTime();
}  

void reportCameraSituation() {
  QStringList allCams = Connections::allCams();
  Dbg() << "Looking for cameras: " << allCams.join(", ") << ".";
  
  QStringList okCams;
  QStringList missingCams;
  foreach (QString s, allCams) {
    if (CamPool::findp(s))
      okCams.append(s);
    else
      missingCams.append(s);
  }

  if (!okCams.isEmpty())
    Dbg() << "Found camera(s): " << okCams.join(", ") << ".";
  if (!missingCams.isEmpty())
    Dbg() << "Missing camera(s): " << missingCams.join(", ") << ".";
  
#ifndef vsdLINUX
  if (okCams.isEmpty() && !missingCams.isEmpty()) {
  #if CCDACQ_ACQUIRE_EVEN_WITHOUT_CAMERA
    Warning() << "Cameras not available. Acquisition will take black frames.";
  #else
    Warning() << "Cameras not available. No CCD data will be acquired.";
  #endif
  } else if (!missingCams.isEmpty()) {
    Warning() << "Found camera(s): " << okCams.join(", ")
	      << ",  but missing camera(s): " << missingCams.join(", ")
	      << ".";
  }
#endif
}

void reportDAQSituation() {
  if (!DAQDevice::find().isValid()) {
    QString msg = checkdaq();
#ifdef vsdLINUX
    Dbg() << "DAQ not available.";
#else
    Warning() << msg + "\n"
	      << "Please adjust TYPE and/or SERNO in the DAQDEV enum "
	      << "in 'connections.xml'. "
	      << "Acquisition will NOT work otherwise.";
#endif
  }
}

int realmain(int argc, char **argv) {
  GUIExc::setArgs(argc, argv);
  QApplication app(argc, argv);
#if QT_VERSION >= 0x050601
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
  Warning::enableGUIWarnings();
  try {
    QObject guard;
    checkTypes();
    setupAppStyle(app);
    setupParsAndConns(argc>=2 ? argv[1] : 0);

    QString fpath = Globals::filePath();
    Globals::trove = new DataTrove(Globals::ptree);
    Globals::trove->rois().setDefaultCamPair(Connections::leaderCamPair());
    GUIExc::setParamTree(Globals::ptree);
    GUIExc::setSettingsDir(fpath + "/_settings");

    setupDefaultSettings();

    Taperbank::initialize(fpath + "/_tapers");
    setupExptName();
    setupDAQ();
    QDomElement guiConf = setupGUI();
    setupAliases();
    setupCams();

    setupMainWindow(app);

    CamImages *ci = new CamImages;
    Globals::ccdw = ci;

    setupVSDTraces();
    CohImages *coi = setupCoherence();
    
    Globals::vprojector = new VProjector(&guard);

    Globals::trial = new Trial(&Globals::trove->trial());
    Globals::trial->prepare(Globals::ptree);

    setupMGAuto(guiConf);
    setupAcquisition(guiConf);
    Globals::savedSettings = new SavedSettings(Globals::mainwindow);
    setupFocus();

    Globals::gui = new vscopeGui(Globals::mainwindow, Globals::ptree,guiConf);
    Globals::gtslots = new gt_slots(Globals::gui);

    ci->setup();
    coi->setup();
    setupCCDScroll();

    setupTimeButtons();

    
    Globals::gui->rootPage().setGeometry(0,Globals::mainwindow->basey(),
					 1024,256);
    Globals::gui->open();

    if (Globals::focus)
      Globals::focus->contactButtons(Globals::gui);
    else
      Dbg() << "No focus page---Cannot contactButtons";

    Globals::blackout = new Blackout(Globals::mainwindow);
    Globals::blackout->hide();

    Globals::exptlog = new ExptLog(Globals::mainwindow);

    Globals::panelHistory = new PanelHistory(Globals::ptree,
                                             Globals::gui);
    Globals::panelHistory->makeButtons();

    Globals::scripts = new Scripts(&Globals::gui->findPage("scripts"));

    ProgressDialog::setRoot(Globals::mainwindow);
    Globals::mainwindow->show();
    
    reportCameraSituation();
    reportDAQSituation();

    setupDbgFile();
    
    Dbg() << "Starting application";
    int res = app.exec();
    Dbg() << "Application done";
    CamPool::closedown();
    return res;
  } catch (Exception const &e) {
    Dbg() << "Exception caught in main.";
    GUIExc::report(e, "");
  }
  return 0;
}

int main(int argc, char **argv) {
  Dbg() << "main";
  int r = realmain(argc, argv);
  Dbg() << "back" << r;
  return r;
}
