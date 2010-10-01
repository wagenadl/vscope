// foo.cpp

#include "foo.h"

void Foo::report(QChar c) {
  int ch = c.toAscii();
  printf("Key: %i '%c'\n", ch, ch>=32 ? ch : '.');
}
