// base26.cpp

#include <base/base26.h>

#include <stdio.h>
#include <QString>

int main(int argc, char **argv) {
  for (int i=-10; i<1000; i++) {
    QString s = num2az(i);
    int j = az2num(s);
    fprintf(stderr,"%i -> %s -> %i\n",i,qPrintable(s),j);
  }
  return 0;
  
}
