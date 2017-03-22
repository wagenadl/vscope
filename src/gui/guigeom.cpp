// gui/guigeom.cpp - This file is part of VScope.
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

// guigeom.cpp

#include <gui/guigeom.h>
#include <xml/attribute.h>
#include <base/exception.h>

guiGeom::guiGeom(QDomElement elt) {
  bool ok;

  // top geometry
  x0 = xmlAttribute(elt,"x0","guiGeom","Cannot read x0").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read x0");

  y0 = xmlAttribute(elt,"y0","guiGeom","Cannot read y0").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read y0");

  w = xmlAttribute(elt,"w","guiGeom","Cannot read w").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read w");

  h = xmlAttribute(elt,"h","guiGeom","Cannot read h").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read h");

  // grid
  cols = xmlAttribute(elt,"cols","guiGeom","Cannot read cols").toDouble(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read cols");

  rows = xmlAttribute(elt,"rows","guiGeom","Cannot read rows").toDouble(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read rows");

  // menucaph
  menucaph = xmlAttribute(elt,"menucaph","guiGeom","Cannot read menucaph").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read menucaph");

  // margins
  leftx = xmlAttribute(elt,"leftx","guiGeom","Cannot read leftx").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read leftx");

  intx = xmlAttribute(elt,"intx","guiGeom","Cannot read intx").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read intx");

  rightx = xmlAttribute(elt,"rightx","guiGeom","Cannot read rightx").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read rightx");

  topy = xmlAttribute(elt,"topy","guiGeom","Cannot read topy").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read topy");

  inty = xmlAttribute(elt,"inty","guiGeom","Cannot read inty").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read inty");

  bottomy = xmlAttribute(elt,"bottomy","guiGeom","Cannot read bottomy").toInt(&ok);
  if (!ok)
    throw Exception("guiGeom","Cannot read bottomy");

  // font
  buttonFont.setFamily(xmlAttribute(elt,"fontFamily","guiGeom","Cannot read fontFamily"));
  buttonFont.setPixelSize(xmlAttribute(elt,"fontSize",
				       "guiGeom","Cannot read fontSize").toInt(&ok));
  if (!ok)
    throw Exception("guiGeom","Cannot read fontSize");

  buttondx = double(w-leftx-rightx+intx) / cols;
  buttondy = double(h-topy-bottomy+inty) / rows;
  buttonw = int(buttondx) - intx;
  buttonh = int(buttondy) - inty;
}
