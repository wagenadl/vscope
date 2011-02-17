// colors.cpp

#include "colors.h"
#include <base/dbg.h>
#include <base/exception.h>

Colors *Colors::colors = 0;

Colors::Colors(QDomElement elt) {
  if (!colors)
    colors = this;
  colors->read(elt);
}

Colors::~Colors() {
  if (colors==this)
    colors=0;
}

bool Colors::has(QString name) {
  if (!colors)
    return false;
  return colors->map.contains(name);
}

QColor Colors::find(QString name) {
  if (!has(name))
    throw Exception("Colors","No color named " + name);;
  return QColor(colors->map[name]);
}

QColor Colors::find(QString name, QString dflt) {
  if (has(name))
    return find(name);
  else
    return QColor(dflt);
}

void Colors::read(QDomElement elt) {
  if (elt.tagName()!="colors") {
    elt = elt.firstChildElement("colors");
    if (elt.isNull()) {
      Dbg() << "Colors: No <colors> element found";
      return;
    }
  }
  for (QDomElement e=elt.firstChildElement("color");
       !e.isNull(); e=e.nextSiblingElement("color")) {
    QString id = e.attribute("id");
    QString val = e.attribute("value");
    if (id.isEmpty() || val.isEmpty())
      throw Exception("Colors",
		      "<color> element must have id and value attributes");
    map[id] = QColor(val);
  }
}

    
