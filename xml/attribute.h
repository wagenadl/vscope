// attribute.h

#ifndef ATTRIBUTE_H

#define ATTRIBUTE_H

#include <QDomElement>

extern QString xmlAttribute(QDomElement e, QString tag,
			    QString issuer, QString errmsg);
/*:F xmlAttribute
 *:D If the element 'e' has attribute 'tag', return its value, else throw
     an exception.
 *:A e: element to mine
     tag: attribute to look for
     issuer: caller's name for identifying exception
     errmsg: caller's message to the world in case of exception.
 *:R Value of attribute.
*/

extern QString xmlAttribute(QDomElement e, QString tag);
/*:F xmlAttribute
 *:D If the element 'e' has attribute 'tag', return its value, else return "".
 *:R Value of attribute or the empty string.
*/

#endif
