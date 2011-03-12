// main.cpp

#include "vscopegui.h"
#include "globals.h"
#include "gt_slots.h"
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>

#include <QPlastiqueStyle>

#include <QDateTime>

#include <bzr_versioninfo.h>
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

#include <gui/xmlpage.h>
#include <gui/timebutton.h>
#include <gfx/roiimage.h>
#include <gfx/roiimages.h>
#include <gfx/multigraph.h>
#include <gfx/colors.h>

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
  QString txt = "<html><body><h1>VScope</h1>";
  txt += QString() + "Revision #" + bzr_version_nick + ":" + bzr_version_revno
    + " dd " + bzr_version_month + "/" + bzr_version_day
    + "/" + bzr_version_year + ".<br>";
  txt += QString() + "(C) Daniel A. Wagenaar 2008&ndash;"
    + bzr_version_year + ".<br>";
  txt += "For more info: email daw@caltech.edu.<br>";

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
  QString fpath = getenv("HOME");
  char const *envpath = getenv("VSCOPEPATH");
  if (envpath)
    fpath = envpath;
  else
    fpath = fpath + "/vsddata";

  QString parafn = fpath + "/_settings/_parameters.xml";
  if (true || !QFile::exists(parafn))
    parafn = ":/parameters.xml";
  Dbg() << "Reading parameters from: " << parafn;
  XML paramsDoc(parafn);
  QDomElement pars = paramsDoc.root();
  QDomElement e_path = pars.firstChildElement("filepath");
  if (!envpath && !e_path.isNull())
    fpath = e_path.attribute("p");
  
  if (!envpath)
    fprintf(stderr,"Warning: VSCOPEPATH not set. Defaulting to %s\n",
	    qPrintable(fpath));
  
  QString connfn = fpath + "/_settings/_connections.xml";
  if (!QFile::exists(connfn))
    connfn = ":/connections.xml";
  Dbg() << "Reading connections from: " << connfn;
  XML connDoc(connfn);
  QDomElement cons = connDoc.root();
  
  Enumerator::readAll(pars);
  Enumerator::readAll(cons); // This must happen before ptree is init'ed
  Connections::readXML(connDoc.root());
  Globals::ptree = new ParamTree(pars);
  Globals::ptree->find("_filePath").set(fpath);
}

void setupDefaultSettings() {
  QString settingsfn = Globals::ptree->find("_filePath").toString()
    + "/_settings/Default.xml";
  if (QFile::exists(settingsfn)) {
    Dbg() << "Reading settings from: " << settingsfn;
    XML valuesDoc(settingsfn);
    QDomElement vals = valuesDoc.root();
    Globals::ptree->read(vals);
  }
}

void setupExptName() {
  QString fpath = Globals::ptree->find("_filePath").toString();
  QString exptname = QDateTime::currentDateTime().toString("yyMMdd");
  QString addn = "";
  int adno=0;
  QDir d;
  while (d.exists(fpath + "/" + exptname + addn)) 
    addn = num2az(++adno);
  Globals::ptree->find("acquisition/_exptname").set(exptname + addn);
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
  QString fpath = Globals::ptree->find("_filePath").toString();
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
  CamPool();
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
  ROIImage::ShowMode sm =
    ROIImage::ShowMode(Globals::ptree->find("analysis/showROIs").toInt());
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
    setDebleach(ROIData::Debleach(Globals::ptree->find("analysis/debleach")
				    .toInt()));
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
    setShowMode(ROIImage::ShowMode(Globals::ptree->find("analysis/showROIs")
				    .toInt()));
  Globals::coherence->hide();

  Globals::cohgraph = new CohGraph(Globals::coherence->getData(),
				   Globals::rightplace);
  Globals::cohgraph->setGeometry(0,0,512,Globals::mainwindow->basey());
  Globals::cohgraph->
    setRefTrace(Globals::ptree->find("analysis/refTrace").toString());
  Globals::cohgraph->hide();

  QObject::connect(Globals::ccdw, SIGNAL(newZoom(QRect)),
		   Globals::coherence, SLOT(updateZoom(QRect)));
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
		   &Globals::gui->findButton("acquisition/_walltime"));
  Globals::exptelapsed =
    new TimeButton(0,
		   &Globals::gui->findButton("acquisition/_exptelapsed"));
  Globals::trialelapsed =
    new TimeButton(0,
		   &Globals::gui->findButton("acquisition/_trialelapsed"));
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
  try {
    checkTypes();
    setupAppStyle(app);
    setupParsAndConns();

    QString fpath = Globals::ptree->find("_filePath").toString();
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
    
    return app.exec();
  } catch (Exception const &e) {
    Warning() << "Exception caught in main.";
    GUIExc::report(e,"");
  }
  return 0;
}

