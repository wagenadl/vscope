// toplevel/acquire.h - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

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
