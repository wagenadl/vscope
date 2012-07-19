// pagebuildgeom.cpp

#include "pagebuildgeom.h"
#include "guiroot.h"
#include <base/exception.h>
#include <base/dbg.h>

PageBuildGeom::PageBuildGeom(guiPage const *parent):
  master(&parent->masterp()->geom()), parent(parent) {
  ingroup = false;
  baserow = 0;
  groupcols = 1;
}

void PageBuildGeom::setup(QDomElement doc) {
  QRect geom = parent->geometry();

  page.dxl = int(master->buttondx*doc.attribute("subdxl")
		   .toDouble() + 1);
  page.dxr = int(master->buttondx*doc.attribute("subdxr")
		   .toDouble() + 1);
  page.dyt = int(master->buttondy*doc.attribute("subdyt")
		   .toDouble() + 1);
  page.dyb = int(master->buttondy*doc.attribute("subdyb")
		   .toDouble() + 1);

  rows = doc.attribute("rows").toInt();
  cols = doc.attribute("cols").toInt();
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

void PageBuildGeom::advance(double dy) {
  nextrow += dy;
  if (rows>0 && nextrow>=rows) {
    nextColumn(QDomElement());
  }
}

void PageBuildGeom::go(QDomElement doc) {
  if (doc.hasAttribute("x"))
    nextcol=doc.attribute("x").toDouble();
  if (doc.hasAttribute("y"))
    nextrow=doc.attribute("y").toDouble();
}

QRect PageBuildGeom::bbox() {
  QRect bb;
  if (ingroup && groupcols>1) {
    if (groupcol==groupcols-1) 
      bb = QRect(int(button.x0 + button.dx*nextcol),
		 int(button.y0 + button.dy*nextrow),
		 button.w/groupcols - 2,
		 button.h);
    else
      bb = QRect(int(button.x0 + button.dx*nextcol),
		 int(button.y0 + button.dy*nextrow),
		 button.w/groupcols - master->intx*(groupcols-1),
		 button.h);
  } else {
    bb = QRect(int(button.x0 + button.dx*nextcol),
	       int(button.y0 + button.dy*nextrow),
	       button.w,
	       button.h);
  }
  boundingbox |= bb;
  return bb;
}

QRect PageBuildGeom::pbox() {
  if (!parent)
    throw Exception("PageBuildGeom", "No parent");
  QRect bb = QRect(page.dxl,
		   page.dyt,
		   parent->width() - page.dxl - page.dxr,
		   parent->height() - page.dyt - page.dyb);
  boundingbox |= bb;
  return bb;
}

void PageBuildGeom::vspace(QDomElement doc) {
  advance(doc.hasAttribute("dy")
	  ? doc.attribute("dy").toDouble()
	  : 0.2);
}
  

void PageBuildGeom::nextColumn(QDomElement doc) {
  if (nextrow>baserow) {
    // i.e., don't do it if advance() already did it.
    nextrow = baserow;
    nextcol += 1./groupcols;
    groupcol++;
  }

  if (doc.hasAttribute("dx"))
    nextcol += doc.attribute("dx").toDouble();
}

void PageBuildGeom::enterGroup(QDomElement doc) {
  ingroup = true;
  groupcols = doc.hasAttribute("cols")
    ? doc.attribute("cols").toInt()
    : 1;
  baserow = nextrow;
  groupcol = 0;
}

void PageBuildGeom::leaveGroup() {
  if (ingroup) 
    nextColumn(QDomElement());
  baserow = 0;
  groupcols = 1;
  ingroup = false;
}

QRect const &PageBuildGeom::boundingBox() const {
  return boundingbox;
}  
