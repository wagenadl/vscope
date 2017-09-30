// xml/xmlfind.h - This file is part of VScope.
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

// xmlfind.h

#ifndef XMLFIND_H

#define XMLFIND_H

#include <QDomElement>
#include <QString>

extern QDomElement xmlFindParam(QDomElement doc, QString path);
/*:F xmlFindParam
 *:D Locates a <param> element inside a larger document
     based on the given path.
 *:N An exception is thrown if the path is invalid or does not point to
     a <param>.
*/

extern QDomElement xmlFindPval(QDomElement doc, QString path,
			       bool autoCreate, bool *exist=0);
/*:F xmlFindPval
 *:D Locates a <pval> element inside a larger document
     based on the given path. It optionally creates a <pval> if none
     existed at the given path.
 *:N An exception is thrown if the path is invalid, but not if it does not
     point to a <pval>.
*/

extern QDomElement xmlFindElement(QDomElement doc, QString tagname);
/*:F findXmlElement
 *:D If doc is an element of type <tagname>, returns doc, otherwise
     searches inside doc for the first element of type <tagname> and
     returns that element.
 *:N If there is no such element, throws an exception.
*/

#endif
