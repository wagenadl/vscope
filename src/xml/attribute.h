// xml/attribute.h - This file is part of VScope.
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
