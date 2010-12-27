// main.cpp


#include "vscopegui.h"
#include "globals.h"
#include "gt_slots.h"
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>

#include <QPlastiqueStyle>

#include <QDateTime>

#include <toplevel/version.h>
#include <base/dbg.h>
#include <base/base26.h>
#include <base/xml.h>
#include <xml/param.h>
#include <xml/paramtree.h>
#include <xml/connections.h>
#include <xml/enumerator.h>
#include <base/exception.h>
#include <base/types.h>
#include <base/roiset3data.h>
#include <toplevel/roisetguard.h>
#include <toplevel/panelhistory.h>
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
#include <gfx/multigraph.h>

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

vscopeGui *Globals::gui;
gt_slots *Globals::gtslots;

QWidget *Globals::mainwindow;
QWidget *Globals::leftplace;
QWidget *Globals::rightplace;

ROIImage *Globals::oxonolw;
ROIImage *Globals::coumarinw;
VSDTraces *Globals::vsdtraces;
Coherence *Globals::coherence;
CohGraph *Globals::cohgraph;

MGAuto *Globals::mgintra;
MGAuto *Globals::mgextra;
MGAuto *Globals::mgstim;

LiveEPhys *Globals::liveephys;
Focus *Globals::focus;

Trial *Globals::trial;
ContAcq *Globals::contacq;
Acquire *Globals::acquire;
VideoGUI *Globals::videogui;

SavedSettings *Globals::savedSettings;

TimeButton *Globals::walltime;
TimeButton *Globals::exptelapsed;
TimeButton *Globals::trialelapsed;

Blackout *Globals::blackout;

ExptLog *Globals::exptlog;

ParamTree *Globals::ptree;
DataTrove *Globals::trove;

Scripts *Globals::scripts;

PanelHistory *Globals::panelHistory;

extern QString checkdaq();
extern QStringList checkcam();

QWidget *makeBanner1(QWidget *parent) {
  QTextEdit *w = new QTextEdit(parent);
  w->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  w->setReadOnly(true);
  QString txt = "<html><body><h1>VScope ";
  txt += VSCOPE_VERSION; txt += "</h1>";
  txt += VSCOPE_COPYRIGHT; txt += "<br>";

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

void setFlips() {
  Connections::CamCon const &donCam = Connections::findCam("Cc");
  Connections::CamCon const &accCam = Connections::findCam("Ox");
  ROI3Data::setFlip(donCam.flipx, donCam.flipy,
		    accCam.flipx, accCam.flipy);
}

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  try {
    checkTypes();
    app.setStyle(new QPlastiqueStyle);

    QFont f = app.font();
    f.setFamily("Arial Rounded MT");
    f.setPixelSize(13);
    app.setFont(f);

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

    Globals::ptree = new ParamTree(pars);
    Globals::trove = new DataTrove(Globals::ptree);
    Globals::ptree->find("_filePath").set(fpath);
    
    Connections::readXML(connDoc.root());

    setFlips();
    
    QString settingsfn = fpath + "/_settings/Default.xml";
    if (QFile::exists(settingsfn)) {
      Dbg() << "Reading settings from: " << settingsfn;
      XML valuesDoc(settingsfn);
      QDomElement vals = valuesDoc.root();
      Globals::ptree->read(vals);
    }

    Taperbank::initialize(fpath + "/_tapers");
    CohData::setFrameLine(Enumerator::find("DIGILINES")->lookup("FrameCc"));
    
    QString exptname = QDateTime::currentDateTime().toString("yyMMdd");
    QString addn = "";
    int adno=0;
    QDir d;
    while (d.exists(fpath + "/" + exptname + addn)) 
      addn = num2az(++adno);
    Globals::ptree->find("acquisition/_exptname").set(exptname + addn);

    Enumerator *daqenum = Enumerator::find("DAQDEV");
    unsigned int daqtype = daqenum->has("TYPE")
      ? daqenum->lookup("TYPE") : 0;
    unsigned int daqserno = daqenum->has("SERNO")
      ? daqenum->lookup("SERNO") : 0;
    QString id = DAQDevice::search(daqtype, daqserno);
    DAQDevice::addAlias(id, "");

    QString guifn = fpath + "/_settings/_guiconfig.xml";
    if (!QFile::exists(guifn))
      guifn = ":/guiconfig.xml";
    Dbg() << "Reading gui config from: " << guifn;
    XML guiConfigDoc(guifn);
    QDomElement guiConf = guiConfigDoc.root();

    CamPool();
    QStringList sz = Connections::allCams();
    for (QStringList::iterator i=sz.begin(); i!=sz.end(); ++i) {
      QString serno = Connections::findCam(*i).serno;
      CamPool::rename(serno,*i);
    }
    
    QWidget mw(0);
    Globals::mainwindow=&mw;
    mw.setWindowTitle("VScope");
    int deskw = app.desktop()->width();
    int deskh = app.desktop()->height();
    dbg("Desktop size is %i x %i\n",deskw,deskh);
    int winh=768;
    if (deskw>1050 && deskh>=800) {
      mw.setGeometry(deskw/2-512,deskh/2-384,1024,winh);
    } else {
      winh=deskh;
      mw.setWindowState(Qt::WindowFullScreen);
    }
    int basey = winh-256;

    Globals::leftplace = makeBanner1(&mw);
    Globals::leftplace->setGeometry(0,0,512,basey);
    Globals::rightplace = makeBanner2(&mw);
    Globals::rightplace->setGeometry(512,0,512,basey);
    
    Globals::coumarinw = new ROIImage(Globals::leftplace);
    Globals::oxonolw = new ROIImage(Globals::rightplace);
    Globals::coumarinw->setROIs(&Globals::trove->rois());
    Globals::oxonolw->setROIs(&Globals::trove->rois());
    Globals::coumarinw->setGeometry(0,0,512,basey);
    Globals::oxonolw->setGeometry(0,0,512,basey);
    { ROIImage::ShowMode sm =
	ROIImage::ShowMode(Globals::ptree->find("analysis/showROIs").toInt());
      Globals::coumarinw->showROIs(sm);
      Globals::oxonolw->showROIs(sm);
    }
    Globals::coumarinw->hide();
    Globals::oxonolw->hide();

    #define conn(s,d,si,sl) \
        QObject::connect(Globals::s,SIGNAL(si), \
    		     Globals::d,SLOT(sl));
    #define connmut(s,d,si,sl)			\
        conn(s,d,si,sl); conn(d,s,si,sl);
    #define conn2(s1,s2,d, si,sl) \
        conn(s1,d,si,sl); \
        conn(s2,d,si,sl);

    connmut(oxonolw, coumarinw, shareZoom(bool,QRect), sharedZoom(bool,QRect));
    connmut(oxonolw, coumarinw, selectedROI(int), acceptROIselect(int));
    connmut(oxonolw, coumarinw, editedROI(int), acceptROIedit(int));
    connmut(oxonolw, coumarinw, deletedROI(int), acceptROIdelete(int));

    conn2(oxonolw, coumarinw, trove, editedROI(int),saveROIs());
    conn2(oxonolw, coumarinw, trove, deletedROI(int),saveROIs());

    Globals::vsdtraces = new VSDTraces(Globals::rightplace);
    Globals::vsdtraces->newROIs(&Globals::trove->rois());
    Globals::vsdtraces->setGeometry(0,0,512,basey);
    Globals::vsdtraces->
      setRefTrace(ROIData::Debleach(Globals::ptree->find("analysis/refTrace")
				    .toInt()));
    Globals::vsdtraces->
      setDebleach(ROIData::Debleach(Globals::ptree->find("analysis/debleach")
				    .toInt()));
    QObject::connect(Globals::vsdtraces,SIGNAL(roisChanged()),
                     Globals::trove,SLOT(saveROIs()));
    Globals::vsdtraces->hide();

    Globals::coherence = new Coherence(Globals::vsdtraces, 0,
				       Globals::rightplace);
    Globals::coherence->setGeometry(0,0,512,basey);
    Globals::coherence->
      setRefTrace(ROIData::Debleach(Globals::ptree->find("analysis/refTrace")
				    .toInt()));
    Globals::coherence->
      setShowMode(ROIImage::ShowMode(Globals::ptree->find("analysis/showROIs")
				    .toInt()));
    Globals::coherence->hide();

    Globals::cohgraph = new CohGraph(Globals::vsdtraces,
				     Globals::coherence->getData(),
				     Globals::rightplace);
    Globals::cohgraph->setGeometry(0,0,512,basey);
    Globals::cohgraph->
      setRefTrace(ROIData::Debleach(Globals::ptree->find("analysis/refTrace")
				    .toInt()));
    Globals::cohgraph->hide();
    
    conn2(oxonolw, coumarinw, vsdtraces, editedROI(int),editROI(int));
    conn2(oxonolw, coumarinw, vsdtraces, selectedROI(int),selectROI(int));
    conn2(oxonolw, coumarinw, vsdtraces, deletedROI(int),deleteROI(int));

    connmut(oxonolw, coherence, shareZoom(bool,QRect), sharedZoom(bool,QRect));
    connmut(coumarinw, coherence, shareZoom(bool,QRect), sharedZoom(bool,QRect));
    
    Globals::videogui = new VideoGUI(VideoProg::find());

    Globals::trial = new Trial(&Globals::trove->trial());
    Globals::trial->prepare(Globals::ptree);

    Globals::contacq = new ContAcq();

    Globals::mgintra = new MGAuto(Globals::leftplace,guiConf,"intra");
    Globals::mgintra->setGeometry(0,0,512,basey);
    Globals::mgintra->hide();

    Globals::mgextra = new MGAuto(Globals::rightplace,guiConf,"extra");
    Globals::mgextra->setGeometry(0,0,512,basey);
    Globals::mgextra->hide();

    Globals::mgstim = new MGAuto(Globals::leftplace,guiConf,"stim");
    Globals::mgstim->setGeometry(0,0,512,basey);
    Globals::mgstim->hide();

    Globals::acquire = new Acquire();

    Globals::liveephys = new LiveEPhys(&mw,guiConf);
    Globals::liveephys->setGeometry(0,0,1024,basey);
    Globals::liveephys->hide();

    Globals::savedSettings = new SavedSettings(&mw);

    try {
      Globals::focus = new Focus(&mw);
      Globals::focus->setGeometry(0,0,1024,basey);
      Globals::focus->hide();
    } catch (...) {
      fprintf(stderr,"Focus not available without cameras\n");
      Globals::focus=0;
    }

    Globals::gui = new vscopeGui(&mw,Globals::ptree,guiConf);
    Globals::gtslots = new gt_slots(Globals::gui);

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

    Globals::gui->rootPage().setGeometry(0,basey,1024,256);
    Globals::gui->open();

    if (Globals::focus)
      Globals::focus->contactButtons(Globals::gui);

    Globals::blackout = new Blackout(&mw);
    Globals::blackout->hide();

    Globals::exptlog = new ExptLog(&mw);

    Globals::panelHistory = new PanelHistory();
    Globals::panelHistory->makeButtons();

    Globals::scripts = new Scripts(&Globals::gui->findPage("scripts"));
    
    //// test code follows
    //Trial trial(Globals::connections,Globals::cameras);
    //trial.prepare(Globals::ptree);
    //trial.write(".");
    //trial.read(".","081024","001", Globals::ptree);
    //// end of test code

    mw.show();

    if (!CamPool::haveAll(Connections::allCams())) {
#ifdef vsdLINUX
      fprintf(stderr,"Cameras not available.\n");
#else
#if CCDACQ_ACQUIRE_EVEN_WITHOUT_CAMERA
      guiwarn("Cameras not available. Acquisition will take black frames.");
#else
      guiwarn("Cameras not available. No CCD data will be acquired.");
#endif
#endif
    }
    if (!DAQDevice::find().ok()) {
      QString msg = checkdaq();
#ifdef vsdLINUX
      fprintf(stderr,"DAQ not available.\n");
#else
      guiwarn(msg + "\nPlease adjust TYPE and/or SERNO in the DAQDEV enum in 'connections.xml'. Acquisition will NOT work otherwise.");
#endif
    }
    return app.exec();
  } catch (Exception const &e) {
    fprintf(stderr,"Caught in main:\n");
    e.report();
    try {
      report(e,"");
    } catch (...) {
    }
  }
  return 0;
}

