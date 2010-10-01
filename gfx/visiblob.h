// visiblob.h

#ifndef VISIBLOB_H

#define VISIBLOB_H

#include <QWidget>
#include <QPen>
#include <base/polyblob.h>

class VisiBlob: public QWidget {
  /*:C VisiBlob
   *:D Visual representation of a PolyBlob
   */
  Q_OBJECT;
public:
  VisiBlob(QWidget *parent);
  /*:F constructor
   *:D The blob is undefined unil setShape is called.
   */
  virtual ~VisiBlob();
  void setShape(PolyBlob *pb, bool redraw=true);
  /*:F setShape
   *:D Makes this VisiBlob represent the given PolyBlob.
   *:N This does not make a copy; the PolyBlob must remain available, and
       may be modified.
   *:N Forces redraw unless 2nd arg is false.
   */
  void setZoom(double ax, double bx, double ay, double by);
  /*:F setZoom
   *:D Sets a transformation between blob and screen, straight for use
       by PolyBlob's paint().
   *:N Forces redraw.
  */
  void setPen(QPen const &pen);
  /*:F setPen
   *:D Set the pen to use for drawing this blob.
   *:N Forces redraw.
   */
  void paintEvent(class QPaintEvent *);
  void startCreate(PolyBlob *dest, class QMouseEvent *);
  /*:F startCreate
   *:D Call this in response to a buttonPressEvent that is supposed
       to create a new blob, then call our drag() function in response
       to mouseMoveEvents, and finally our complete() function in response
       to a mouseReleaseEvent. That way, the work of shaping and reshaping
       a polyblob-based ROI is almost entirely done by this class.
  */
  void startAdjust(class QMouseEvent *);
  /*:F startAdjust
   *:D Like startCreate, but to adjust a a pre-existing blob.
   */
  void startMove(class QMouseEvent *);
  /*:F startMove
   *:D Like startAdjust, but moves the blob as a whole.
   */
  void drag(class QMouseEvent *);
  /*:F drag
   *:D Call this from your mouseMoveEvent function so we can update our
       current action (create/adjust).
  */
  bool complete(class QMouseEvent *);
  /*:F complete
   *:D Call this from your mouseReleaseEvent function to complete our
       current action (create/adjust). Once this function returned, the
       polyblob can be examined.
   *:N Returns true iff a drag was actually in progress.
  */
  bool dragging() const;
  /*:F dragging
   *:D Returns true iff currently dragging
   */
public:
  virtual void mousePressEvent(class QMouseEvent *);
  virtual void mouseReleaseEvent(class QMouseEvent *);
  virtual void mouseMoveEvent(class QMouseEvent *);
  virtual void mouseDoubleClickEvent(class QMouseEvent *);
private:
  PolyBlob *pb;
  double ax, bx, ay, by;
  QPen pen;
  enum DragType {
    CREATE,
    DISTORT,
    MOVE,
  } dragtype;
  bool indrag;
  QPolygonF *newpoly;
  QPointF startScreen; // start of drag, screen coords, only used for MOVE.
  QPointF startBlob;
  /*:v startBlob
   *:D Start point of drag, blob coords. Meaning depends on drag type.
       For MOVE, it is the center of the blob.
  */
};

#endif
