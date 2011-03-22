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
public slots:
  void updateSelection(int id);
  void setRefTrace(QString chn);
  void setRefDigi(int chn);
  void setRefFreq(double frqhz);
private slots:
  void updateEPhysData();
private:
  class VSDGraph *selected;
  class VSDAllGraph *allgraph;
  class LineGraph *refgraph;
  class TraceInfo *reftrace;
  int selid;
  QString refchn;
};

#endif
