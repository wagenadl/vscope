// acquire.h

#ifndef ACQUIRE_H

#define ACQUIRE_H

#include <QObject>
#include <QTimer>

class Acquire: public QObject {
  Q_OBJECT;
public:
  Acquire();
  virtual ~Acquire();
public:
  void acqFrame();
  void acqTrial();
  void prepareLoad();
  void prepareImportROIs();
  void unlock();
  void setContEphys(); // reads from ptree whether or not to enable cont ephys
  void setAutoRun(); // reads from ptree whether or not to enable auto run
  void newTrialPeriod();
  static int maxTrial(); // returns the highest existing trial no for current expt
public slots:
  void loadData(QString fn); // usu. "/PATH/EXPT/TRIAL.xml"
  void importROIs(QString fn); // usu. "/PATH/EXPT/TRIAL.xml"
  void trialDone();
  void abort();
private slots:
  void chgDir(QString dir);
  void autoRunEvent();
private:
  void doneFrame();
  void doneTrial();
  void abortFrame();
  void abortTrial();
  void incTrialNo();
  void saveData();
  void displayCCD(bool writePixStatsToLog=false);
  void displayEPhys();
  void updateVSDTraces();
  void closeDialog();
public:
  void redisplayCCD();
private:
  void overrideAuto(class ParamTree *);
private:
  enum TrialType { NONE, FRAME, TRIAL };
  TrialType type;
  class FileDlgKey *loaddlg, *importdlg;
  class QFrame *loadframe, *importframe;
  QString lastdir;
  QTimer autoRunner;
  bool blockout;
};

#endif
