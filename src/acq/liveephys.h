// acq/liveephys.h - This file is part of VScope.
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

// liveephys.h

#ifndef LIVEEPHYS_H

#define LIVEEPHYS_H

#include <QFrame>
#include <QStringList>
#include <QMap>
#include <QDomElement>
#include <xml/connections.h>
#include <daq/analogin.h>

class LiveEPhys: public QFrame {
  Q_OBJECT;
public:
  LiveEPhys(QWidget *parent, QDomElement conf);
  virtual ~LiveEPhys();
  void activate(class ParamTree *p); // this activates as stand-alone
  void activate(class ParamTree *p,
		class ContAcq *ca); // this activates as passive follower
  void deactivate(bool quietIfAlready=false);
  void newTimebase();
  void newAIChannels();
  void newDIChannels();
  void autoRange();
  void setVCOChannel(QString cid);
  void hideConfig(class guiPage &cfgpage);
  void unhideConfig(class guiPage &cfgpage);
protected:
  void resizeEvent(class QResizeEvent *);
protected slots:
  void dataAvailable();
  void contAcqAvailable(int ascans, int dscans);
private:
  void addChannels(class MultiGraph *cc, QStringList const &list);
  void addChannel(QString cid);
  bool isChannelTiny(QString id) const;
  void newData(class MultiGraph *cc);
  void feedVCO(int offset, int count);
  void activateVCO();
  void addedData();
  void reactivate();
private:
  class MultiGraph *ic, *ec;
  bool isActive;
  class ParamTree *ptree;
  class AnalogIn *ain;
  QStringList potchs_left;
  QStringList potchs_right;
  class AnalogData *data;
  double dt_s;
  QMap<QString, class TraceInfo *> aitraces; // map from channel ID to trace
  QMap<QString, class LineGraph *> aigraphs; // map from channel ID to graph
  QList<Connections::AIChannel const *> channels;
  bool first;
  double t0;
  int dataoffset;
  int vcoidx;
  double vcoscale;
  QString vcoid;
  class VCO *vco;
  class ContAcq *contacq;
};

#endif
