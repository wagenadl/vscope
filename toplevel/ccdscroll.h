// ccdscroll.h

#ifndef CCDSCROLL_H

#define CCDSCROLL_H

#include <gfx/hscrollbar.h>

class CCDScroll: public HScrollBar {
  Q_OBJECT;
public:
  CCDScroll(QWidget *parent);
  virtual ~CCDScroll();			    
public slots:
  void newData();
protected slots:
  void sliderMove(double t_ms);
private:
  void rerange(double t0, double t1, double ival);
private:
  double t0, t1, ival;
};

#endif
