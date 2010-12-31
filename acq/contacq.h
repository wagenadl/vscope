// contacq.h

#ifndef CONTACQ_H

#define CONTACQ_H

#include <QObject>
#include <QVector>
#include <QDomElement>

class ContAcq: public QObject {
  Q_OBJECT;
public:
  ContAcq();
  virtual ~ContAcq();
  void prepare(class ParamTree /*const*/ *ptree, QString dir);
public: // information
  class AnalogData *adata() const { return active ? adataIn : 0; }
  class DigitalData *ddata() const { return active ? ddataIn : 0; }
  double getAcqFreq_Hz() const { return analogRate_Hz; }
  bool isActive() const { return active; }
public slots:
  void start();
  void stop();
  void markTrial(int trialno);
private:
  void dataAvailable(int analogscans, int digitalscans);
  static void dataAvFcn(int analogscans, int digitalscans, void *aux);
signals:
  void dataSaved(int analogscans, int digitalscans);
private:
  class AnalogData *adataIn;
  class DigitalData *ddataIn;
  class EPhysAcq *ephysacq;
  QString exptname;
  QString trialid;
  QString dir;
  bool active;
  bool prep;
  bool dummy;
  class XML *xml;
  QString xmlfn;
  class QFile *analogFile;
  class QFile *digitalFile;
  class QDataStream *analogStream;
  class QDataStream *digitalStream;
  int analogFileOffset; // number of scans written so far
  int digitalFileOffset; // number of scans written so far
  double analogRate_Hz; // so we can calculate duration
  QVector<double> analogMax; // by channel index, not port number
  QDomElement info_elt;
  QDomElement ana_elt;
  QDomElement digi_elt;
  int16_t *analogBinary;
  int analogBinaryScans;
};

#endif
