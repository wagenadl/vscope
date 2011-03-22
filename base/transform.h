// transform.h

#ifndef TRANSFORM_H

#define TRANSFORM_H

#include <QRect>
#include <QRectF>
#include <QDomElement>

class Transform {
public:
  Transform();
  Transform &operator=(Transform const &);
  Transform(QDomElement const &);
  void read(QDomElement const &src);
  /*:F read
   *:D src may be a <transform> element or contain a <transform> element.
   */
  void write(QDomElement &dst) const;
  /*:F write
   *:D If DST is a <transform> element, writes into it. Otherwise, appends
       a <transform> element to dst.
  */
  bool operator==(Transform const &) const;
  bool operator!=(Transform const &) const;
public:
  /* The following construct transformations. */
  static Transform unit();
  static Transform scaling(double x, double y);
  static Transform scaling(double xy);
  static Transform translation(double dx, double dy);
  static Transform hreflection(double xcoord);
  static Transform vreflection(double ycoord);
  static Transform inferred(QRect src, QRect dst);
  static Transform inferred(QRectF src, QRectF dst);
  /*:F inferred
   *:D Creates a transformation matrix that maps src onto dst, that is
       Transform::inferred(src,dst)(src) := dst
  */
public:
  /* The following apply the transformation. */
  QPoint operator()(QPoint const &) const;
  QPointF operator()(QPointF const &) const;
  QSize operator()(QSize const &) const;
  QSizeF operator()(QSizeF const &) const;
  // sizes remain positive even on reflection
  QRect operator()(QRect const &) const;
  QRectF operator()(QRectF const &) const;
  Transform operator()(Transform const &) const;
  // If A and B are transforms, and x is a point, then (A(B))(x) := A(B(x)).
  QPointF mapDisplacement(QPointF const &) const; // treats the point as a distance. Result is -ve in case of reflection
  double mapx(double x) const;
  double mapy(double y) const;
  double mapdx(double dx) const;
  double mapdy(double dy) const;
  double maplength(double r) const;
  // Caution: if ever we want to include rotations, then these must go.
  // Note that maplength is always approximate anyway.
public:
  /* The following affect our own transform. */
  /* Example: X.scale(2,3) is the same as X=Transform::scaling(2,3)(X),
     which is not the same as X=X(Transform::scaling(2,3)). */
  Transform &scale(double x, double y);
  Transform &scale(double xy);
  Transform &translate(double dx, double dy);
  Transform &flipx(double xcoord);
  Transform &flipy(double ycoord);
  Transform &invert();
public:
  /* The following create copies. */
  Transform scaled(double x, double y) const;
  Transform scaled(double xy) const;
  Transform translated(double dx, double dy) const;
  Transform xflipped(double xcoord) const;
  Transform yflipped(double ycoord) const;
  Transform inverse() const;
public:
  /* Informational functions */
  bool reflectsX() const;
  bool reflectsY() const;
private:
  double ax, ay; // scale
  double bx, by; // shift
  // Mapping is x <- ax + bx*x.
  //            y <- ay + by*y.
  // We currently do not support rotation.
public:
  static QPoint intPixel(QPointF const &p) {
    return QPoint(int(p.x()), int(p.y()));
  }
  static QPointF pixelCenter(QPoint const &p) {
    return QPointF(p.x()+0.5, p.y()+0.5);
  }
  static QPointF pixelCenter(int x, int y) {
    return QPointF(x+0.5, y+0.5);
  }
  /* A pixel is nominally a rectangle QRect(x,y,1,1).
     intPixel returns the integer indices of a pixel, i.e., the coordinates
     of its top left.
     pixelCenter returns the coordinates of the center of a pixel.
  */
};



#endif
