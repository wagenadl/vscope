// pagebuildgeom.cpp

#include "pagebuildgeom.h"
#include "guiroot.h"

PageBuildGeom::PageBuildGeom(guiPage const *parent):
  master(&parent->masterp()->geom()), parent(parent) {
}

void PageBuildGeom::setup(QDomElement doc) {
  QRect geom = parent->geometry();

  bool ok; // dummy var, all default to zero
  page.dxl = int(master->buttondx*doc.attribute("subdxl")
		   .toDouble(&ok) + 1);
  page.dxr = int(master->buttondx*doc.attribute("subdxr")
		   .toDouble(&ok) + 1);
  page.dyt = int(master->buttondy*doc.attribute("subdyt")
		   .toDouble(&ok) + 1);
  page.dyb = int(master->buttondy*doc.attribute("subdyb")
		   .toDouble(&ok) + 1);

  rows = doc.attribute("rows").toInt(&ok);
  cols = doc.attribute("cols").toInt(&ok);
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
			   - caph - master->bottomy + master->inty)
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
