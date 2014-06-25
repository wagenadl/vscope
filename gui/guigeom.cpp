// guigeom.cpp

#include <gui/guigeom.h>
#include <xml/attribute.h>
#include <base/exception.h>

guiGeom::guiGeom(EasyXML elt) {
  bool ok;

  // top geometry
  x0 = elt.demandInt("x0", "guiGeom: Cannot read x0");
  y0 = elt.demandInt("y0", "guiGeom: Cannot read y0");  
  w = elt.demandInt("w", "guiGeom: Cannot read w");  
  h = elt.demandInt("h", "guiGeom: Cannot read h");

  cols = elt.demandReal("cols", "guiGeom: Cannot read cols");    
  rows = elt.demandReal("rows", "guiGeom: Cannot read rows");    

  menucaph = elt.demandInt("menucaph", "guiGeom: Cannot read menucaph");  

  leftx = elt.demandInt("leftx", "guiGeom: Cannot read leftx");
  rightx = elt.demandInt("rightx", "guiGeom: Cannot read rightx");
  topy = elt.demandInt("topy", "guiGeom: Cannot read topy");
  bottomy = elt.demandInt("bottomy", "guiGeom: Cannot read bottomy");

  buttonFont.setFamily(elt.demandString("fontFamily",
					"guiGeom: Cannot read fontFamily"));
  buttonFont.setPixelSize(elt.demandInt("fontSize",
					"guiGeom: Cannot read fontSize"));

  buttondx = double(w-leftx-rightx+intx) / cols;
  buttondy = double(h-topy-bottomy+inty) / rows;
  buttonw = int(buttondx) - intx;
  buttonh = int(buttondy) - inty;
}
