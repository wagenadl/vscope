#include <QApplication>
#include <QLabel>

int main(int argc, char **argv) {
  QApplication a(argc, argv);
  QLabel l(0);
  l.setText("Hello world");
  l.setGeometry(100,200,200,100);
  l.show();
  a.exec();
  return 0;
}

