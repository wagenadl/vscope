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
  VSDGraph(QWidget *parent);
  virtual ~VSDGraph();
  void setROI(XYRRA el);
  void setROI(class PolyBlob const *pb);
  void setData(class CCDData const *donor, class CCDData const *acceptor);
  void setDebleach(ROIData::Debleach d);
protected:
  virtual void paintEvent(class QPaintEvent *);
private:
  ROI3Data data;
  class TraceInfo *trcDonor, *trcAcceptor, *trcRatio;
  double t0_ms, dt_ms;
  class ROISet *roiset;
};

#endif
