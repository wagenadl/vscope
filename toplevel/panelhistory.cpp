// panelhistory.cpp

#include "panelhistory.h"
#include "globals.h"
#include "vscopegui.h"
#include <xml/paramtree.h>
#include <gui/xmlbutton.h>
#include <base/dbg.h>

#include <toplevel/mgauto.h>
#include <toplevel/vsdtraces.h>
#include <toplevel/coherence.h>
#include <toplevel/cohgraph.h>
#include <gfx/roiimage.h>

PanelHistory::PanelHistory() {
  busy = false;
  nButtons = countButtons();
  oldLeft = 0; // Globals::banner;
  oldRight = 0;
  //  makeButtons();
}

QString PanelHistory::whatName(QString where, int n) {
  return QString("panelHistory/what%1%2").arg(where).arg(n);
}

QString PanelHistory::prioName(QString where, int n) {
  return QString("panelHistory/prio%1%2").arg(where).arg(n);
}

int PanelHistory::countButtons() {
  int n = 0;
  while (Globals::ptree->findp(whatName("Left",n+1)))
    n++;
  return n;
}

void PanelHistory::makeButtons() {
  makeButtons("Left");
  makeButtons("Right");
}

void PanelHistory::makeButtons(QString where) {
  //dbg("makebuttons(%s) nbut=%i",qPrintable(where),nButtons);
  QString what = Globals::ptree->find("panel" + where).toString();
  //  dbg("makebuttons. what=%s",qPrintable(what));
  for (int k=1; k<=nButtons; k++) {
    setShown(where,k,"");
    if (shown(where,k) == what) 
      Globals::gui->findButton(QString("panel%1%2").arg(where).arg(k))
	.setSelected(true);
  }
}

void PanelHistory::newSelection(QString const &where) {
  if (busy) {
    // dbg("panelhistory:newsel: recursion quelched");
    return;
  }
  busy = true;
  QString what = Globals::ptree->find("panel" + where).toString();
  QString label = niceLabel(where,what);
  // dbg("PanelHistory: changing '%s' to '%s' which is called '%s'",
  //     qPrintable(where),qPrintable(what),qPrintable(label));
  int n = find(where,what);
  if (n==0) {
    n = weakest(where);
    setShown(where,n,what);
    // dbg("  Replacing oldest, i.e., #%i",n);
  } else {
    // dbg("  Found at #%i",n);
  }
  reprioritize(where,n);
  // dbg("  new contents: 1: %s (%g). 2: %s (%g). 3: %s (%g)",
  //     qPrintable(shown(where,1)),prio(where,1),
  //     qPrintable(shown(where,2)),prio(where,2),
  //     qPrintable(shown(where,3)),prio(where,3));
  open(where, what);
  busy = false;
}

void PanelHistory::oldSelection(QString const &p) {
  if (busy) {
    // dbg("panelhistory:oldsel: recursion quelched");
    return;
  }
  busy = true;
  QString where = p.contains("Left") ? "Left" : "Right";
  int n = p.right(1).toInt();
  //dbg("PanelHistory: selecting #%i for '%s'",n,qPrintable(where));
  QString pname = whatName(where,n);
  //dbg("  looking for %s",qPrintable(pname));
  QString what = Globals::ptree->find(pname).toString();
  // dbg("PanelHistory: selecting #%i for '%s': '%s'",
  //     n,qPrintable(where),qPrintable(what));
  reprioritize(where,n);
  // dbg("  new contents: 1: %s (%g). 2: %s (%g). 3: %s (%g)",
  //     qPrintable(shown(where,1)),prio(where,1),
  //     qPrintable(shown(where,2)),prio(where,2),
  //     qPrintable(shown(where,3)),prio(where,3));
  open(where, what);
  busy=false;
}

void PanelHistory::reprioritize(QString where, int n) {
  for (int k=1; k<=nButtons; k++) {
    double p = prio(where,k);
    if (k==n)
      p+=1;
    else
      p*=.8;
    setPrio(where,k,p);
  }
  Globals::gui->findButton(QString("panel%1%2").arg(where).arg(n))
    .setSelected(true);
}

double PanelHistory::prio(QString where, int n) {
  return Globals::ptree->find(prioName(where,n)).toDouble();
}

QString PanelHistory::shown(QString where, int n) {
  return Globals::ptree->find(whatName(where,n)).toString();
}

int PanelHistory::find(QString where, QString what) {
  for (int k=1; k<=nButtons; k++)
    if (shown(where,k)==what)
      return k;
  return 0;
}

void PanelHistory::setPrio(QString where, int n, double prio) {
  Globals::ptree->find(prioName(where,n)).setDouble(prio);
}

QString PanelHistory::niceLabel(QString where, QString what) {
  xmlButton const *butp = Globals::gui->findpButton(QString("panel%1/%2")
						   .arg(where).arg(what));
  return butp ? butp->text() : what;
}

void PanelHistory::setShown(QString where, int n, QString what) {
  // dbg("panelhistory:setshown(%s,%i,%s)",qPrintable(where),n,qPrintable(what));
  if (what.isEmpty())
    what = Globals::ptree->find(whatName(where,n)).toString();
  else
    Globals::ptree->find(whatName(where,n)).set(what);
  // dbg("  -> what=%s",qPrintable(what));
  xmlButton &but = Globals::gui->findButton(QString("panel%1%2")
					    .arg(where).arg(n));
  // dbg("  found button");
  but.setFormat(niceLabel(where,what));
  // dbg("  setformat");
  but.setValue(what);
  // dbg("  setvalue");
}

int PanelHistory::weakest(QString where) {
  int best=nButtons;
  double p0=1000000;
  for (int k=1; k<=nButtons; k++) {
    double p = prio(where,k);
    if (p<p0) {
      best=k;
      p0=p;
    }
  }
  return best;
}

void PanelHistory::open(QString where, QString what) {
  // dbg("PanelHistory: open %s at %s",qPrintable(what),qPrintable(where));
  Globals::gui->findButton(QString("panel%1/%2")
			   .arg(where).arg(what))
    .setSelected(true);

  QWidget *place = (where=="Left") ? Globals::leftplace : Globals::rightplace;
  QWidget *&old = (where=="Left") ? oldLeft : oldRight;
  QWidget *&otherOld = (where=="Left") ? oldRight : oldLeft;
  QWidget *child = 0;
  if (what=="VSDCc")
    child = Globals::coumarinw;
  else if (what=="VSDOx")
    child = Globals::oxonolw;
  else if (what=="EphysInt")
    child = Globals::mgintra;
  else if (what=="EphysExt")
    child = Globals::mgextra;
  else if (what=="Stimuli")
    child = Globals::mgstim;
  else if (what=="VSDTraces")
    child = Globals::vsdtraces;
  else if (what=="Coherence")
    child = Globals::coherence;
  else if (what=="CohGraph")
    child = Globals::cohgraph;
  if (child) {
    if (old && old->parent()==place)
      old->hide();
    old=0;
    child->setParent(place);
    child->show();
    if (child==otherOld) {
      // we're stealing
      QString otherWhere = (where=="Left") ? "Right" : "Left";
      for (int k=1; k<=nButtons; k++) {
	QString pname = whatName(otherWhere,k);
	QString otherWhat = Globals::ptree->find(pname).toString();
	if (otherWhat!=what) {
	  busy=false;
	  oldSelection(pname);
	  busy=true;
	  break;
	}
      }
    }
    old = child;
  } else {
    // dbg("PanelHistory::open: Don't know about %s",qPrintable(what));
  }
}

/*
    if (p=="leftCam1")
      Globals::coumarinw->show();
    else if (p=="rightCam2")
      Globals::oxonolw->show();
    else if (p=="rightVSD")
      Globals::vsdtraces->show();
    else if (p=="leftIntra")
      Globals::mgintra->show();
    else if (p=="leftStim")
      Globals::mgstim->show();
    else if (p=="rightExtra")
      Globals::mgextra->show();
*/
