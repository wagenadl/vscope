// enumerator.cpp

#include <xml/enumerator.h>
#include <base/exception.h>

#include <QStringList>

#include <iostream>
using namespace std;

Enumerator::Enumerator(QDomElement def) {
  if (def.hasAttribute("id"))
    myname = def.attribute("id");
  else
    myname = "-";
  largestValue=-1000;
  smallestValue=1000;
  bool first=true;
  for (QDomNode n=def.firstChild(); !n.isNull(); n=n.nextSibling()) {
    QDomElement e=n.toElement();
    if (!e.isNull()) {
      if (e.tagName()=="item") {
	if (e.hasAttribute("tag") && e.hasAttribute("value")) {
	  QString tag = e.attribute("tag");
	  int value = e.attribute("value").toInt(0,0);
	  values[tag] = value;
	  tags[value] = tag;
	  if (first || value<smallestValue)
	    smallestValue = value;
	  if (first || value>largestValue)
	    largestValue = value;
	  first = false;
	} else {
	  cerr << "Warning: enum '" << qPrintable(myname) << "' has bad item def" << endl;
	}
      }
    }
  }
  enums[myname] = this;
}

Enumerator::Enumerator() {
  myname = "-";
  enums[myname] = this;
}

int Enumerator::lookup(QString s) const {
  if (values.contains(s))
    return values[s];
  else
    throw Exception("Enumerator ", myname + " does not define '" + s + "'");
}

bool Enumerator::has(QString s) const {
 return values.contains(s);
}

bool Enumerator::has(int n) const {
  return tags.contains(n);
}

QString Enumerator::reverseLookup(int n) const {
  if (tags.contains(n))
    return tags[n];
  else
    throw Exception("Enumerator", myname + " does not contain " + QString::number(n));
}

Enumerator::~Enumerator() {
  enums.remove(myname);
}

QStringList Enumerator::getAllTags() const {
  return QStringList(values.keys());
}

QMap<QString,Enumerator *> Enumerator::enums;

Enumerator *Enumerator::find(QString e) {
  if (enums.contains(e))
    return enums[e];
  else
    throw Exception("Enumerator",QString("There is no enumerator named '") + e + "'");
}

void Enumerator::readAll(QDomElement doc) {
  if (doc.tagName()!="enums")
    doc = doc.firstChildElement("enums");
  if (doc.isNull())
    throw Exception("Enumerator","Expected <enums> element","readAll");
  for (QDomElement e=doc.firstChildElement("enum");
       !e.isNull(); e=e.nextSiblingElement("enum"))
    new Enumerator(e);
}
