// xmlgui.cpp

#include <gui/guiroot.h>
#include <gui/guigeom.h>
#include <gui/guipage.h>
#include <gui/guibutton.h>
#include <QWidget>
#include <xml/paramtree.h>
#include <QRect>
#include <base/exception.h>
#include <base/dbg.h>

guiRoot::guiRoot(QWidget *parent,
	       ParamTree *ptree_, QDomElement doc) {
  geom_ = new guiGeom(doc.firstChildElement("geometry"));
  ptree = ptree_;
  readOnly = false;
  root = 0; // let guiPages know that Master doesn't have a root yet
  QDomElement elt = doc.firstChildElement("page");
  root = new guiPage(parent, ptree,
		     "",
		     this,
		     QRect(geom_->x0,geom_->y0,geom_->w,geom_->h));
  root->setup(elt);
  root->show();
  findButton("acquisition/unlock").hide();
}

guiRoot::~guiRoot() {
  delete geom_;
  delete root;
}

void guiRoot::open() {
  root->open();
}

void guiRoot::close() {
  root->close();
}

QString guiRoot::pathToLocal(QString path) const {
  return root->pathToLocal(path);
}

QString guiRoot::pathToGlobal(QString path) const {
  return root->pathToGlobal(path);
} 
QString guiRoot::pathInstantiate(QString path) const {
  return root->pathInstantiate(path);
}

QString guiRoot::pathDeinstantiate(QString path) const {
  return root->pathDeinstantiate(path);
}

void guiRoot::setCustom(QString path, int cno, QString newval) {
  Param &par = ptree->find(QString("custom/%1-%2").arg(path).arg(cno));
  QString olds = par.toString();
  par.set(newval);
  QString news = par.toString();
  if (news!=olds && !readOnly)
    emit customValueChanged(path, cno, news);
}

void guiRoot::setParam(QString path, QString newval) {
  if (!canSetParam(path,newval))
    throw Exception("guiRoot",QString("Cannot set parameter %1 to %s").arg(path).arg(newval));
  Param &par = ptree->find(path);
  QString olds = par.toString();
  par.set(newval);
  QString news = par.toString();
  if (olds!=news && !readOnly)
    emit paramChanged(path, news);
}

bool guiRoot::canSetParam(QString path, QString newval) {
  Param &p = ptree->find(path);
  Param q(p); // make a copy
  try {
    q.set(newval); // can we set it?
  } catch(Exception) {
    return false;
  }
  return true;
}

guiButton *guiRoot::findpButton(QString path) {
  return root->findpButton(path.split(QChar('/')));
}

guiButton &guiRoot::findButton(QString path) {
  return root->findButton(path.split(QChar('/')));
}

guiPage *guiRoot::findpPage(QString path) {
  return root->findpPage(path.split(QChar('/')));
}

guiPage &guiRoot::findPage(QString path) {
  return root->findPage(path.split(QChar('/')));
}

void guiRoot::setTree(ParamTree *pt) {
  Dbg() << "guiRoot::setTree " << pt;
  ptree = pt;
  root->reTree(pt);
}

void guiRoot::setReadOnly(bool ro) {
  readOnly = ro;
  root->setReadOnly(ro);
  if (ro)
    findButton("acquisition/unlock").show();
  else
    findButton("acquisition/unlock").hide();    
}
