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
  void setup(QDomElement doc);
  QRect bbox(); // box for next button
  QRect pbox(); // box for subpage
  void go(QDomElement doc);
  void advance(double dy=1);
  void vspace(QDomElement doc);
  void nextColumn(QDomElement doc);
  void enterGroup(QDomElement doc);
  void leaveGroup();
  QRect const &boundingBox() const;
private:
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
  bool ingroup;
  int groupcols;
  double baserow;
  int groupcol;
  QRect boundingbox;
  guiPage const *parent;
};


#endif
