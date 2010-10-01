#include <gfx/filedlgkey.h>
#include <QApplication>
 
int main(int argc, char **argv) {
  QApplication app(argc, argv);
  FileDlgKey fd(0);
  fd.setGeometry(200,200,800,600);
  fd.mustExist(true);
  fd.setExtn("cpp");
  fd.setLabel("Load a cpp file");
  fd.goDir("/home/wagenaar/progs/vsdscope");
  fd.show();
  return app.exec();
}
