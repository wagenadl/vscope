// pagebuildgeom.cpp

#include "pagebuildgeom.h"
#include "xmlgui.h"

PageBuildGeom::PageBuildGeom(xmlPage const *parent):
  master(&parent->masterp()->geom()), parent(parent) {
}

void PageBuildGeom::setup(QDomElement doc) {
  QRect geom = parent->geometry();

    bool ok;
  page.dxl = int(master->buttondx*doc.attribute("subdxl")
		   .toDouble(&ok)); // default to 0.
  page.dxr = int(master->buttondx*doc.attribute("subdxr")
		   .toDouble(&ok)); // default to 0.
  page.dyt = int(master->buttondy*doc.attribute("subdyt")
		   .toDouble(&ok)); // default to 0.
  page.dyb = int(master->buttondy*doc.attribute("subdyb")
		   .toDouble(&ok)); // default to 0.

  rows = doc.attribute("rows").toInt(&ok); // default to 0.
  cols = doc.attribute("cols").toInt(&ok); // default to 0.
  caph = master->topy;
  button.dx = master->buttondx;
  button.w = master->buttonw;
  button.dy = master->buttondy;
  button.h = master->buttonh;
  button.y0 = caph;
  button.x0 = master->leftx;
  if (rows>0) {
    if (geom.height()-caph<button.dy*rows) {
      button.dy = double(geom.height()
			   -caph-master->bottomy+master->inty)
	/ rows;
      button.h = int(button.dy) - master->inty;
    } else {
      //resize(width(),int(button.dy*(rows+1)-button.h));
    }
  }
  if (cols>0) {
    if (geom.width()<button.dx*cols) {
      button.dx = double(geom.width()-master->leftx
			   -master->rightx+master->intx)
	/ cols;
      button.w = int(button.dx) - master->intx;
    } else {
      //resize(int(button.dx*(cols+1)-button.w),height());
    }
  }
  nextcol = 0;
  nextrow = 0;
}
