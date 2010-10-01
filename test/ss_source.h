// sigslot.cpp

#include <QObject>

class Source: public QObject {
  Q_OBJECT;
public:
  virtual ~Source();
  void makeint(int a, int b) {
    printf("will send %i %i.\n",a,b);
    emit intsig(a,b);
    printf("sent %i %i.\n",a,b);
  }
signals:
  void intsig(int a, int b);
};
