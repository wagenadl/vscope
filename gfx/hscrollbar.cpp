// hscrollbar.cpp

#include "hscrollbar.h"
#include <QPainter>
#include <QMouseEvent>
#include <base/dbg.h>

HScrollBar::HScrollBar(QWidget *parent): QFrame(parent) {
  x0_range=0;
  w_range=10;
  x0_slider=0;
  w_slider=1;
  dragging = false;
}

HScrollBar::~HScrollBar() {
}

void HScrollBar::setRange(double x0, double w) {
  Dbg() << "HScrollBar::setRange("<<x0<<","<<w<<")";
  x0_range = x0;
  w_range = w;
  if (w_slider>w_range)
    w_slider=w_range;
  if (x0_slider<x0_range)
    x0_slider=x0_range;
  if (x0_slider>x0_range+w_range-w_slider)
    x0_slider=x0_range+w_range-w_slider;
  update();
}

void HScrollBar::setSlider(double x0, double w, bool constrain) {
  Dbg() << "HScrollBar::setSlider("<<x0<<","<<w<<")";
  x0_slider = x0;
  w_slider = w;
  if (constrain) {
    if (w_slider>w_range)
      w_slider=w_range;
    if (x0_slider<x0_range)
      x0_slider=x0_range;
    if (x0_slider>x0_range+w_range-w_slider)
      x0_slider=x0_range+w_range-w_slider;
  } else {
    if (x0_range>x0_slider)
      x0_range = x0_slider;
    if (x0_range+w_range<x0_slider+w_slider)
      w_range=x0_slider+w_slider-x0_range;
  }
  Dbg() << "->("<<x0_slider<<","<<w_slider<<")";
  update();
}

void HScrollBar::paintEvent(class QPaintEvent *e) {
  QFrame::paintEvent(e);
  QPainter p(this);
  QBrush b_bg(QColor("#aaaaaa"));
  QPalette p_bg(QColor("#aaaaaa"));
  QBrush b_fg(QColor("#eeeeee"));
  QPalette p_fg(QColor("#eeeeee"));
  qDrawShadePanel(&p,0,0,width(),height(),p_bg,true,2,&b_bg);
  double x0 = dragging ? x0_dragcurrent : x0_slider;
  int xleft = 2+int((width()-4) * (x0-x0_range) / w_range);
  int w = int((width()-4)*w_slider/w_range);

  if (w<6) {
    xleft-=(6-w)/2;
    w=6;
    if (xleft<0)
      xleft=0;
    if (xleft+w > width())
      xleft = width() - w;
  }
  int y0=4;
  int h = height()-8;
  if (h<0) {
    y0=0;
    h=height();
  }

  //dbg("scrollbar: range=(%g+%g) slider=(%g+%g) xl=%i w=%i y0=%i h=%i",
  //    x0_range,w_range, x0_slider,w_slider, xleft, w, y0, h);

  qDrawShadePanel(&p,xleft,y0,w,h, p_fg, false, 2, &b_fg);
}

void HScrollBar::mousePressEvent(QMouseEvent *e) {
  int xs = e->pos().x();
  double x = x0_range + (xs-2)*w_range/(width()-4);
  //dbg("hsb:mousepress: xs=%i x=%g rng=(%g+%g) sli=(%g+%g)",
  //    xs,x,x0_range,w_range,x0_slider,w_slider);
  if (x<x0_slider) {
    x = x0_slider - w_slider;
    if (x<x0_range)
      x = x0_range;
    if (x!=x0_slider) {
      x0_slider = x;
      update();
      emit moved(x);
    }
  } else if (x>=x0_slider+w_slider) {
    x = x0_slider + w_slider;
    if (x >= x0_range+w_range-w_slider)
      x = x0_range + w_range - w_slider;
    if (x!=x0_slider) {
      x0_slider = x;
      update();
      emit moved(x);
    }
  } else {
    x0_dragcurrent = x0_dragstart = x0_slider;
    xs0_dragcurrent = xs0_dragstart = e->pos().x();
    dragging = true;
  }
}

void HScrollBar::mouseMoveEvent(QMouseEvent *e) {
  if (!dragging)
    return;

  xs0_dragcurrent = e->pos().x();

  x0_dragcurrent = x0_dragstart +
    (xs0_dragcurrent-xs0_dragstart)*w_range/(width()-4);

  if (x0_dragcurrent<0)
    x0_dragcurrent = 0;
  if (x0_dragcurrent >= x0_range + w_range - w_slider)
    x0_dragcurrent = x0_range + w_range - w_slider;
  //dbg("hsb:drag xs0=%i/%i x0=%g/%g",xs0_dragstart,xs0_dragcurrent,
  //    x0_dragstart,x0_dragcurrent);
  update();
  emit moving(x0_dragcurrent);
}

void HScrollBar::mouseReleaseEvent(QMouseEvent *e) {
  if (!dragging)
    return;
  
  mouseMoveEvent(e);
  dragging = false;
  x0_slider = x0_dragcurrent;
  emit moved(x0_slider);
}
