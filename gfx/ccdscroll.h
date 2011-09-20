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
  void setNFrames(int n);
  void newCCDData();
signals:
  void gotoFrame(int n);
protected slots:
  void sliderMove(double x0);
private:
  int oldpos;
};

#endif
