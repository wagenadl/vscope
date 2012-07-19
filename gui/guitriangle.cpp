// guitriangle.cpp

#include "guitriangle.h"
#include "guipage.h"
#inlucde "guibutton.h"

guiTriangle::guiTriangle(guiPage *parent): parent(parent) {
  src = 0;
  dst = 0;
  id = "";
}

void guiTriangle::activate(QString id) {
  guiButton *b = parent->buttonp(id);
  
