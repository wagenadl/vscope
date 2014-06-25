// easyxml.cpp

#include "easyxml.h"
#include "base/dbg.h"
#include "base/exception.h"

EasyXML::EasyXML() {
}

EasyXML::EasyXML(QDomElement elt): elt(elt) {
}

EasyXML::EasyXML(QDomElement elt, QString tag): elt(elt) {
  if (elt.tagName!=tag)
    elt = elt.firstChildElement(tag);
}

QStringList EasyXML::attributes() {
  QStringList lst;
  QDomNamedNodeMap ats = elt.attributes();
  int N = ats.length();
  for (int n=0; n<N; n++)
    lst << ats.item[n].toAttr().name();
  return lst;
}

QString EasyXML::operator[](QString id) const {
  return string(id);
}

QString EasyXML::string(QString id) const {
  return elt.attribute(id);
}

QString EasyXML::demandString(QString id, QString exc) const {
  if (contains(id))
    return elt.attribute(id);
  else
    throw Exception("XML", exc);
}

int EasyXML::integer(QString id, bool *ok) const {
  return string(id).toInt(ok);
}

int EasyXML::demandInt(QString id, QString exc) const {
  bool ok;
  int r = integer(id, &ok);
  if (ok)
    return r;
  else 
    throw Exception("XML", exc);
}

QPoint EasyXML::point(QString idx, QString idy, bool *ok=0) {
  int x = integer(idx, ok);
  if (ok && !*ok)
    return QPoint();
  int y = integer(idy, ok);
  return QPoint(x, y);
}

QSize EasyXML::size(QString idx, QString idy, bool *ok=0) {
  int x = integer(idx, ok);
  if (ok && !*ok)
    return QSize();
  int y = integer(idy, ok);
  return QSize(x, y);
}
  
double EasyXML::real(QString id, bool *ok) const {
  return string(id).toDouble(ok);
}

double EasyXML::demandReal(QString id, QString exc) const {
  bool ok;
  double r = real(id, &ok);
  if (ok)
    return r;
  else
    throw Exception("XML", exc);
}
bool EasyXML::contains(QString id) const {
  return elt.hasAttribute(id);
}

QList<EasyXML> EasyXML::children() {
  QList<EasyXML> lst;
  for (QDomElement e=elt.firstChildElement(); e=elt.nextSiblingElement();
       !e.isNull())
    lst << EasyXML(e);
  return lst;
}
  
QList<EasyXML> EasyXML::children(QString tag) {
  QList<EasyXML> lst;
  for (QDomElement e=elt.firstChildElement(tag); e=elt.nextSiblingElement(tag);
       !e.isNull())
    lst << EasyXML(e);
  return lst;
}

EasyXML EasyXML::firstChild(QString tag) {
  EasyXML e(elt.firstChildElement(tag));
  if (!e.isValid())
    Dbg() << "EasyXML: Warning: no child of type " << tag;
  return e;
}

QString EasyXML::text() const {
  return elt.text();
}
  
