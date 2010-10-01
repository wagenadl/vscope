// vsdallgraph.h

#ifndef VSDALLGRAPH_H

#define VSDALLGRAPH_H

#include <QMap>
#include <gfx/linegraph.h>
#include <base/xyrra.h>
#include <base/roidata.h>

class VSDAllGraph: public LineGraph {
  /*:C VSDAllGraph
   *:D A LineGraph derivative that shows (just) the ratio trace for
       a whole bunch of ROIs.
   *:N See also VSDGraph.
   */
  Q_OBJECT;
public:
  VSDAllGraph(class ROISet3Data *rs3d, QWidget *parent=0);
  virtual ~VSDAllGraph();
  void setROI(int id, XYRRA el);
  void setROI(int id, class PolyBlob const *pb);
  void removeROI(int id);
  void selectROI(int id);
  void clearROIs();
  void setData(class CCDData const *donor, class CCDData const *acceptor);
  void setDebleach(ROIData::Debleach d);
  class ROI3Data *getData(int id);
  bool haveData(int id) const;
protected:
  virtual void paintEvent(class QPaintEvent *);
private:
  void newOffsets(); // recalculates the y offsets of each trace
  void setROIbase(int id);
private:
  class ROISet3Data *data;
  QMap<int, class TraceInfo *> traces;
  int selectedId;
private:
  VSDAllGraph(VSDAllGraph const &other);
  VSDAllGraph &operator=(VSDAllGraph const &other);
};

#endif
