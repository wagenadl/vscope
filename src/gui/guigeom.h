// gui/guigeom.h - This file is part of VScope.
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

// guigeom.h

#ifndef GUIGEOM_H

#define GUIGEOM_H

#include <QDomElement>
#include <QFont>

class guiGeom {
public:
  guiGeom(QDomElement elt);
public:
  int x0, y0, w, h; // position of root page relative to main window
  double cols, rows; // nominal size of button grid
  int menucaph; // pixels for menu caption
  int leftx, intx, rightx; // horizontal margins (pixels)
  int topy, inty, bottomy; // vertical margins (pixels)
  double buttondx, buttondy; // default grid intervals (pixels)
  int buttonw, buttonh; // size of buttons (pixels)
  QFont buttonFont;
};

#endif
