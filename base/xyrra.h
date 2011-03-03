// xyrra.h - elliptical ROI

#ifndef Xyrra_H
#define Xyrra_H

#include <QDomElement>
#include <QRectF>

class XYRRA {
public:
  XYRRA(double x0=0, double y0=0, double R=0, double r=0, double a=0):
    x0(x0), y0(y0), R(R), r(r), a(a) {
    if (r<=0)
      r=R;
  }
  XYRRA(QPointF xy0, double R=0, double r=0, double a=0);
  XYRRA(QDomElement doc);
  void read(QDomElement doc);
  void write(QDomElement doc) const;
  template <class DATATYPE> void fill(DATATYPE *data, int X, int Y, 
                                      DATATYPE value=1) const;
  // fill(data, X, Y, value) where DATA points to an Y*X sized array,
  // fills pixels internal to the ellipse with VALUE.

  double findNearest(double x, double y) const;
  /*:F findNearest
   *:D Returns the omega angle for the point on the ellipse nearest (x,y).
   */
  double omegaToX(double omega) const;
  double omegaToY(double omega) const;
  bool inside(double x, double y, double marg=0) const;
  /*:F inside
   *:D Returns true if the point (x,y) lies inside the ellipse.
   *:D If MARG>0, that number of pixels is added to the local radius
       for the comparison.
  */
  double parallel(double omega) const;
  /*:F parallel
   *:D Find parallel to ellipse at a given point.
       theta = parallel(omega) finds the angle of the parallel line to
       the ellipse at its point OMEGA.
  */
  double maxY() const;
  /*:F maxY
   *:D Returns the ellipse's max. Y coordinate
   */
  double maxX() const;
  /*:F maxX
   *:D Returns the ellipse's max. X coordinate
   */
  QRectF bbox() const;
  void paint(class QPainter *pntr, int nPoints=32) const;
  /*:F paint
   *:D Renders this ellipse with the current pen onto a drawable using
       the given painter. The ellipse is rendered as a polygon with
       nPoints points around the circumference.
  */
  bool operator==(XYRRA const &other);
  XYRRA &transform(class Transform const &t);
  XYRRA transformed(class Transform const &t) const;
public:
  double x0, y0;
  double R, r;
  double a;
};

#endif
