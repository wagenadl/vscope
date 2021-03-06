// xml/enumerator.cpp - This file is part of VScope.
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

// enumerator.cpp

#include <xml/enumerator.h>
#include <base/exception.h>
#include <base/dbg.h>
#include <QSet>
#include <QStringList>

#include <iostream>
using namespace std;

Enumerator::Enumerator(QDomElement def) {
  if (def.hasAttribute("id"))
    myname = def.attribute("id");
  else
    myname = "-";
  enums[myname] = this;
  reset();
  add(def);
}

Enumerator::Enumerator(QString name) {
  myname = name;
  enums[myname] = this;
  reset();
}

void Enumerator::reset() {
  val2tag.clear();
  tag2val.clear();
  orderedTags.clear();
}

void Enumerator::add(QDomElement def) {
  if (def.tagName()=="item") {
    if (def.hasAttribute("tag")) {
	if (def.hasAttribute("value"))
	  add(def.attribute("tag"), def.attribute("value").toInt());
	else
	  add(def.attribute("tag"));
      }
  } else {
    for (QDomElement e=def.firstChildElement("item"); !e.isNull();
	 e=e.nextSiblingElement("item")) 
      add(e);
  }
}

void Enumerator::add(QString s) {
  if (has(s))
    return;
  int n = isEmpty() ? 0 : largestVal + 1;
  add(s, n);
}

void Enumerator::add(QString s, int n) {
  if (has(s))
    remove(s);
  if (isEmpty() || n<smallestVal)
    smallestVal = n;
  if (isEmpty() || n>largestVal)
    largestVal = n;
  tag2val[s] = n;
  val2tag[n] = s;
  orderedTags.append(s);
  //  Dbg() << "Added " << s << ": " << n << " to enum " << myname;
}

void Enumerator::remove(QString s) {
  if (!has(s))
    return;
  val2tag.remove(tag2val[s]);
  tag2val.remove(s);
  orderedTags.removeAll(s);
}

int Enumerator::lookup(QString s, int dflt) const {
  if (has(s))
    return tag2val[s];
  else
    return dflt;
}

int Enumerator::lookup(QString s) const {
  if (has(s))
    return tag2val[s];
  else
    throw Exception("Enumerator ", myname + " does not define '" + s + "'");
}

bool Enumerator::has(QString s) const {
 return tag2val.contains(s);
}

bool Enumerator::has(int n) const {
  return val2tag.contains(n);
}

QString Enumerator::reverseLookup(int n) const {
  if (val2tag.contains(n))
    return val2tag[n];
  else
    throw Exception("Enumerator", myname + " does not contain " + QString::number(n));
}

Enumerator::~Enumerator() {
  enums.remove(myname);
}

QStringList Enumerator::getAllTags() const {
  return orderedTags;
}

QStringList Enumerator::getNonnegativeTags() const {
  QStringList tags;
  foreach (QString t, orderedTags)
    if (tag2val[t]>=0)
      tags.append(t);
  return tags;
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

void Enumerator::replace(QSet<QString> const &newids) {
  QSet<QString> oldids;
  foreach (QString id, getNonnegativeTags())
    oldids.insert(id);
  foreach (QString id, oldids)
    if (!newids.contains(id))
      remove(id);
  foreach (QString id, newids)
    if (!oldids.contains(id))
      add(id);
}
