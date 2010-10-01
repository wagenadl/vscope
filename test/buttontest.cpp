// buttontest.cpp

#include <QApplication>
#include <QWidget>
#include <gfx/button.h>
#include <stdio.h>
#include <QButtonGroup>

int main(int argc, char **argv) {
  printf("Hello world\n");
  QApplication qapp(argc, argv);
  QWidget mw;
  mw.setGeometry(400,300,1024,768);

  Button a1(&mw,50,10,0);
  a1.setText("Act1");

  Button a2(&mw,50,50,1);
  a2.setText("Act2");

  Button t1(&mw,200,10,2);
  t1.makeToggle();
  t1.setText("Toggle1");

  Button t2(&mw,200,50,3);
  t2.makeToggle();
  t2.setText("Toggle2");

  Button r1(&mw,400,10,4);
  r1.makeRadio();
  r1.setText("Radio1");

  Button r2(&mw,400,50,5);
  r2.makeRadio();
  r2.setText("Radio2");

  QObject::connect(&a1,SIGNAL(clicked()),&a2,SLOT(toggleSelected()));
  
  QButtonGroup bg;
  bg.addButton(&r1);
  bg.addButton(&r2);

  Button f1(&mw,600,10,6);
  f1.setText("Flat1");
  f1.setEditable(false);

  Button f2(&mw,600,50,7);
  f2.setText("Flat2");
  f2.setEnabled(false);
 
  mw.show();
  qapp.exec();
  return 0;
}
