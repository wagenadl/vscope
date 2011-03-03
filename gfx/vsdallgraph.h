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
  VSDAllGraph(class ROIData3Set *rs3d, QWidget *parent=0);
  virtual ~VSDAllGraph();
  class ROI3Data *getData(int id);
  bool haveData(int id) const;
public slots:
  void updateSelection(int id);
  void updateData();
private slots:
  void updateROI(int id);
  void updateROIs();
protected:
  virtual void paintEvent(class QPaintEvent *);
private:
  void updateROIcore(int id);
  void newOffsets(); // recalculates the y offsets of each trace
private:
  class ROIData3Set *data; // we do not own this
  QMap<int, class TraceInfo *> traces;
  int selectedId;
private:
  VSDAllGraph(VSDAllGraph const &other);
  VSDAllGraph &operator=(VSDAllGraph const &other);
};

#endif
