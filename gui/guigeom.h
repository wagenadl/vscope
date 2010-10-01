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
