#include <base/polyblob.h>
#include <stdio.h>
#include <base/xml.h>

int main(int, char **) {
  double xx[] = { 1, 5, 8, 5, 1 };
  double yy[] = { -2, -1, 0, 1, 2 };
  PolyBlob b(5);
  b.build(5,xx,yy);
  b.revertex(6);
  b.revertex(4);
  for (int i=0; i<16; i++)
    printf("%g %g\n",b.x(i),b.y(i));

  XML xml(0,"blob");
  QDomElement elt = xml.append("roi");
  b.write(elt);
  xml.write("/tmp/pbt.xml");

  printf("\n\n");
  XML xm("/tmp/pbt.xml");
  QDomElement el = xm.find("roi");
  PolyBlob c(el);
  for (int i=0; i<16; i++)
    printf("%g %g\n",c.x(i),c.y(i));
  return 0;
}


