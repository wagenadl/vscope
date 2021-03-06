// toplevel/panelhistory.cpp - This file is part of VScope.
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

// panelhistory.cpp

#include "panelhistory.h"
#include "vscopegui.h"
#include "globals.h"
#include <xml/paramtree.h>
#include <gui/guibutton.h>
#include <base/dbg.h>
#include <QDir>
#include <toplevel/mgauto.h>
#include <toplevel/vsdtraces.h>
#include <gfx/cohmaps.h>
#include <gfx/cohgraph.h>
#include <toplevel/exptlog.h>
#include <gfx/roiimages.h>
#include <xml/enumerator.h>

PanelHistory::PanelHistory(ParamTree *pt, guiRoot *gui): ptree(pt), gui(gui) {
  busy = false;
  nButtons = countButtons();
  oldLeft = 0;
  oldRight = 0;
  lasttrial="";
  figno=0;
}

PanelHistory::~PanelHistory() {
}

void PanelHistory::setTree(ParamTree *pt) {
  ptree = pt;
}

Param const &PanelHistory::whatPar(QString where, int n) const {
  QString name = QString("panelHistory/what%1%2").arg(where).arg(n);
  return ptree->find(name);
}

Param &PanelHistory::whatPar(QString where, int n) {
  QString name = QString("panelHistory/what%1%2").arg(where).arg(n);
  return ptree->find(name);
}

Param const &PanelHistory::prioPar(QString where, int n) const {
  QString name = QString("panelHistory/prio%1%2").arg(where).arg(n);
  return ptree->find(name);
}

Param &PanelHistory::prioPar(QString where, int n) {
  QString name = QString("panelHistory/prio%1%2").arg(where).arg(n);
  return ptree->find(name);
}

guiButton &PanelHistory::barButton(QString where, int n) {
  QString name = QString("panel%1%2").arg(where).arg(n);
  return gui->findButton(name);
}

guiButton &PanelHistory::menuButton(QString where, QString what) {
  QString name = QString("panel%1/%2").arg(where).arg(what);
  return gui->findButton(name);
}

int PanelHistory::countButtons() const {
  int n = 0;
  while (ptree->findp(QString("panelHistory/what%1%2")
			       .arg("Left").arg(n+1)))
    n++;
  return n;
}

void PanelHistory::makeButtons() {
  makeButtons("Left");
  makeButtons("Right");
}

void PanelHistory::relabelAll() {
  relabel("Left");
  relabel("Right");
}

void PanelHistory::relabel(QString where) {
  QString what = ptree->find("panel" + where).toString();
  for (int k=1; k<=nButtons; k++) {
    Button &b = barButton(where,k);
    setItemAt(where,k,"");
    if (itemAt(where,k) == what) 
      b.setSelected(true);
    else
      b.setSelected(false); // ? do we need this?
  }
}

void PanelHistory::makeButtons(QString where) {
  QString what = ptree->find("panel" + where).toString();
  for (int k=1; k<=nButtons; k++) {
    Button &b = barButton(where,k);
    setItemAt(where,k,"");
    if (itemAt(where,k) == what) 
      b.setSelected(true);
    connect(&b, SIGNAL(doubleClicked(QString,QString)),
	    this, SLOT(doubleClicked(QString,QString)));
  }
}

void PanelHistory::newSelection(QString const &where) {
  //Dbg() << "PanelHistory::newSelection(" << where <<")";
  if (busy) {
    return;
  }
  busy = true;
  QString what = ptree->find("panel" + where).toString();
  QString label = niceLabel(where,what);
  int n = find(where,what);
  if (n==0) {
    n = weakest(where);
    setItemAt(where,n,what);
  } else {
    ;
  }
  reprioritize(where,n);
  open(where, what);
  busy = false;
}

QString PanelHistory::butNameToWhere(QString const &s) const {
  return s.contains("Left") ? "Left" : "Right";
}

int PanelHistory::butNameToNumber(QString const &s) const {
  return s.right(1).toInt();
}

QString PanelHistory::itemAt(QString const &s) const {
  QString where = butNameToWhere(s);
  int n = butNameToNumber(s);
  return itemAt(where, n);
}

void PanelHistory::oldSelection(QString const &p) {
  //Dbg() << "PanelHistory::oldSelection(" << p <<")";
  if (busy) {
    return;
  }
  busy = true;
  QString where = butNameToWhere(p);
  int n = butNameToNumber(p);
  QString what = itemAt(where, n);
  reprioritize(where,n);
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
  barButton(where,n).setSelected(true);
}

double PanelHistory::prio(QString where, int n) const {
  return prioPar(where,n).toDouble();
}

QString PanelHistory::itemAt(QString where, int n) const {
  return whatPar(where,n).toString();
}

int PanelHistory::find(QString where, QString what) const {
  for (int k=1; k<=nButtons; k++)
    if (itemAt(where,k)==what)
      return k;
  return 0;
}

void PanelHistory::setPrio(QString where, int n, double prio) {
  prioPar(where,n).setDouble(prio);
}

QString PanelHistory::niceLabel(QString where, QString what) {
  try {
    return menuButton(where, what).text();
  } catch (Exception const &) {
    return what;
  }      
}

void PanelHistory::setItemAt(QString where, int n, QString what) {
  if (what.isEmpty()) /* Special case for MakeButtons */
    what = itemAt(where, n);
  else
    whatPar(where,n).set(what);
  guiButton &but = barButton(where,n);
  but.setFormat(niceLabel(where,what));
  but.setValue(what);
}

int PanelHistory::weakest(QString where) const {
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

QWidget *PanelHistory::childWidget(QString what) {
  if (what.startsWith("CCD-")) {
    QString id = what.mid(4);
    if (Globals::ccdw->has(id))
      return Globals::ccdw->get(id);
    else
      return 0;
  } else if (what.startsWith("Coh-")) {
    QString id = what.mid(4);
    if (Globals::cohmaps->has(id))
      return Globals::cohmaps->get(id);
    else
      return 0;
  }  
  QMap<SHOWWHAT, QWidget *> m;
  m[SW_VSDTraces] = Globals::vsdtraces;
  m[SW_EphysInt] = Globals::mgintra;
  m[SW_EphysExt] = Globals::mgextra;
  m[SW_Stimuli] = Globals::mgstim;
  m[SW_CohGraph] = Globals::cohgraph;
  
  SHOWWHAT sw = (SHOWWHAT)Enumerator::find("SHOWWHAT")->lookup(what);
  if (m.contains(sw))
    return m[sw];

  return 0;
}
  

void PanelHistory::open(QString where, QString what) {
  menuButton(where,what).setSelected(true);
  QWidget *place = (where=="Left") ? Globals::leftplace : Globals::rightplace;
  QPointer<QWidget> &old = (where=="Left") ? oldLeft : oldRight;
  QPointer<QWidget> &otherOld = (where=="Left") ? oldRight : oldLeft;
  QWidget *child = childWidget(what);
  if (!child) {
    dbg("PanelHistory::open: Don't know about %s",qPrintable(what));
    return;
  }

  if (old && old->parent()==place)
    old->hide();
  old=0;
  child->setParent(place);
  child->show();
  if (child==otherOld) {
    // we're stealing
    QString otherWhere = (where=="Left") ? "Right" : "Left";
    for (int k=1; k<=nButtons; k++) {
      QString otherWhat = whatPar(otherWhere,k).toString();
      if (otherWhat!=what) {
	busy=false;
	oldSelection(QString("panel%1%2").arg(otherWhere).arg(k));
	busy=true;
	break;
      }
    }
  }
  old = child;
}

void PanelHistory::doubleClicked(QString id, QString) {
  QString what = itemAt(id);
  //Dbg() << "PanelHistory::doubleClicked("<<id<<","<<txt<<"): " << what;
  QWidget *child = childWidget(what);
  if (!child) {
    dbg("PanelHistory::doubleClicked: Don't know about %s",qPrintable(what));
    return;
  }

  QPixmap pixmap(child->size());
  child->render(&pixmap);

  QImage img = pixmap.toImage();
  QImage tst = img.scaled(QSize(4,4),Qt::IgnoreAspectRatio,
    Qt::SmoothTransformation)
    .convertToFormat(QImage::Format_RGB32);
  double gry = 0;
  for (int k=0; k<16; k++) 
    gry += (tst.bits()[4*k+0] + tst.bits()[4*k+1]
	    + tst.bits()[4*k+2])/3.0/256.0;
  //Dbg() << "gry: " << gry;
  if (gry<8)
    img.invertPixels();
  
  QString filePath = Globals::filePath();
  QString exptname = ptree->find("acquisition/exptname").toString();
  int trialno = ptree->find("acquisition/trialno").toInt();
  QString trialid = QString("%1").arg(trialno,int(3),int(10),QChar('0'));
  QString newtrial = filePath + "/" + exptname + "/" + trialid;
  if (newtrial==lasttrial)
    figno++;
  else
    figno=1;
  lasttrial = newtrial;
  QString path = filePath + "/" + exptname;
  QDir d; d.mkpath(path);
  QString leaf;
  QString fileName;
  while (true) {
    QString figId = QString("%1").arg(figno,int(2),int(10),QChar('0'));
    leaf = trialid + "-fig-" + figId + "-" + what + ".png";
    fileName = path + "/" + leaf;
    QFile f(fileName);
    if (!f.exists())
      break;
    figno++;
  };
  img.save(fileName);
  Globals::exptlog->addNote("Figure: " + leaf);
}
