// pagebuildgeom.cpp

#include "pagebuildgeom.h"
#include "guiroot.h"
#include <base/exception.h>
#include <base/dbg.h>
#include <xml/attribute.h>
#include <math.h>

PageBuildGeom::PageBuildGeom(guiPage const *parent):
  parent(parent),
  master(&parent->masterp()->geom()) {
}

PageBuildGeom::PageBuildGeom(PageBuildGeom const &src,
			     QDomElement doc):
  parent(src.parent),
  master(src.master) {

  double c0 = (doc.hasAttribute("x"))
    ? doc.attribute("x").toDouble()
    : src.nextcol;
  int nc = (doc.hasAttribute("spancols"))
    ? doc.attribute("spancols").toInt()
    : src.cols-c0;
  double r0 = (doc.hasAttribute("y"))
    ? doc.attribute("y").toDouble()
    : src.nextrow;
  int nr = (doc.hasAttribute("spanrows"))
    ? doc.attribute("spanrows").toInt()
    : src.rows-r0;
    
  double w = src.button.dx*(nc-1) + src.button.w;
  double h = src.button.dy*(nr-1) + src.button.h;
  double x0 = src.button.x0 + c0*src.button.dx;
  double y0 = src.button.y0 + r0*src.button.dy;
  setup(doc, QRect(x0, y0, w, h));
}  

void PageBuildGeom::setup(QDomElement doc) {
  if (!parent)
    throw Exception("PageBuildGeom", "No parent");

  QRect geom = QRect(master->leftx,
		     master->topy,
		     parent->width()-master->rightx-master->leftx,
		     parent->height()-master->bottomy-master->topy);
  setup(doc, geom);
}

void PageBuildGeom::setup(QDomElement doc, QRect geom) {
  rows = doc.attribute("rows").toDouble();
  cols = doc.attribute("cols").toDouble();

  button.dx = master->buttondx;
  button.w = master->buttonw;
  button.dy = master->buttondy;
  button.h = master->buttonh;
  button.y0 = geom.top();
  button.x0 = geom.left();

  double avwidth = geom.width() + master->intx;
  double avheight = geom.height() + master->inty;

  bool fullspec = cols>0 && rows>0;
  
  if (rows==0)
    rows = round(avheight / button.dy);
  if (cols==0)
    cols = round(avwidth / button.dx);
  if (button.dy*rows > avheight) {
    button.dy = avheight / rows;
    button.h = button.dy - master->inty;
  }
  if (button.dx*cols > avwidth) {
    button.dx = avwidth / cols;
    button.w = button.dx - master->intx;
  }

  page.dxl = int(button.dx*doc.attribute("subdxl").toDouble() + 1);
  page.dxr = int(button.dx*doc.attribute("subdxr").toDouble() + 1);
  page.dyt = int(button.dy*doc.attribute("subdyt").toDouble() + 1);
  page.dyb = int(button.dy*doc.attribute("subdyb").toDouble() + 1);

  if (fullspec) {
    go(cols-1, rows-1);
    include(bbox());
  }
  go(0, 0);
}

void PageBuildGeom::down() {
  nextrow++;
  if (nextrow+.99>=rows) {
    nextrow=0;
    nextcol++;
  }
}

void PageBuildGeom::right() {
  nextcol++;
  if (nextcol+.99>=cols) {
    nextcol=0;
    nextrow++;
  }
}

void PageBuildGeom::up() {
  nextrow--;
  if (nextrow<0) {
    nextrow=floor(rows)-1;
    nextcol--;
  }
}

void PageBuildGeom::left() {
  nextcol--;
  if (nextcol<0) {
    nextcol=floor(cols)-1;
    nextcol++;
  }
}

void PageBuildGeom::last(int N) {
  nextcol = floor(cols)-1;
  nextrow = floor(rows)-1;
  for (int n=1; n<N; n++)
    left();
}

void PageBuildGeom::go(QDomElement doc) {
  if (doc.hasAttribute("x"))
    nextcol = doc.attribute("x").toDouble();
  if (doc.hasAttribute("y"))
    nextrow = doc.attribute("y").toDouble();
}

void PageBuildGeom::go(int col, int row) {
  nextcol = col;
  nextrow = row;
}

QRect PageBuildGeom::bbox() {
  QRect bb;
  bb = QRect(int(button.x0 + button.dx*nextcol),
	     int(button.y0 + button.dy*nextrow),
	     button.w,
	     button.h);
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

QRect const &PageBuildGeom::boundingBox() const {
  return boundingbox;
}  

void PageBuildGeom::include(QRect bb) {
  boundingbox |= bb;
}
