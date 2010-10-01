#include <QObject>
#include <QString>
#include <stdio.h>

class Recv: public QObject {
  Q_OBJECT;
public:
  virtual ~Recv();
public slots:
  void recv() {
    printf("received nothing\n");
  }
  void recvint(int a) {
    printf("received int: %i\n",a);
  }
  void recvintint(int a, int b) {
    printf("received ints: %i %i\n",a,b);
  }
  void recvstring(QString a) {
    printf("received string: %s\n",qPrintable(a));
  }
  void recvintstring(int a, QString b) {
    printf("received int %i, string: %s\n",a,qPrintable(b));
  }
  
};
