// cohgraph.h

#ifndef COHGRAPH_H

#define COHGRAPH_H

#include <base/roidata.h>
#include <gfx/radialgraph.h>
#include <toplevel/globals.h>
#include <toplevel/vsdtraces.h>
#include <math/cvec.h>
#include <QSet>
#include <base/ccdtiming.h>

class CohGraph: public RadialGraph {
  Q_OBJECT;
  /*:C CohGraph
   *:D This class heavily depends on a VSDTraces for its data.
   */
public:
  CohGraph(class CohData *dat=0, QWidget *parent=0);
  virtual ~CohGraph();
  virtual void showEvent(class QShowEvent *);
  virtual void paintEvent(class QPaintEvent *);
  /*:F paintEvent
   *:D Redraws the current image and any ROIs
   */
public slots:
  void newData();
  void setRefTrace(int achn);
  void setRefDigi(int digiline);
  void setRefFreq(double fref_hz);
private:
  void perhapsRefresh();
private:
  class CohData *data;
  bool owndata;
private: // these will not be implemented
  CohGraph(CohGraph const &other);
  CohGraph &operator=(CohGraph const &other);
};

#endif
