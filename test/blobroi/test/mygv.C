// mygv.C

#include "mygv.H"
#include <QMouseEvent>
#include <QKeyEvent>

MyGV::MyGV(class QGraphicsScene *scene, QWidget *parent):
  QGraphicsView(scene, parent) {
  setFocusPolicy(Qt::StrongFocus);
  setTransformationAnchor(AnchorUnderMouse);
}

MyGV::~MyGV() {
}

void MyGV::mousePressEvent(QMouseEvent *e) {
  QGraphicsView::mousePressEvent(e);
  QPoint p = e->pos();
  QPointF s = mapToScene(p);
  printf("mousePress (b=%i) at (x,y) = (%g,%g)\n",e->button(),s.x(),s.y());
}

void MyGV::mouseMoveEvent(QMouseEvent *e) {
  QGraphicsView::mouseMoveEvent(e);
  QPoint p = e->pos();
  QPointF s = mapToScene(p);
  printf("mouseMoved to       (x,y) = (%g,%g)\n",s.x(),s.y());
  cursorpos = s;
}

void MyGV::mouseReleaseEvent(QMouseEvent *e) {
  QGraphicsView::mouseReleaseEvent(e);
  QPoint p = e->pos();
  QPointF s = mapToScene(p);
  printf("mouseRelease (b=%i) at (x,y) = (%g,%g)\n",e->button(),s.x(),s.y());
}

void MyGV::keyPressEvent(QKeyEvent *e) {
  printf("keypressevent\n");
  QString s = e->text();
  if (s=="+") {
    printf("zoom in\n");
    scale(2,2);
  } else if (s=="-") {
    scale(0.5,0.5);
  } else {
    Qt::KeyboardModifiers m = e->modifiers();
    double scale = 1;
    if (m & Qt::ShiftModifier)
      scale = 5;
    else if (m & Qt::ControlModifier)
      scale = 1/5.;
    switch (e->key()) {
    case Qt::Key_Left:
      shift(-scale,0);
      break;
    case Qt::Key_Right:
      shift(scale,0);
      break;
    case Qt::Key_Up:
      shift(0,-scale);
      break;
    case Qt::Key_Down:
      shift(0,scale);
    }
  }
}

void MyGV::shift(double dx, double dy) {
  printf("shift %g %g\n",dx,dy);
  QTransform t = transform();
  double xscale = t.m11();
  double yscale = t.m22();
  setTransformationAnchor(NoAnchor);
  translate(-50*dx/xscale, -50*dy/yscale);
  setTransformationAnchor(AnchorUnderMouse);
}
