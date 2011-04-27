// radialgraph.h

#ifndef RADIALGRAPH_H

#define RADIALGRAPH_H

#include <QWidget>
#include <QFrame>
#include <QPointF>

class RadialGraph: public QFrame {
  Q_OBJECT;
public:
  RadialGraph(QWidget *parent=0);
  virtual void paintEvent(class QPaintEvent *);
  void setColors(QColor major, QColor minor);
public:
  QPointF toScreen(double x, double y) const;
  double toScreenX(double x) const;
  double toScreenY(double x) const;
private:
  int hMargin;
  int vMargin;
  QColor majorColor;
  QColor minorColor;
};

#endif
