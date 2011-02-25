// trial.h

#ifndef TRIAL_H

#define TRIAL_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include <base/types.h>
#include <base/xyrra.h>
#include <acq/trialdata.h>

class Trial: public QObject {
  Q_OBJECT;
public:
  Trial(TrialData *d);
  virtual ~Trial();
  void prepare(class ParamTree const *ptree);
  void prepareSnapshot(class ParamTree const *ptree);
  bool isPrepared() const { return dat->isPrepared(); }
  bool isActive() const { return isPrepared() ? active : false; }
  // TrialData const &data() const { return *dat; }
  // TrialData &data() { return *dat; }
protected:
  virtual void completedEvent();
  /*:F completedEvent
   *:D Called after a trial is complete, just before the "ended" signal
       is emitted. Descendants can use this to update their state.
  */
public slots:
  void start();
  void abort(); // I need to decide about error handling in abort() 
signals:
  void ended(QString exptname, QString trialid);
protected slots:
  void ephysoutComplete();
  void ephysacqComplete();
  void allEPhysComplete();
private:
  TrialData *dat;
  // acquisition
  class EPhysAcq *ephysacq;
  class CCDAcq *ccdacq;
  class EPhysOut *ephysout;
  // status
  bool prep;
  bool active;
  bool outcomplete, acqcomplete; // only valid if prepared
};

#endif
