#include <gfx/scrollbuttonbar.h>
#include <QApplication>

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  ScrollButtonBar pb(0);
  pb.setGeometry(200,200,500,50);
  pb.setAsPath("/home/wagenaar/progs/vsdscope/src/toplevel");
  pb.setAsPath("/usr/local/dw/bin");
  pb.show();
  pb.setAsPath("/usr/share/tex");
  pb.setAsPath("/home/wagenaar/progs/vsdscope/src/toplevel");
  return app.exec();
}
