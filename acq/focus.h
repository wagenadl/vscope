// focus.h

#ifndef FOCUS_H

#define FOCUS_H

#include <QFrame>
#include <QWidget>
#include <base/types.h>

class Focus: public QFrame {
  Q_OBJECT;
public:
  /*
    The current version of Focus is perfectly happy to have 0, 1, or 2 cameras.
    However, if it has two cameras, it requires that they have the same size.
    Throwing a "Pixel count mismatch" exception may be avoided in the future
    if I implement a way to map fields of view of two cameras onto each other.
    At a minimum, that would require new "scale", "shiftx", and "shifty"
    variables in the cameras' Connections information.
   */
  enum ViewMode {
    ViewAll,
    ViewA,
    ViewB,
    ViewZoom,
    ViewDiff
  };
public:
  Focus(QWidget *parent);
  virtual ~Focus();
  void contactButtons(class xmlGui *gui);
  void activate();
  void deactivate();
protected:
  void resizeEvent(class QResizeEvent *);
public slots:
  void setCams(QString idA); // automatically finds partner
  void setViewMode(enum ViewMode vm);
  void autoRange();
protected slots:
  void dataAvailable();
protected:
  void newViewMode();
  void reportFocus(int n, uint16_t const *img);
  void reportAlignment();
  double calcdx(int x0, int x1, int y0, int y1, bool ynotx);
  double fitpeak(double *yy, int x0);
  double average(uint16_t const *img,
		 int x0, int x1, int y0, int y1,
		 bool flipx, bool flipy);
  double variance(uint16_t const *img,
		  int x0, int x1, int y0, int y1,
		  double avg,
		  bool flipx, bool flipy);
  double covariance(uint16_t const *imgA, uint16_t const *imgB,
		    int x0, int x1, int y0, int y1,
		    double avgA, double avgB);  
private:
  class Camera *camA, *camB;

  class CCDImage *left, *right;
  class CCDImage *hiddenA, *hiddenB;
  class CCDConfig *cfgA, *cfgB;
  class QTimer *timer;
  bool isActive;
  enum ViewMode viewMode;
  uint16_t const *frmA;
  uint16_t const *frmB; // we do not own these
  int npix, X, Y; // eventually, these should be separate for A and B
  bool isfirstA, isfirstB;
  class xmlButton *butFocus[2];
  class xmlButton *butIntensity[2];
  class xmlButton *butRotate;
  class xmlButton *butShiftX;
  class xmlButton *butShiftY;
  class xmlButton *butScale;
  bool flipXA, flipXB;
  bool flipYA, flipYB;
  double exp_msA, exp_msB;
};

#endif
