// transform.cpp

#include "transform.h"

Transform::Transform() {
  ax=ay=1;
  bx=by=0;
}

Transform &Transform::operator=(Transform const &t) {
  ax=t.ax;
  ay=t.ay;
  bx=t.bx;
  by=t.by;
  return *this;
}

Transform Transform::unit() {
  return Transform();
}

Transform Transform::scaling(double x, double y) {
  return Transform().scale(x, y);
}

Transform Transform::scaling(double xy) {
  return Transform().scale(xy);
}

Transform Transform::translation(double dx, double dy) {
  return Transform().translate(dx,dy);
}

Transform Transform::hreflection(double xcoord) {
  return Transform().flipx(xcoord);
}

Transform Transform::vreflection(double ycoord) {
  return Transform().flipy(ycoord);
}

Transform &Transform::scale(double x, double y) {
  ax*=x;
  ay*=y;
  bx*=x;
  by*=y;
  return *this;
}

Transform &Transform::scale(double xy) {
  return scale(xy,xy);
}

Transform &Transform::translate(double dx, double dy) {
  bx += dx;
  by += dy;
  return *this;
}

Transform &Transform::flipx(double xcoord) {
  // Original transform: x' = ax*x+bx.
  // Now we want x'' = xcoord - x'.
  // So x'' = xcoord - ax*x - bx =: ax' * x + bx'.
  // So ax' = -ax, and bx' = xcoord - bx.
  ax = -ax;
  bx = xcoord - bx;
  return *this;
}

Transform &Transform::flipy(double ycoord) {
  // See flipx for math
  ay = -ay;
  by = ycoord - by;
  return *this;
}

Transform &Transform::invert() {
  // Original transform: x' = ax*x + bx
  // Write as: x = ax'*x' + bx'
  // That is: x = ax'*(ax*x + bx) + bx' = ax'*ax*x + ax'*bx + bx'
  // This must hold for all x, thus: ax'*ax = 1, and ax'*bx = -bx'
  ax = 1./ax;
  bx = -bx*ax;
  ay = 1./ay;
  by = -by*ay;
  return *this;
}

Transform Transform::scaled(double x, double y) const {
  Transform t(*this);
  return t.scaled(x,y);
}

Transform Transform::scaled(double xy) const {
  Transform t(*this);
  return t.scaled(xy);
}

Transform Transform::translated(double dx, double dy) const {
  Transform t(*this);
  return t.translated(dx,dy);
}

Transform Transform::xflipped(double xcoord) const {
  Transform t(*this);
  return t.flipx(xcoord);
}
  
Transform Transform::yflipped(double ycoord) const {
  Transform t(*this);
  return t.flipy(ycoord);
}

Transform Transform::inverse() const {
  Transform t(*this);
  return t.invert();
}

QPoint Transform::operator()(QPoint const &p) const {
  return QPoint(ax*p.x()+bx, ay*p.y()+by);
}

QPointF Transform::operator()(QPointF const &p) const {
  return QPointF(ax*p.x()+bx, ay*p.y()+by);
}

QSize Transform::operator()(QSize const &s) const {
  return QSize(ax*s.width(), ay*s.height());
}

QSizeF Transform::operator()(QSizeF const &s) const {
  return QSizeF(ax*s.width(), ay*s.height());
}

QRect Transform::operator()(QRect const &r) const {
  return QRect(operator()(r.topLeft()),operator()(r.size()));
}

QRectF Transform::operator()(QRectF const &r) const {
  return QRectF(operator()(r.topLeft()),operator()(r.size()));
}

Transform Transform::operator()(Transform const &t) const {
  // We do x'' = ax*x' + bx. t does x' = ax'*x + bx'.
  // We should produce: x'' = ax''*x + bx''.
  // Now: x'' = ax*(ax'*x + bx') + bx = ax*ax'*x + ax*bx' + bx.
  Transform tt;
  tt.ax = ax*t.ax;
  tt.bx = ax*t.bx + bx;
  tt.ay = ay*t.ay;
  tt.by = ay*t.by + by;
  return tt;
}

double Transform::mapx(double x) const {
  return ax*x + bx;
}

double Transform::mapy(double y) const {
  return ay*y + by;
}
