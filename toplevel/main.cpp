// main.cpp

#include "vscopegui.h"
#include "globals.h"
#include "gt_slots.h"
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>

#include <QPlastiqueStyle>

#include <QDateTime>

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
#include <gui/xmlpage.h>
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
#include <video/videoprog.h>
#include <gui/videogui.h>

#include <toplevel/acquire.h>
#include <toplevel/mgauto.h>
#include <toplevel/vsdtraces.h>
#include <toplevel/coherence.h>
#include <toplevel/cohgraph.h>
#include <toplevel/savedsettings.h>
#include <toplevel/exptlog.h>
#include <gfx/blackout.h>
#include <gui/guiexc.h>
#include <math/taperbank.h>
#include <math/cohdata.h>

#include <acq/ccdacq.h>

#include <QTextEdit>

extern QString checkdaq();
extern QStringList checkcam();

QWidget *makeBanner1(QWidget *parent) {
  QTextEdit *w = new QTextEdit(parent);
  w->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  w->setReadOnly(true);
  XML vsndoc(":/version.xml");
  QDomElement vsn = vsndoc.root();
  QString txt = "<html><body><h1>VScope ";
  txt += vsn.attribute("branch");
  txt += ":" + vsn.attribute("rev") + "</h1>";
  txt += "<p>(C) Daniel A. Wagenaar 2008&ndash;" + vsn.attribute("year");
  txt += "<p>Last commit: " + vsn.attribute("date") + "<br>";
  txt += "Build date: " + vsn.attribute("builddate");
  txt += "<p>For more info: daw@caltech.edu";

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
  app.setStyle(new QPlastiqueStyle);
  
  QFont f = app.font();
  f.setFamily(BUTTON_FontFamily);
  f.setPixelSize(BUTTON_FontSize);
  app.setFont(f);
}  

void setupParsAndConns() {

  XML enumsDoc(":/enums.xml");
  XML paramsDoc(":/parameters.xml");
  QDomElement enums = enumsDoc.root();
  QDomElement pars = paramsDoc.root();
  
  char const *envpath = getenv("VSCOPEPATH");
  QString fpath = "/";
  if (envpath)
    fpath = envpath;
  else {
    QDomElement e_path = pars.firstChildElement("filepath");
    if (e_path.isNull())
      fpath = QString(getenv("HOME")) + "/vsddata";
    else
      fpath = e_path.attribute("p");
    if (!envpath)
      fprintf(stderr,"Warning: VSCOPEPATH not set. Defaulting to %s\n",
	      qPrintable(fpath));
  }
  
  QString connfn = fpath + "/_settings/_connections.xml";
  if (!QFile::exists(connfn))
    connfn = ":/connections.xml";
  Dbg() << "Reading connections from: " << connfn;
  XML connDoc(connfn);
  QDomElement cons = connDoc.root();

  Enumerator::readAll(enums);
  Enumerator::readAll(pars);
  Enumerator::readAll(cons); // This must happen before ptree is init'ed
  
  Connections::readXML(cons);
  
  Globals::ptree = new ParamTree(pars);
  Dbg() << "fpath2:"<<fpath;
  Globals::ptree->find("filePath").set(fpath);
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
  if (!dbgfile)
    dbgfile = new DbgFile();
  dbgfile->setDir(fpath + "/" + exptname + addn);
}

void setupDAQ() {
  Enumerator *daqenum = Enumerator::find("DAQDEV");
  unsigned int daqtype = daqenum->has("TYPE")
    ? daqenum->lookup("TYPE") : 0;
  unsigned int daqserno = daqenum->has("SERNO")
    ? daqenum->lookup("SERNO") : 0;
  QString id = DAQDevice::search(daqtype, daqserno);
  DAQDevice::addAlias(id, "");
}  

QDomElement setupGUI() {
  QString fpath = Globals::filePath();
  QString guifn = fpath + "/_settings/_guiconfig.xml";
  if (!QFile::exists(guifn))
    guifn = ":/guiconfig.xml";
  Dbg() << "Reading gui config from: " << guifn;
  XML guiConfigDoc(guifn);
  QDomElement guiConf = guiConfigDoc.root();
  Aliases::read(guiConf);
  new Colors(guiConf);
  return guiConf;
}

void setupCams() {
  CamPool::initialize();
  QStringList sz = Connections::allCams();
  foreach (QString id, sz) {
    QString serno = Connections::findCam(id).serno;
    CamPool::rename(serno, id);
    Connections::markCameraExists(id, CamPool::findp(id));
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
  bar->setGeometry(acqPage.width()/5+12, acqPage.height()*4/5+8,
		   acqPage.width()*4/5-17,acqPage.height()/5-15);
  QObject::connect(&Globals::trove->trial(),
		   SIGNAL(newData()),
		   bar, SLOT(newData()));
}

void setupCCDImages() {
  Globals::ccdw = new ROIImages(QRect(0,0,512,512));
  foreach (QString id, Connections::allCams()) {
    ROIImage *img = new ROIImage(Globals::leftplace);
    img->setCamPair(Connections::camPair(id));
    Globals::ccdw->add(id, img);
    img->setGeometry(0,0,512,Globals::mainwindow->basey());
    img->hide();
  }
  Globals::ccdw->setROIs(&Globals::trove->rois());
  SHOWROIS sm = (SHOWROIS)Globals::ptree->find("analysis/showROIs").toInt();
  Globals::ccdw->showROIs(sm);
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

void setupMGAuto(QDomElement &guiConf) {
  Globals::mgintra = new MGAuto(Globals::leftplace,guiConf,"intra");
  Globals::mgintra->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::mgintra->hide();
  
  Globals::mgextra = new MGAuto(Globals::rightplace,guiConf,"extra");
  Globals::mgextra->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::mgextra->hide();
  
  Globals::mgstim = new MGAuto(Globals::leftplace,guiConf,"stim");
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

void setupCoherence() {
  Globals::coherence = new Coherence(0, Globals::rightplace);
  Globals::coherence->setCanvas(Globals::ccdw->currentCanvas());
  Globals::coherence->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::coherence->
    setRefTrace(Globals::ptree->find("analysis/refTrace").toString());
  Globals::coherence->
    setShowMode((SHOWROIS)Globals::ptree->find("analysis/showROIs").toInt());
  Globals::coherence->hide();

  Globals::cohgraph = new CohGraph(Globals::coherence->getData(),
				   Globals::rightplace);
  Globals::cohgraph->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::cohgraph->
    setRefTrace(Globals::ptree->find("analysis/refTrace").toString());
  Globals::cohgraph->hide();

  QObject::connect(Globals::ccdw, SIGNAL(newZoom(QRect)),
		   Globals::coherence, SLOT(updateZoom(QRect)));
  QObject::connect(Globals::coherence, SIGNAL(newZoom(QRect)),
		   Globals::ccdw, SLOT(updateZoom(QRect)));

  QObject::connect(Globals::ccdw, SIGNAL(newSelection(int)),
		   Globals::coherence, SLOT(updateSelection(int)));
  QObject::connect(Globals::ccdw, SIGNAL(newSelection(int)),
		   Globals::cohgraph, SLOT(updateSelection(int)));

  QObject::connect(Globals::coherence, SIGNAL(newSelection(int)),
		   Globals::ccdw, SLOT(updateSelection(int)));
  QObject::connect(Globals::coherence, SIGNAL(newSelection(int)),
		   Globals::cohgraph, SLOT(updateSelection(int)));
  QObject::connect(Globals::coherence, SIGNAL(newSelection(int)),
		   Globals::vsdtraces, SLOT(updateSelection(int)));

  QObject::connect(Globals::cohgraph, SIGNAL(newSelection(int)),
		   Globals::ccdw, SLOT(updateSelection(int)));
  QObject::connect(Globals::cohgraph, SIGNAL(newSelection(int)),
		   Globals::coherence, SLOT(updateSelection(int)));
  QObject::connect(Globals::cohgraph, SIGNAL(newSelection(int)),
		   Globals::vsdtraces, SLOT(updateSelection(int)));

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
  if (okCams.isEmpty()) {
  #if CCDACQ_ACQUIRE_EVEN_WITHOUT_CAMERA
    GUIExc::warn("Cameras not available. Acquisition will take black frames.");
  #else
    GUIExc::warn("Cameras not available. No CCD data will be acquired.");
  #endif
  } else if (!missingCams.isEmpty()) {
    GUIExc::warn("Found camera(s): " + okCams.join(", ")
		 + ",  but missing camera(s): " + missingCams.join(", ")
		 + ".");
  }
#endif
}

void reportDAQSituation() {
  if (!DAQDevice::find().ok()) {
    QString msg = checkdaq();
#ifdef vsdLINUX
    Dbg() << "DAQ not available.";
#else
    GUIExc::warn(msg + "\n"
		 + "Please adjust TYPE and/or SERNO in the DAQDEV enum "
		 + "in 'connections.xml'. "
		 + "Acquisition will NOT work otherwise.");
#endif
  }
}

int main(int argc, char **argv) {
  GUIExc::setArgs(argc, argv);
  QApplication app(argc, argv);
  Warning::enableGUIWarnings();
  try {
    checkTypes();
    setupAppStyle(app);
    setupParsAndConns();

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
    setupCams();

    setupMainWindow(app);
    setupCCDImages();
    setupVSDTraces();
    setupCoherence();
    
    Globals::videogui = new VideoGUI(VideoProg::find());

    Globals::trial = new Trial(&Globals::trove->trial());
    Globals::trial->prepare(Globals::ptree);

    setupMGAuto(guiConf);
    setupAcquisition(guiConf);
    Globals::savedSettings = new SavedSettings(Globals::mainwindow);
    setupFocus();

    Globals::gui = new vscopeGui(Globals::mainwindow, Globals::ptree,guiConf);
    Globals::gtslots = new gt_slots(Globals::gui);
    setupCCDScroll();

    setupTimeButtons();

    Globals::gui->rootPage().setGeometry(0,Globals::mainwindow->basey(),
					 1024,256);
    Globals::gui->open();

    if (Globals::focus)
      Globals::focus->contactButtons(Globals::gui);

    Globals::blackout = new Blackout(Globals::mainwindow);
    Globals::blackout->hide();

    Globals::exptlog = new ExptLog(Globals::mainwindow);

    Globals::panelHistory = new PanelHistory();
    Globals::panelHistory->makeButtons();

    Globals::scripts = new Scripts(&Globals::gui->findPage("scripts"));
    
    Globals::mainwindow->show();

    reportCameraSituation();
    reportDAQSituation();

    Dbg() << "Starting application";
    int res = app.exec();
    Dbg() << "Application done";
    return res;
  } catch (Exception const &e) {
    Warning() << "Exception caught in main.";
    GUIExc::report(e,"");
  }
  return 0;
}

