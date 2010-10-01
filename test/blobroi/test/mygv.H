// mygv.H

#ifndef MYGV_H

#define MYGV_H

#include <QGraphicsView>

class MyGV: public QGraphicsView {
  Q_OBJECT;
public:
  MyGV(class QGraphicsScene *scene, QWidget *parent=0);
  virtual ~MyGV();
  void mousePressEvent(class QMouseEvent *e);
  void mouseMoveEvent(class QMouseEvent *e);
  void mouseReleaseEvent(class QMouseEvent *e);
  void keyPressEvent(class QKeyEvent *e);
private:
  QPointF cursorpos;
  void shift(double dx, double dy);
};

#endif
