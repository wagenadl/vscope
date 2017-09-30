// xml/xmlfind.cpp - This file is part of VScope.
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

// xmlfind.cpp

#include <xml/xmlfind.h>
#include <base/exception.h>
#include <QStringList>
#include <QRegExp>
#include <QDomNodeList>
#include <stdio.h>

QDomElement xmlFindParam(QDomElement doc, QString path) {
  QStringList p = path.split("/");
  QDomElement domain = doc;
  while (p.size()>=2) {
    // not yet at bottom level, let's go deeper
    QString id = p.first();
    /* We are looking for an element of type category or array with this id.
       This must occur at this level of the xml tree, except if we are
       still outside of the <vsdscopeParameters> block, in which case we must
       enter.
    */
    bool stay=false;
    bool found=false;
    for (QDomElement e=domain.firstChildElement(); !e.isNull(); e=e.nextSiblingElement()) {
      if (e.tagName()=="vsdscopeDefs" || e.tagName()=="params") {
	domain = e;
	stay = true;
	found = true;
	break;
      } else if (e.tagName()=="category" || e.tagName()=="array") {
	if (e.attribute("id")==id) {
	  domain = e;
	  found = true;
	  break;
	}
      }
    }
    if (!stay)
      p.removeFirst();
    if (!found)
      throw Exception("Param",QString("Path ") + path + " does not lead to a <param>");
  }

  // so now we need to find our param.
  for (QDomElement e=domain.firstChildElement(); !e.isNull(); e=e.nextSiblingElement()) 
    if (e.tagName()=="param") 
      if (e.attribute("id")==p.first())
	return e;
  throw Exception("Param",QString("Path ") + path + " does not lead to a <param>");
}

QDomElement xmlFindPval(QDomElement doc, QString path,
			bool autoCreate, bool *exist) {
  QStringList p;
  QStringList typ;
  QRegExp sep("[/:]");
  bool nextiselt=false;
  //fprintf(stderr,"xmlFindPval '%s' on <%s> '%s'\n",qPrintable(path),
  //        qPrintable(doc.tagName()),qPrintable(doc.attribute("id")));
  for (int splidx=path.indexOf(sep); splidx>=0; splidx=path.indexOf(sep)) {
    // this is not yet the deepest level
    p.push_back(path.left(splidx));
    if (path[splidx]==':') {
      typ.push_back("array");
      nextiselt=true;
    } else {
      typ.push_back(nextiselt ? "elt":"category");
      nextiselt=false;
    }
    path = path.mid(splidx+1);
  }
  p.push_back(path);
  typ.push_back("pval");

  //fprintf(stderr,"Parsed request as:\n");
  //for (int i=0; i<p.size(); i++)
  //  fprintf(stderr,"  level %i: <%s> '%s'\n",
  //          i,qPrintable(typ[i]),qPrintable(p[i]));

  QDomElement domain = doc;
  while (!p.isEmpty()) {
    QString id = p.first();
    QString type = typ.first();
    /* We are looking for an element of this type with this id.
       This must occur at the current level of the xml tree, except if we are
       still outside of the <settings> block, in which case we must
       enter.
    */
    bool stay=false;
    bool found=false;
    for (QDomElement e=domain.firstChildElement(); !e.isNull(); e=e.nextSiblingElement()) {
      //fprintf(stderr,"Levels left: %i. Scanning <%s> '%s' for <%s> '%s'\n",
      //        p.size(),qPrintable(e.tagName()),qPrintable(e.attribute("id")),
      //        qPrintable(type),qPrintable(id));
      if (e.tagName()=="vsdscopeSettings" || e.tagName()=="settings") {
	domain = e;
	stay = true;
	found = true;
	break;
      } else if (e.tagName()==type) {
	if (e.attribute("id")==id) {
	  domain = e;
	  found = true;
	  break;
	}
      }
    }
    //fprintf(stderr,"Found? %i Stay? %i\n",found,stay);
    if (!found) {
      if (autoCreate) {
	// we must create a new element
	QDomElement e = domain.ownerDocument().createElement(type);
	e.setAttribute("id",id);
	domain.appendChild(e);
	domain=e;
      } else {
	if (exist)
	  *exist=false;
	return QDomElement();
      }
    }
    if (!stay) {
      p.removeFirst();
      typ.removeFirst();
    }
  }
  //fprintf(stderr,"Found: <%s> '%s'\n",
  //        qPrintable(domain.tagName()),qPrintable(domain.attribute("id")));
  if (exist)
    *exist=true;
  return domain;
}

QDomElement xmlFindElement(QDomElement doc, QString tagname) {
  if (doc.tagName()==tagname)
    return doc;
  
  QDomNodeList l = doc.elementsByTagName(tagname);
  if (l.isEmpty())
    throw Exception("xmlFindElement",QString("No '") + tagname + "' element found.");
  if (l.size()>1)
    fprintf(stderr,"Warning: xmlFindElement: Multiple '%s' elements found. Using first.\n",
	    qPrintable(tagname));
  return l.item(0).toElement();
}
