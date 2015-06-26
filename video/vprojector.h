// vprojector.h

#ifndef VPROJECTOR_H

#define VPROJECTOR_H

#include <QObject>

class VProjector: public QObject {
  Q_OBJECT;
public:
  VProjector(QObject *parent=0);
  virtual ~VProjector();
public slots:
  bool activate();
  void prepare(class ParamTree const *);
  void start();
private:
  class QProcess *proc;
};

#endif
