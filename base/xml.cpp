// xml.cpp

#include "xml.h"
#include <base/exception.h>
#include <QFile>
#include <QTextStream>

XML::XML(XML const &xml) {
  doc = xml.doc;
}

XML::XML(QString fn) {
  read(fn);
}

XML::XML(int, QString typ) {
  doc = QDomDocument(typ);
  doc.appendChild(doc.createElement(typ));
}

void XML::read(QString fn) {
  QFile f(fn);
  if (!f.open(QIODevice::ReadOnly))
    throw Exception("XML","Cannot open '" + fn +"' for reading","read");
  QString errmsg;
  int l,c;
  if (!doc.setContent(&f,&errmsg,&l,&c))
    throw Exception("XML",
		    QString("Parse error in '%1': %2 at line %3 (c. %4)")
		    .arg(fn).arg(errmsg).arg(l).arg(c),"read");
  f.close();
}

void XML::write(QString fn) {
  QFile f(fn);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    throw Exception("XML",QString("Cannot open '%1' for writing").arg(fn),
		    "write");
  QString xml = doc.toString(); 
  { QTextStream out(&f);
    out << xml;
  }
  f.close();
}

QDomElement XML::root() {
  return doc.documentElement();
}

QDomElement XML::append(QString tag) {
  return append(tag,root());
}

QDomElement XML::append(QString tag, QDomElement parent) {
  QDomElement e = doc.createElement(tag);
  parent.appendChild(e);
  return e;
}

QDomElement XML::find(QString tag) {
  return root().firstChildElement(tag);
}
