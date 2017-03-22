// gfx/ellipse.h - This file is part of VScope.
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

// ellipse.h - visual representation of an ROI

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <QWidget>
#include <QPen>
#include <base/xyrra.h>

class Ellipse: public QWidget {
  Q_OBJECT;
 public:
  Ellipse(QWidget *parent);
  /*:F constructor
   *:N The ellipse is undefined until setShape is called.
   */
  virtual ~Ellipse();
  void setShape(XYRRA const &xyrra);
  /*:F setShape
   *:D Sets new parameters for this ellipse.
   *:N Forces redraw.
   */
  XYRRA const &getShape() const;
  /*:F getShape
   *:D Retrieve current parameters for this ellipse.
   */
  void setPen(QPen const &pen);
  /*:F setPen
   *:D Set the pen to use for drawing this ellipse.
   *:N Forces redraw.
   */
  void startCreate(class QMouseEvent *);
  /*:F startCreate
   *:D Call this in response to a buttonPressEvent that is supposed
       to create a new ellipse, then call our drag() function in response
       to mouseMoveEvents, and finally our complete() function in response
       to a mouseReleaseEvent. That way, the work of shaping and reshaping
       a xyrra-based ellipse is almost entirely done by this class.
  */
  void startMove(class QMouseEvent *);
  /*:F startMove
   *:D Like startCreate, but to move a a pre-existing ellipse.
   */
  void startResize(class QMouseEvent *);
  /*:F startResize
   *:D Like startCreate, but to resize a a pre-existing ellipse.
   */
  void startRotate(class QMouseEvent *, bool far=false);
  /*:F startRotate
   *:D Like startCreate, but to rotate a a pre-existing ellipse.
       If FAR=true, rotates around farthest point, else around center.
   */
  void startRotSize(class QMouseEvent *);
  /*:F startRotSize
   *:D Like startRotSize, but to rotate/resize a a pre-existing ellipse.
   */
  void drag(class QMouseEvent *);
  /*:F drag
   *:D Call this from your mouseMoveEvent function so we can update our
       current action (create/move/resize/rotate).
  */
  void complete(class QMouseEvent *);
  /*:F complete
   *:D Call this from your mouseReleaseEvent function to complete our
       current action (create/move/resize/rotate). Once this function
       returns, you probably want to call getShape to find out what happened.
  */
  static void setNPoints(int n);
  /*:F setNPoints
   *:D Set the number of points to use for drawing ellipses.
   *:N Ellipses are drawn as QPolygonFs.
   */
 protected:
  virtual void paintEvent(class QPaintEvent *);
private:
  enum Mode { Nop, Create, Move, Resize, Rotate, RotSize };
private:
  void startDrag(class QMouseEvent *e, Mode m);
  /*:F startDrag
   *:D Core code for starting any kind of drag.
   */
private:
  XYRRA xyrra;
  QPen pen;
  Mode mode;
  static int nPoints;
  double x0, y0; // origin of drag
  XYRRA xyrra0; // ellipse as it was before drag
  double omega, theta; // used for Resize dragging
  bool far; // used for Rotate dragging
};

#endif
