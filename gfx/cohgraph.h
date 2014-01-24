// cohgraph.h

#ifndef COHGRAPH_H

#define COHGRAPH_H

#include <base/roidata.h>
#include <gfx/radialgraph.h>
#include <math/cvec.h>
#include <QSet>
#include <QMap>
#include <QPointF>
#include <base/ccdtiming.h>

class CohGraph: public RadialGraph {
  Q_OBJECT;
  /*:C CohGraph
   */
public:
  CohGraph(class CohData *dat, QWidget *parent=0);
  virtual ~CohGraph();
  virtual void showEvent(class QShowEvent *);
  virtual void paintEvent(class QPaintEvent *);
  /*:F paintEvent
   *:D Redraws the current image and any ROIs
   */
public slots:
  void updateData();
  void setRefTrace(QString achn);
  void setRefDigi(QString digiline);
  void setRefFreq(double fref_hz);
  void updateSelection(int id);
protected:
  void mousePressEvent(class QMouseEvent *);
signals:
  void newSelection(int id);
private:
  void perhapsRefresh();
private:
  class CohData *data;
  bool owndata;
  int selectedID;
  QMap<int, QPointF> labelxy;
private: // these will not be implemented
  CohGraph(CohGraph const &other);
  CohGraph &operator=(CohGraph const &other);
};

#endif
