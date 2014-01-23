// aliases.cpp

#include "aliases.h"
#include <base/dbg.h>
#include <base/exception.h>

QMap<QString, QString> Aliases::map;
QMap<QString, QString> Aliases::wildcarded;

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
    if (id.endsWith("*"))
      wildcarded[id.left(id.size()-1)] = lbl;
    else
      map[id] = lbl;
  }
}

bool Aliases::has(QString id) {
  if (map.contains(id))
    return true;
  foreach (QString k, wildcarded.keys()) 
    if (id.startsWith(k))
      return true;
  return false;
}

QString Aliases::lookup(QString id) {
  if (map.contains(id))
    return map[id];
  foreach (QString k, wildcarded.keys()) 
    if (id.startsWith(k))
      return wildcarded[k] + id.mid(k.size());
  return id;
}
