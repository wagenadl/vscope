// vsdgraph.h

#ifndef VSDGRAPH_H

#define VSDGRAPH_H

#include <gfx/linegraph.h>
#include <base/xyrra.h>
#include <base/roidata.h>
#include <base/roi3data.h>

class VSDGraph: public LineGraph {
  /*:C VSDGraph
   *:D A LineGraph derivative that shows donor, acceptor, and ratio traces
       for a single VSD ROI.
  */
  Q_OBJECT;
public:
  VSDGraph(class ROIData3Set *rs3d, QWidget *parent);
  virtual ~VSDGraph();
public slots:
  void updateSelection(int id);
private slots:
  void updateROIs();
  void updateROI(int id);
protected:
  virtual void paintEvent(class QPaintEvent *);
private:
  ROIData3Set *data; // we don't own this
private:
  class TraceInfo *trcDonor, *trcAcceptor, *trcRatio;
  int selid;
};

#endif
