// acq/contacq.h - This file is part of VScope.
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

// contacq.h

#ifndef CONTACQ_H

#define CONTACQ_H

#include <QObject>
#include <QVector>
#include <QDomElement>
#include <stdint.h>

class ContAcq: public QObject {
  Q_OBJECT;
public:
  ContAcq();
  virtual ~ContAcq();
  QString prepare(class ParamTree *ptree);
  /*:F prepare
   *:R Returns actual trial name (which may have a letter appended).
   */
public: // information
  class AnalogData *adata() const { return active ? adataIn : 0; }
  class DigitalData *ddata() const { return active ? ddataIn : 0; }
  double getAcqFreq_Hz() const { return analogRate_Hz; }
  bool isActive() const { return active; }
public slots:
  void start();
  void stop();
  void markTrial(QString trialno);
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
  QVector<int16_t> analogBinary;
  int analogBinaryScans;
};

#endif
