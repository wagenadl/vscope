// vsdtraces.h

#ifndef VSDTRACES_H

#define VSDTRACES_H

#include <gfx/multigraph.h>
#include <base/xyrra.h>
#include <base/roidata.h>
#include <QMap>
#include <gfx/vsdallgraph.h>
#include <base/ccdtiming.h>

class VSDTraces: public MultiGraph {
  /*:C VSDTraces
   *:D MultiGraph showing:
       (1) The cc, ox, and rat traces of the currently selected ROI
       (2) The rat traces of all ROIs (current highlighted in red?)
       (3) (?) Intracellular voltage [as a tiny]
       (4) (?) Stimuli [as a tiny]
   *:N For now, only (1, 2) are implemented.
  */
  Q_OBJECT;
public:
  VSDTraces(QWidget *parent);
  virtual ~VSDTraces();
  class ROI3Data *getData(int id);
  bool haveData(int id) const;
  CCDTiming const &getTiming() const { return timing; }
  class ROISet const *getROIs() const { return roiset; }
  class AnalogData const *getAnalog() const { return lastAnalog; }
  class DigitalData const *getDigital() const { return lastDigital; }
public slots:
  void selectROI(int id);
  void editROI(int id); // emits roisChanged
  void deleteROI(int id); // emits roisChanged
  void newCCDData(bool dontTell=false);
  void newEPhys(bool dontTell=false);
  void newROIs(class ROISet const *newrois, bool dontTell=false); // does *not* emit roisChanged
  void setDebleach(ROIData::Debleach d);
  void setRefTrace(int chn);
  void setRefDigi(int chn);
  void setRefFreq(double frqhz);
signals:
  void dataChanged();
  void roisChanged();
private:
  class VSDGraph *selected;
  class VSDAllGraph *allgraph;
  class LineGraph *refgraph;
  class TraceInfo *reftrace;
  int selid;
  int refchn;
  CCDTiming timing;
private: // we do not own the following
  class ROISet const *roiset;
  class AnalogData const *lastAnalog;
  class DigitalData const *lastDigital;
};

#endif
