// acq/trial.h - This file is part of VScope.
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
  void reconstructStim(class ParamTree const *ptree);
  bool isPrepared() const { return dat->isPrepared(); }
  bool isActive() const { return isPrepared() ? active : false; }
  bool isComplete() const { return outcomplete && acqcomplete && ccdcomplete; }
public slots:
  void start();
  void abort(); // I need to decide about error handling in abort()
signals:
  void ended(QString exptname, QString trialid); // check isComplete to see if truly OK
protected slots:
  void ephysoutComplete();
  void ephysacqComplete();
  void allEPhysComplete();
  void updateCameras();
private:
  TrialData *dat;
  // acquisition
  class EPhysAcq *ephysacq;
  class CCDAcq *ccdacq;
  class EPhysOut *ephysout;
  // status
  bool prep;
  bool active;
  bool outcomplete;
  bool acqcomplete;
  bool ccdcomplete;
};

#endif
