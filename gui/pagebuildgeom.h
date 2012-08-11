// pagebuildgeom.h

#ifndef PAGEBUILDGEOM_H

#define PAGEBUILDGEOM_H

#include <QDomElement>
#include <QRect>

#include <gui/guigeom.h>

class guiPage;

class PageBuildGeom {
  /*:C PageBuildGeom
   *:D Information about geometry used while building a page.
   */
public:
  PageBuildGeom(guiPage const *parent);
  PageBuildGeom(PageBuildGeom const &src, QDomElement doc);
  void setup(QDomElement doc);
  QRect bbox(); // box for next button
  QRect pbox(); // box for subpage
  void go(QDomElement doc);
  void right();
  void down();
  void left();
  void up();
  void last(int n=1);
  QRect const &boundingBox() const;
  void include(QRect bb);
private:
  guiPage const *parent;
  guiGeom const *master;

  double nextcol, nextrow; // location of next button

  struct ButtonPlacement {
    int x0, y0; // pixel position of button grid origin
    int w, h; // size of buttons
    int dx, dy; // grid spacing
  } button;

  struct SubPagePlacement {
    int dxl, dxr, dyt, dyb; // pixel count of how much smaller subpages are
                            // compared to parent.
  } page; 

  double rows; // number of rows of buttons, or 0 if not specified.
  double cols;  // number of columns of buttons, or 0 if not specified.

  QRect boundingbox;
private:
  void setup(QDomElement doc, QRect space);
};


#endif
