#include <gfx/button.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QWidget>
#include "test/myscrollarea.h"

int main(int argc, char **argv) {
  QApplication app(argc,argv);
  MyScrollArea sa(0);
  sa.setGeometry(100,100,500,50);

  QWidget mw;
  sa.setWidget(&mw);
  mw.setGeometry(100,100,80,50);
  QHBoxLayout ly(&mw);

  ly.addStretch(10);
  
  Button one(&mw);
  one.setText("Five hundred and fifty one");
  ly.addWidget(&one);

  Button two(&mw);
  two.setText("Two");
  ly.addWidget(&two);

  Button three(&mw);
  three.setText("Three");
  ly.addWidget(&three);

  Button four(&mw);
  four.setText("Four");
  ly.addWidget(&four);

  Button five(&mw);
  five.setText("Five");
  ly.addWidget(&five);

  ly.setMargin(2);
  ly.setSpacing(4);

  //  mw.setLayout(&ly);
  mw.resize(mw.sizeHint());

  sa.show(); mw.show();
  return app.exec();
}

  
