// videogui.cpp

#include "videogui.h"

#include <gui/guipage.h>
#include <gui/guiroot.h>
#include <gui/guibutton.h>
#include <gui/guiexc.h>
#include <xml/paramtree.h>
#include <base/dbg.h>

VideoGUI::VideoGUI(VideoProg &vprog): vprog(vprog) {
  populatedprognames = false;
}

VideoGUI::~VideoGUI() {
}

bool VideoGUI::openPage(guiRoot *gui, ParamTree *ptree) {
  if (!vprog.ok()) {
    if (ptree->find("stimVideo/enable").toBool())
      failure(Exception("VideoGUI","No video system"),gui,ptree);
    gui->findButton("stimVideo/enable").setEnabled(false);
    return false;
  }
  if (!vprog.enabled(ptree))
    return true;
  try {
    ensureGUI(gui, ptree);
    vprog.ensureSettings(ptree);
  } catch (Exception const &e) {
    failure(e, gui, ptree);
    return false;
  }
  return true;
}  

bool VideoGUI::reset(guiRoot *gui, ParamTree *ptree) {
  if (!vprog.ok()) 
    return false;
  try {
    ensureGUI(gui, ptree);
    vprog.reset(ptree);
  } catch (Exception const &e) {
    failure(e, gui, ptree);
    return false;
  }
  return true;
}

bool VideoGUI::changeEnable(guiRoot *gui, ParamTree *ptree) {
  Dbg() << "VideoGUI::changeEnable";
  if (!vprog.ok()) 
    return false;
  if (!vprog.enabled(ptree))
    return true;
  try {
    Dbg() << "VG: ensureGui";
    ensureGUI(gui, ptree);
    Dbg() << "VG: ensureSettings";
    vprog.ensureSettings(ptree);
  } catch (Exception const &e) {
    failure(e, gui, ptree);
    return false;
  }
  Dbg() << "VG: OK";
  return true;
}

bool VideoGUI::changeProgram(guiRoot *gui, ParamTree *ptree) {
  if (!vprog.ok()) 
    return false;
  try {
    bool ena = vprog.enabled(ptree);
    ensureGUI(gui, ptree);
    vprog.setProgram(ptree, ena);
    for (int k=1; k<=VideoProg::MAXPAR; k++)
      vprog.setParam(ptree, k, ena);
  } catch (Exception const &e) {
    failure(e, gui, ptree);
    return false;
  }
  return true;
}

bool VideoGUI::changeParam(guiRoot *gui, ParamTree *ptree, QString parid) {
  if (!vprog.ok()) 
    return false;
  int parno=1 + char(parid[3].toAscii()) - 'A';
  try {
    vprog.setParam(ptree,parno,vprog.enabled(ptree));
  } catch (Exception const &e) {
    failure(e, gui, ptree);
    return false;
  }
  return true;
}

void VideoGUI::failure(Exception const &e, guiRoot *gui, ParamTree *ptree) {
  GUIExc::warn("Communication failure with Video device: " + e + ".\n" + 
	       "Video production disabled. " +
	       "Please ensure the Video device is operational and try again.");
  ptree->find("stimVideo/enable").setBool(false);
  connect(this,SIGNAL(failed()),
	  gui->findpPage("stimVideo"),SLOT(open()),Qt::QueuedConnection);
  emit failed();
  disconnect(this,SIGNAL(failed()),
	     gui->findpPage("stimVideo"),SLOT(open()));
}

void VideoGUI::populateProgNames(guiPage *gui) {
  dbg("VideoGUI::populateProgNames");
  guiPage &pg = dynamic_cast<guiPage&>(gui->findPage(QStringList(QString("prog"))));
  QList<guiButton *> buttons = pg.allButtons();
  QList<guiButton *>::iterator but_i = buttons.begin();
  QMap<int, QString> const &names = vprog.progNames();
  for (QMap<int, QString>::const_iterator i=names.begin();
       i!=names.end(); ++i) {
    int progno = i.key();
    QString progname = i.value();
     while (but_i!=buttons.end() && (*but_i)->isCustom())
      but_i++;
    if (but_i!=buttons.end()) {
      (*but_i)->setValue(progname);
      (*but_i)->setVisible(true);
      but_i++;
    } else {
      dbg("Note: I tried to name program #%i '%s', but there is no button.",
	  progno,qPrintable(progname));
    }
  }
  while (but_i != buttons.end()) {
    (*but_i)->setVisible((*but_i)->isCustom());
    but_i++;
  }    
  dbg("  VideoProg::populateProgNames done");
  populatedprognames = true;
}

  
void VideoGUI::populateParValues(guiPage *gui, int prog, int par) {
  Dbg() << "VideoGUI::populateParValues: " << prog << " " << par;
  QStringList const &values = vprog.parValues(prog,par);
  guiPage &pg =
    dynamic_cast<guiPage&>(gui->findPage(QStringList(QString("par%1").
						     arg(QString('A'+par-1)))));
  QList<guiButton *> buttons = pg.allButtons();
  QList<guiButton *>::iterator but_i = buttons.begin();
  for (QStringList::const_iterator i=values.begin();
       i!=values.end(); ++i) {
    QString val = *i;
    Dbg() << "Listing value " << val << " for prog " << prog << " par " << par;
    if (but_i!=buttons.end()) {
      (*but_i)->setValue(val);
      (*but_i)->setVisible(true);
      but_i++;
    } else {
      dbg("Note: I tried to add value '%s', but there is no button.",
	  qPrintable(val));
    }
  }
  while (but_i != buttons.end()) {
    (*but_i)->setVisible((*but_i)->isCustom());
    but_i++;
  }    
  dbg("  VideoGUI::populateParValues done");
}

void VideoGUI::ensureGUI(guiRoot *gui, ParamTree *ptree) {
  dbg("VideoGUI::ensureGUI");
  guiPage &vidp = gui->findPage("stimVideo");
  if (!populatedprognames)
    populateProgNames(&vidp);
  Param &progp = ptree->find("stimVideo/prog");
  QString prog = progp.toString();
  if (vprog.progname2num(prog)<0) {
    dbg("VideoGUI::ensureReady: I don't have a suitable program set.");
    if (vprog.progNames().empty()) {
      dbg("... but I don't have any names available, so too bad.");
    } else {
      prog = vprog.progNames().begin().value();
      progp.set(prog);
      gui->findButton("stimVideo/prog").setValue(prog);
    }
  }

  int pno = vprog.progname2num(prog);
  populateParNames(&vidp, pno);
  for (int k=1; k<=VideoProg::MAXPAR; k++)
    populateParValues(&vidp, pno, k);
  dbg("  VideoGUI::ensureGUI done");
}

void VideoGUI::populateParNames(guiPage *gui, int prog) {
  dbg("VideoGUI::populateParNames");
  
  QStringList const &parnames = vprog.parNames(prog);
  QStringList const &parunits = vprog.parUnits(prog);
  for (int k=1; k<=VideoProg::MAXPAR; k++) {
    QString name = k<=parnames.size() ? parnames[k-1] : "";
    QString unit = k<=parunits.size() ? parunits[k-1] : "";
    QString label = name;
    if (unit!="")
      label += " (" + unit + ")";
    label+=":\n%1";
    guiButton *b = gui->findpButton(QStringList(QString("par%1")
						.arg(QString('A'+k-1))));
    if (b) {
      if (name=="") {
	b->setEnabled(false);
	b->setFormat("n/a");
      } else {
	b->setEnabled(true);
	b->setFormat(label);
      }
    } else {
      dbg("Note: I tried to name parameter #%i '%s', but there is no button.",
	  k,qPrintable(label));
    }
  }
  dbg("  VideoGUI::populateParNames done");
}
