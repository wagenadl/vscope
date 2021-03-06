// base/transform.cpp - This file is part of VScope.
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

// transform.cpp

#include "transform.h"
#include <math.h>
#include <base/exception.h>

Transform::Transform() {
  ax=ay=1;
  bx=by=0;
}

Transform::Transform(QDomElement const &e) {
  read(e);
}

bool Transform::operator==(Transform const &t) const {
  return ax==t.ax && bx==t.bx && ay==t.ay && by==t.by;
}

bool Transform::operator!=(Transform const &t) const {
  return ax!=t.ax || bx!=t.bx || ay!=t.ay || by!=t.by;
}

void Transform::read(QDomElement const &e) {
  if (e.isNull())
    throw Exception("Transform","Cannot read null element");
  if (e.tagName()=="transform") {
    ax = e.attribute("ax").toDouble(0);
    bx = e.attribute("bx").toDouble(0);
    ay = e.attribute("ay").toDouble(0);
    by = e.attribute("by").toDouble(0);
  } else {
    read(e.firstChildElement("transform"));
  }
}

void Transform::write(QDomElement &e) const {
  if (e.isNull())
    throw Exception("Transform","Cannot write to null element");
  if (e.tagName()=="transform") {
    e.setAttribute("ax",QString("%1").arg(ax));
    e.setAttribute("bx",QString("%1").arg(bx));
    e.setAttribute("ay",QString("%1").arg(ay));
    e.setAttribute("by",QString("%1").arg(by));
  } else {
    QDomElement x = e.ownerDocument().createElement("transform");
    e.appendChild(x);
    write(x);
  }
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
  return t.scale(x,y);
}

Transform Transform::scaled(double xy) const {
  Transform t(*this);
  return t.scale(xy);
}

Transform Transform::translated(double dx, double dy) const {
  Transform t(*this);
  return t.translate(dx,dy);
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
  return QSize(fabs(ax)*s.width(), fabs(ay)*s.height());
}

QSizeF Transform::operator()(QSizeF const &s) const {
  return QSizeF(fabs(ax)*s.width(), fabs(ay)*s.height());
}

QPointF Transform::mapDisplacement(QPointF const &p) const {
  return QPointF(ax*p.x(), ay*p.y());
}

QRect Transform::operator()(QRect const &r) const {
  QPoint tl = operator()(r.topLeft());
  QSize s = operator()(r.size());
  if (ax<0) 
    tl.setX(tl.x()-s.width());
  if (ay<0)
    tl.setY(tl.y()-s.height());
  return QRect(tl, s);
}

QRectF Transform::operator()(QRectF const &r) const {
  QPointF tl = operator()(r.topLeft());
  QSizeF s = operator()(r.size());
  if (ax<0) 
    tl.setX(tl.x()-s.width());
  if (ay<0)
    tl.setY(tl.y()-s.height());
  return QRectF(tl, s);
}

Transform Transform::operator()(Transform const &t) const {
  // We do x'' = ax*x' + bx. t does x' = ax'*x + bx'.
  // We should produce: x'' = ax''*x + bx''.
  // Now: x'' = ax*(ax'*x + bx') + bx = ax*ax'*x + ax*bx' + bx.
  // So, ax'' = ax*ax', and bx'' = ax*bx' + bx.
  
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

double Transform::mapdx(double dx) const {
  return ax*dx;
}

double Transform::mapdy(double dy) const {
  return ay*dy;
}

double Transform::maplength(double r) const {
  return r*sqrt(ax*ay);
}

Transform Transform::inferred(QRect src, QRect dst) {
  return inferred(QRectF(src), QRectF(dst));
}

Transform Transform::inferred(QRectF src, QRectF dst) {
  double x0D = dst.left();
  double x0S = src.left();
  double wD = dst.width();
  double wS = src.width();

  double y0D = dst.top();
  double y0S = src.top();
  double hD = dst.height();
  double hS = src.height();

  
  /* We want:
       x0D = ax*x0S + bx
       wD = ax*wS
     and same for y.
     That's really easy!
  */
  Transform t;
  t.ax = wD/wS;
  t.bx = x0D - t.ax*x0S;
  t.ay = hD/hS;
  t.by = y0D - t.ay*y0S;
  
  return t;
}

bool Transform::reflectsX() const {
  return ax<0;
}

bool Transform::reflectsY() const {
  return ay<0;
}

QString Transform::stringRep() const {
  return QString("%1x%2+%3+%4").arg(ax).arg(ay).arg(bx).arg(by);
}

QDebug &operator<<(QDebug &d, Transform const &t) {
  return d << "Transform(" << t.ax << "," << t.bx << ";"
           << t.ay << "," << t.by << ")";
}
