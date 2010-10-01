#include <QObject>

class Foo: public QObject {
  Q_OBJECT;
public slots:
  void report(QChar c);
};

