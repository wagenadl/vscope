// vprojector.h

#ifndef VPROJECTOR_H

#define VPROJECTOR_H

#include <QObject>

class VProjector: public QObject {
  Q_OBJECT;
public:
  VProjector(QObject *parent=0);
  virtual ~VProjector();
  bool isOk() const;
public slots:
  bool activate();
  void prepare(class ParamTree const *);
  void start();
private slots:
  void readyReadStderr();
  void readyReadStdout();
private:
  class QProcess *proc;
};

#endif
