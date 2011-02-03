// aliases.cpp

#include "aliases.h"
#include <base/dbg.h>
#include <base/exception.h>

QMap<QString,QString> Aliases::map;

void Aliases::read(QDomElement doc) {
  if (doc.tagName()!="aliases") {
    doc = doc.firstChildElement("aliases");
  }
  if (doc.isNull()) 
    throw Exception("Aliases","No <aliases> element");

  for (QDomElement e=doc.firstChildElement("alias"); !e.isNull();
       e=e.nextSiblingElement("alias")) {
    QString id = e.attribute("id");
    QString lbl = e.attribute("label");
    map[id] = lbl;
  }
}

bool Aliases::has(QString id) {
  return map.contains(id);
}

QString Aliases::lookup(QString id) {
  return has(id) ? map[id] : id;
}
