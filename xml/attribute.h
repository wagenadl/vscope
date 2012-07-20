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

extern QString xmlAttribute(QDomElement e, QString tag, QString dflt="");
/*:F xmlAttribute
 *:D If the element 'e' has attribute 'tag', return its value, else return
     default value.
 *:R Value of attribute or the empty string.
*/

extern bool attributeTrue(QDomElement e, QString tag);
/*:F booleanAttribute
 *:D Returns true iff the attribute ATTR on xml element ELT represents
     true (i.e., if it is "yes", "on", "true", or "1").
 *:N Returns false in all other cases, including if TAG does not name an
     attribute or if the attribute has a nonsensical value.
*/

extern bool attributeFalse(QDomElement e, QString tag);
/*:F booleanAttribute
   *:D Returns true iff the attribute ATTR on xml element ELT represents
       false (i.e., if it is "no", "off", "false", or "0").
 *:N Returns false in all other cases, including if TAG does not name an
     attribute or if the attribute has a nonsensical value.
*/



#endif
