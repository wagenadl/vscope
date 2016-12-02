// checkvid.cpp

#include <QWidget>
#include <stdio.h>
#include <base/exception.h>

void checkvid(QWidget *dest) {
  fprintf(stderr,"checkvid. dest=%p\n", dest);
  throw Exception("CheckVid","We don't have video","checkvid");
}
