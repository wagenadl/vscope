// base/xml.h - This file is part of VScope.
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

// xml.h

#ifndef XML_H

#define XML_H

#include <QDomElement>
#include <QDomDocument>

class XML {
  /*:C XML
   *:D Represents an XML document
   */
public:
  XML(XML const &xml);
  XML(QString fn);
  /*:F constructor
   *:D Loads an XML document from a file.
   *:E Throws an exception if the file cannot be read or contains invalid xml.
   */
  XML(int, QString typ);
  /*:F constructor
   *:D Constructs a new document of given type.
   *:N The first argument is ignored. Pass zero.
   */
  QDomElement append(QString tag);
  /*:F append
   *:D Creates an element with given tag at top level.
   */
  QDomElement append(QString tag, QDomElement parent);
  /*:F append
   *:D Creates an element with given tag inside the given parent.
   */
  QDomElement find(QString tag);
  /*:F find
   *:D Returns the first element with the given tag at top level.
   *:N Returns a null element if there is no such element.
   */
  void read(QString fn);
  /*:F read
   *:D Replaces our contents with the contents of a file.
   *:E Throws an exception if the file cannot be read or contains invalid xml.
   */
  void write(QString fn);
  /*:F write
   *:D Writes our contents to a file.
   *:E Throws an exception if the file cannot be written.
   */
  QDomElement root();
  /*:F root
   *:D Returns our document element.
   */
private:
  QDomDocument doc;
};

#endif
