// pagebuildgeom.h

#ifndef PAGEBUILDGEOM_H

#define PAGEBUILDGEOM_H

#include <QDomElement>

#include <gui/guigeom.h>

class xmlPage;

class PageBuildGeom {
  /*:C PageBuildGeom
   *:D Information about geometry used while building a page.
   */
public:
  PageBuildGeom(xmlPage const *parent);
  void setup(QDomElement doc);
public:
  double nextcol, nextrow; // location of next button
  struct ButtonPlacement {
    int x0, y0; // pixel position of button grid origin
    int w, h; // size of buttons
    double dx, dy; // grid spacing
  } button;
  struct SubPagePlacement {
    int dxl, dxr, dyt, dyb; // pixel count of how much smaller subpages compared to parent.
  } page; 
  int rows, cols; // number of rows,cols of buttons, or 0 if not specified.
  int caph; // height of caption widget, or 0 if none.
  guiGeom const *master;
private:
  xmlPage const *parent;
};


#endif
