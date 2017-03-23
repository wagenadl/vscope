// base/xml.cpp - This file is part of VScope.
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
