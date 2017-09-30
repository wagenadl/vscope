// acq/focus.h - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// focus.h

#ifndef FOCUS_H

#define FOCUS_H

#include <QFrame>
#include <QWidget>
#include <base/types.h>
#include <base/enums.h>

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
public:
  Focus(QWidget *parent);
  virtual ~Focus();
  void contactButtons(class guiRoot *gui);
  void activate(bool quietIfAlready=false);
  void deactivate(bool quietIfAlready=false);
  QString getCamA() const;
  QString getCamB() const;
protected:
  void resizeEvent(class QResizeEvent *);
public slots:
  void setCams(QString idA, QString idB);
  void setCamA(QString idA);
  void setCamB(QString idB);
  void setViewMode(enum FOCUSMODE vm);
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
  QString camIDA, camIDB;
  class Camera *camA, *camB;
  class CCDImage *left, *right;
  class CCDImage *hiddenA, *hiddenB;
  class CCDConfig *cfgA, *cfgB;
  class QTimer *timer;
  bool isActive;
  enum FOCUSMODE viewMode;
  uint16_t const *frmA;
  uint16_t const *frmB; // we do not own these
  int npix, X, Y; // eventually, these should be separate for A and B
  bool isfirstA, isfirstB;
  class guiButton *butFocus[2];
  class guiButton *butIntensity[2];
  class guiButton *butRotate;
  class guiButton *butShiftX;
  class guiButton *butShiftY;
  class guiButton *butScale;
  bool flipXA, flipXB;
  bool flipYA, flipYB;
  double exp_msA, exp_msB;
};

#endif
