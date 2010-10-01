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
