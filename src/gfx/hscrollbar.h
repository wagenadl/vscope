// hscrollbar.h

#ifndef HSCROLLBAR_H

#define HSCROLLBAR_H

#include <QFrame>

class HScrollBar: public QFrame {
  Q_OBJECT;
public:
  HScrollBar(QWidget *parent);
  virtual ~HScrollBar();
  void setRange(double x0, double w);
  void setSlider(double x0, double w, bool constrain=false);
signals:
  void moved(double x0);
  void moving(double x0);
protected:
  virtual void paintEvent(class QPaintEvent *);
  virtual void mousePressEvent(class QMouseEvent *);
  virtual void mouseMoveEvent(class QMouseEvent *);
  virtual void mouseReleaseEvent(class QMouseEvent *);
private:
  double x0_range;
  double w_range;
  double x0_slider;
  double w_slider;
  double x0_dragstart;
  double x0_dragcurrent;
  int xs0_dragstart;
  int xs0_dragcurrent;
  bool dragging;
};

#endif
