#include "solvep.h"
#include <stdio.h>
#include <QString>

void solns(QList<double> r) {
  QString sep="";
  for (QList<double>::iterator i=r.begin(); i!=r.end(); ++i) {
    printf("%sx=%g",qPrintable(sep),*i);
    sep = " | ";
  }
  if (sep=="")
    printf("no real sol.");
}

void solns(QList<cplx> r) {
  QString sep="";
  for (QList<cplx>::iterator i=r.begin(); i!=r.end(); ++i) {
    printf("%sx=%g+%gi",qPrintable(sep),(*i).re,(*i).im);
    sep = " | ";
  }
}

int main(int, char **) {

  cplx a(1,2);
  cplx b(3,4);
  cplx c = a/b;
  printf("(1+2i) / (3+4i) = %g + %g i\n",c.re,c.im);
  c=sqrt(a);
  printf("sqrt(1+2i) = %g + %g i\n",c.re,c.im);
  c=curt(a);
  printf("curt(1+2i) = %g + %g i\n",c.re,c.im);
  c=0.5/a;
  printf("0.5/(1+2i) = %g + %g i\n",c.re,c.im);
  
  QList<double> r = solvep2(1,0,-1);
  printf("x^2 -1 = 0 <=> "); solns(r); printf("\n");

  r = solvep2(1,0,1);
  printf("x^2 + 1 = 0 <=> "); solns(r); printf("\n");

  r = solvep2(1,2,1);
  printf("x^2 + 2x + 1 = 0 <=> "); solns(r); printf("\n");
  
  r = solvep4(1,0,-1,0,-2);
  printf("x^4 -x^2 -2 = 0 <=> "); solns(r); printf("\n");

  QList<cplx> q = solvep4(cplx(1),0,-1,0,-2);
  printf("x^4 -x^2 -2 = 0 <=> "); solns(q); printf("\n");
  
  
  return 0;
}

    
  
