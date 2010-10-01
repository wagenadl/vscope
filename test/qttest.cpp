#include <QApplication>
#include <tstlib.h>

int main(int argc, char **argv) {
  QApplication a(argc, argv);
  testLabel l(0);
  l.setText("Hello world");
  l.setGeometry(100,200,200,100);
  l.show();
  a.exec();
  return 0;
}

