// settingsfile.cpp

#include "settingsfile.h"

#include <xml/connections.h>
#include <base/xml.h>

#include <QDateTime>

void SettingsFile::save(QString fn, ParamTree const *src) {
  XML xml(0,"vsdscopeSettings");
  src->write(xml.root());
  Connections::writeXML(xml.root());
  QDomElement e=xml.append("date");
  e.setAttribute("modified",QDateTime::currentDateTime().toString());
  xml.write(fn);
}

void SettingsFile::load(QString fn, ParamTree *dst) {
  XML xml(fn);
  dst->read(xml.root());
  Connections::readXML(xml.root());
} 
