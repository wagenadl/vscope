#include <gfx/virtkeyboard.h>
#include <QApplication>
#include "foo.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  VirtKeyboard vk(0);
  Foo foo;
  QObject::connect(&vk,SIGNAL(keyPress(QChar)),&foo,SLOT(report(QChar)));
  vk.setGeometry(100,100,800,250);
  vk.show();
  return app.exec();
}
