// xmlgui.cpp

#include <gui/xmlgui.h>
#include <gui/guigeom.h>
#include <gui/xmlpage.h>
#include <gui/xmlbutton.h>
#include <QWidget>
#include <xml/paramtree.h>
#include <QRect>
#include <base/exception.h>

xmlGui::xmlGui(QWidget *parent,
	       ParamTree *ptree_, QDomElement doc) {
  geom_ = new guiGeom(doc.firstChildElement("geometry"));
  ptree = ptree_;
  root = 0; // let xmlPages know that Master doesn't have a root yet
  root = new xmlPage(parent, ptree,
		     doc.firstChildElement("page"),
		     this,
		     "",
		     QRect(geom_->x0,geom_->y0,geom_->w,geom_->h));
  root->show();
}

xmlGui::~xmlGui() {
  delete geom_;
  delete root;
}

xmlButton *xmlGui::findpButton(QString path) {
  return root->findpButton(path.split(QChar('/')));
}

xmlButton &xmlGui::findButton(QString path) {
  return root->findButton(path.split(QChar('/')));
}

xmlPage *xmlGui::findpPage(QString path) {
  return root->findpPage(path.split(QChar('/')));
}

xmlPage &xmlGui::findPage(QString path) {
  return root->findPage(path.split(QChar('/')));
}

Param const *xmlGui::findpParam(QString path) {
  return ptree->findp(path);
}

Param const &xmlGui::findParam(QString path) {
  return ptree->find(path);
}

Param *xmlGui::accesspParam(QString path) {
  return ptree->findp(path);
}

Param &xmlGui::accessParam(QString path) {
  return ptree->find(path);
}

void xmlGui::open() {
  root->open();
}

void xmlGui::close() {
  root->close();
}

QString xmlGui::pathToLocal(QString path) const {
  return root->pathToLocal(path);
}

QString xmlGui::pathToGlobal(QString path) const {
  return root->pathToGlobal(path);
} 
QString xmlGui::pathInstantiate(QString path) const {
  return root->pathInstantiate(path);
}

QString xmlGui::pathDeinstantiate(QString path) const {
  return root->pathDeinstantiate(path);
}

void xmlGui::setCustom(QString path, int cno, QString newval) {
  Param &par = ptree->find(QString("custom/%1-%2").arg(path).arg(cno));
  QString olds = par.toString();
  par.set(newval);
  QString news = par.toString();
  if (news!=olds)
    emit customValueChanged(path,cno, news);
}

void xmlGui::setParam(QString path, QString newval) {
  if (!canSetParam(path,newval))
    throw Exception("xmlGui",QString("Cannot set parameter %1 to %s").arg(path).arg(newval));
  Param &par = ptree->find(path);
  QString olds = par.toString();
  par.set(newval);
  QString news = par.toString();
  if (olds!=news)
    emit paramChanged(path,news);
}

bool xmlGui::canSetParam(QString path, QString newval) {
  Param &p = ptree->find(path);
  Param q(p); // make a copy
  try {
    q.set(newval); // can we set it?
  } catch(Exception) {
    return false;
  }
  return true;
}
